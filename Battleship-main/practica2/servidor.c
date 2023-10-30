#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "funciones.h"

/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set *readfds, int *numClientes, int arrayClientes[]);

int main()
{

    /*----------------------------------------------------
        Descriptor del socket y buffer de datos
    -----------------------------------------------------*/
    int sd, new_sd;
    struct sockaddr_in sockname, from;
    char buffer[MSG_SIZE];
    socklen_t from_len;
    fd_set readfds, auxfds;
    int salida;
    int arrayClientes[MAX_CLIENTS];
    int numClientes = 0;
    int numPartidas = 0;
    // contadores
    int i, j, k;
    int recibidos;
    char identificador[MSG_SIZE];
    int on, ret;

    // Creo el vector de jugadores
    struct jugador jugadores[MAX_CLIENTS];
    inicializar_vector_jugadores(jugadores, MAX_CLIENTS);

    // Creo el vector de partidas
    struct partida partidas[MAX_MATCHES];
    inicializar_vector_partidas(partidas, MAX_MATCHES);

    /* --------------------------------------------------
        Se abre el socket
    ---------------------------------------------------*/
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("No se puede abrir el socket cliente\n");
        exit(1);
    }

    // Activaremos una propiedad del socket para permitir· que otros
    // sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    // Esto permite· en protocolos como el TCP, poder ejecutar un
    // mismo programa varias veces seguidas y enlazarlo siempre al
    // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    // quedase disponible (TIME_WAIT en el caso de TCP)
    on = 1;
    ret = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(2065);
    sockname.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1)
    {
        perror("Error en la operación bind");
        exit(1);
    }

    /*---------------------------------------------------------------------
        Del las peticiones que vamos a aceptar sólo necesitamos el
        tamaño de su estructura, el resto de información (familia, puerto,
        ip), nos la proporcionará el método que recibe las peticiones.
    ----------------------------------------------------------------------*/
    from_len = sizeof(from);

    if (listen(sd, 1) == -1)
    {
        perror("Error en la operación de listen");
        exit(1);
    }

    printf("El servidor está esperando conexiones...\n"); // Inicializar los conjuntos fd_set

    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd, &readfds);
    FD_SET(0, &readfds);

    // Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT, manejador);

    Barco barcos1[] = {{4, 'B', 0, 0}, {3, 'B', 0, 1}, {3, 'B', 0, 2}, {2, 'B', 0, 3}, {2, 'B', 0, 4}};
    Barco barcos2[] = {{4, 'B', 0, 0}, {3, 'B', 0, 1}, {3, 'B', 0, 2}, {2, 'B', 0, 3}, {2, 'B', 0, 4}};

    /*-----------------------------------------------------------------------
        El servidor acepta una petición
    ------------------------------------------------------------------------ */
    while (1)
    {

        // Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)

        auxfds = readfds;

        salida = select(FD_SETSIZE, &auxfds, NULL, NULL, NULL);

        if (salida > 0)
        {

            for (i = 0; i < FD_SETSIZE; i++)
            {

                // Buscamos el socket por el que se ha establecido la comunicación
                if (FD_ISSET(i, &auxfds))
                {

                    if (i == sd) // AQUÍ ENTRO CUANDO SE CONECTA UN CLIENTE (TERMINAL) POR PRIMERA VEZ (./cliente)
                    {

                        if ((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1)
                        {
                            perror("Error aceptando peticiones");
                        }
                        else
                        {
                            if (numClientes < MAX_CLIENTS) // MÁS CONCRETAMENTE AQUÍ
                            {
                                arrayClientes[numClientes] = new_sd;
                                numClientes++;
                                FD_SET(new_sd, &readfds);
                                strcpy(buffer, "+Ok. Usuario conectado\n");
                                send(new_sd, buffer, sizeof(buffer), 0);
                                // for (j = 0; j < (numClientes - 1); j++)
                                //{
                                //
                                //     bzero(buffer, sizeof(buffer));
                                //     sprintf(buffer, "Nuevo Cliente conectado en <%d>", new_sd);
                                //     send(arrayClientes[j], buffer, sizeof(buffer), 0);
                                // }

                                // Aquí viene mi código (ojo, aquí socket es new_sd)

                                guardarNuevoJugador(jugadores, MAX_CLIENTS, new_sd, numClientes);
                            }
                            else
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "Demasiados clientes conectados\n");
                                send(new_sd, buffer, sizeof(buffer), 0);
                                close(new_sd);
                            }
                        }
                    }
                    else if (i == 0) // AQUÍ ENTRO SI EL MENSAJE NO VIENE DE UN CLIENTE SINO DEL TECLADO
                    {
                        // Se ha introducido información de teclado
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);

                        // Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                        if (strcmp(buffer, "SALIR\n") == 0)
                        {

                            for (j = 0; j < numClientes; j++)
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "Desconexión servidor\n");
                                send(arrayClientes[j], buffer, sizeof(buffer), 0);
                                close(arrayClientes[j]);
                                FD_CLR(arrayClientes[j], &readfds);
                            }
                            close(sd);
                            exit(-1);
                        }
                        // Mensajes que se quieran mandar a los-1 clientes (implementar)
                    }
                    else // AQUÍ ENTRA EL SERVIDOR CUANDO RECIBE UN MENSAJE DE UN CLIENTE QUE NO ES NUEVO
                    {
                        bzero(buffer, sizeof(buffer));

                        recibidos = recv(i, buffer, sizeof(buffer), 0);

                        if (recibidos > 0)
                        {
                            // COMPRUEBO SI EL MENSAJE DEL CLIENTE ES SALIR
                            if (strcmp(buffer, "SALIR\n") == 0)
                            {
                                salirCliente(i, &readfds, &numClientes, arrayClientes);
                            }

                            // Iniciar partida y buscarlas
                            else if (strncmp(buffer, "INICIAR-PARTIDA", 15) == 0)
                            {
                                printf("El jugador %d quiere jugar\n", i);

                                // Busco al jugador en el vector de jugadores
                                int pos = buscarSocket(jugadores, MAX_CLIENTS, i);

                                if (jugadores[pos].estado == NO_CONECTADO || jugadores[pos].estado == CONECTADO || jugadores[pos].estado == USUARIO_ENCONTRADO)
                                {
                                    // Le envío al jugador que no puede jugar
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: Debes estas LOGUEADO para poder INICIAR-PARTIDA\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else if (jugadores[pos].estado == ESPERANDO || jugadores[pos].estado == JUGANDO)
                                {
                                    // Le envío al jugador que no puede jugar
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: ya estás en una partida\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }

                                else
                                {
                                    // Funcion que busca una partdia libre (primero con uno que ya está esperando y segundo con una vacía)

                                    // Busco si hay partidas en el vector de partidas
                                    int hayPartidas = buscarPartida(jugadores, partidas, numPartidas, i, MAX_MATCHES);

                                    // Tengo que saber si lo he sentado solo o con otro jugador
                                    if (hayPartidas == VACIA)
                                    {
                                        // te toca esperar
                                        printf("El jugador %d ha encontrado partida VACIA\n", i);

                                        // Le envío al jugador que ha encontrado partida
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Encontrada partida VACIA\n");
                                        send(i, buffer, sizeof(buffer), 0);

                                        // Actualizo el estado del jugador
                                        int pos_1 = buscarSocket(jugadores, MAX_CLIENTS, i);
                                        jugadores[pos_1].estado = ESPERANDO; // jugador 1
                                    }
                                    else if (hayPartidas == SEMILLENA)
                                    {
                                        // toda la logica de iniciar la partida

                                        // numPartidas++;//incremento el numero de partidas

                                        // Busco la posicion del jugador 1 ESPERANDO en el vector de jugadores
                                        int waiting_player_socket = buscarSocketDisponible(jugadores, MAX_MATCHES, i);

                                        // Busco la posicion del jugador 2 en el vector de jugadores
                                        int pos1 = buscarSocket(jugadores, MAX_CLIENTS, waiting_player_socket); // pos jugador 1
                                        int pos2 = buscarSocket(jugadores, MAX_CLIENTS, i);                     // pos jugador 2

                                        printf("El jugador %d ha encontrado partida SEMILLENA\n", i);

                                        // Le envío al jugador 2 que ha encontrado partida SEMILLENA
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Encontrada partida SEMILLENA\n");
                                        send(i, buffer, sizeof(buffer), 0);

                                        // Le envío a los dos jugadores que ha comenzado la partida
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Empezamos partida\n");
                                        send(jugadores[pos1].socket, buffer, sizeof(buffer), 0);

                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Empezamos partida\n");
                                        send(jugadores[pos2].socket, buffer, sizeof(buffer), 0);

                                        bzero(buffer, sizeof(buffer));

                                        // Actualizo el estado del jugador
                                        jugadores[pos1].estado = JUGANDO; // jugador 1
                                        jugadores[pos2].estado = JUGANDO; // jugador 2

                                        // Busco la partida
                                        int idPartida = buscarIDPartida(partidas, MAX_MATCHES, jugadores[pos1].socket);

                                        // Inicializo los campos de la partida
                                        partidas[idPartida].disparos1 = 0;
                                        partidas[idPartida].disparos2 = 0;
                                        partidas[idPartida].turno = 1; // le toca al jugador 1, si vale 2 le toca al 2
                                        partidas[idPartida].turno2 = 0;

                                        // Creo los tableros
                                        //  Crear los barcos, cuadrículas y jugar
                                        // Barco barcos1[] = {{4, 'B', 0, 0}, {3, 'B', 0, 1}, {3, 'B', 0, 2}, {2, 'B', 0, 3}, {2, 'B', 0, 4}};
                                        // Barco barcos2[] = {{4, 'B', 0, 0}, {3, 'B', 0, 1}, {3, 'B', 0, 2}, {2, 'B', 0, 3}, {2, 'B', 0, 4}};

                                        inicializarCuadricula(&partidas[idPartida].tableroBarcos1);
                                        inicializarCuadricula(&partidas[idPartida].tableroBarcos2);
                                        // imprimirCuadricula(&partidas[idPartida].tableroBarcos1);
                                        // imprimirCuadricula(&partidas[idPartida].tableroBarcos2);

                                        // Colocar los barcos en las cuadrículas
                                        // mostrar partidas
                                        // for(int z=0;z<MAX_MATCHES;z++){
                                        //    printf("id partida %d\n", idPartida);
                                        //    printf("partida %d con jugador 1: %d y jugador 2: %d \n", z, partidas[z].socket1, partidas[z].socket2);
                                        //}

                                        for (int h = 0; h < 5; h++)
                                        {
                                            colocarBarco(&partidas[idPartida].tableroBarcos1, &barcos1[h], 0);
                                            colocarBarco(&partidas[idPartida].tableroBarcos2, &barcos2[h], 1);
                                        }

                                        // Funcion que convierta una matriz en cadena de caracteres
                                        //  A,A,A,A,B,B;A,A,A,B,B,B,B;
                                        bzero(buffer, sizeof(buffer));
                                        char *cadenaBarcos1 = matrizBarcosToString(&partidas[idPartida].tableroBarcos1);
                                        char *cadenaBarcos2 = matrizBarcosToString(&partidas[idPartida].tableroBarcos2);

                                        // Envio los tableros a cada jugador
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, cadenaBarcos1);
                                        send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, cadenaBarcos2);
                                        send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                        // Imprimo las cuadriculas en el servidor
                                        imprimirCuadricula(&partidas[idPartida].tableroBarcos1);
                                        imprimirCuadricula(&partidas[idPartida].tableroBarcos2);

                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. es tu turno\n");
                                        send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. es turno del contrincante\n");
                                        send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                        // Imprimo los vectores de jugadores para probar
                                        // printf("\npartida %d con jugador 1: %d y jugador 2: %d \n\n", numPartidas, partidas[numPartidas].socket1, partidas[numPartidas].socket2);
                                        // imprimirJugadores(jugadores, MAX_CLIENTS);
                                        // printf("\n\n");
                                        // imprimirPartidas(partidas, MAX_MATCHES);
                                    }
                                    else
                                    {
                                        // Le envío al jugador que no puede jugar
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-Err: No hay partidas disponibles\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                }
                            }
                            else if (strncmp(buffer, "DISPARO", 7) == 0)
                            {
                                char col;
                                int fil;

                                if (sscanf(buffer, "DISPARO %c,%d", &col, &fil) == 2)
                                {
                                    // Compruebo el estado del jugador que está disparando
                                    // int pos = buscarSocket(jugadores, MAX_CLIENTS, i);

                                    // Concateno la columna y la fila
                                    char disparo[3];
                                    sprintf(disparo, "%c%d", col, fil);

                                    // int turno = 1; // Comienza el jugador 1

                                    int idPartida = buscarIDPartida(partidas, MAX_MATCHES, i);
                                    int pos1 = buscarSocket(jugadores, MAX_CLIENTS, partidas[idPartida].socket1);
                                    int pos2 = buscarSocket(jugadores, MAX_CLIENTS, partidas[idPartida].socket2);

                                    //printf("pos1: %d\n", pos1);
                                    //printf("pos2: %d\n", pos2);

                                    //imprimirPartidas(partidas, MAX_MATCHES);

                                    // Turno de jugador 1
                                    if (partidas[idPartida].turno == 1 && partidas[idPartida].turno2 == 0 && partidas[idPartida].socket1 == i)
                                    {
                                        if (jugadores[pos1].estado == JUGANDO && jugadores[pos2].estado == JUGANDO) // Si está jugando
                                        {
                                            // Compruebo si el disparo es válido
                                            int disparoValido = validarDisparo(&partidas[idPartida].tableroDisparos1, disparo);

                                            if (disparoValido == 0)
                                            {
                                                // Ya se ha disparado en la coordenada, vuelve a intentarlo
                                                bzero(buffer, sizeof(buffer));
                                                sprintf(buffer, "-Err: Ya has disparado en esa coordenada\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                            else if (disparoValido == -1)
                                            {
                                                // El disparo no es válido
                                                bzero(buffer, sizeof(buffer));
                                                sprintf(buffer, "-Err: Disparo no válido\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                            else
                                            {
                                                // El disparo es válido
                                                // Compruebo si ha tocado a algún barco
                                                int tocado = disparar(&partidas[idPartida].tableroDisparos1, &partidas[idPartida].tableroBarcos2, barcos2, disparo);

                                                if (tocado == 0) // Agua
                                                {
                                                    // No ha tocado a ningún barco
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Agua: %c,%d\n", col, fil);
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 2 que le han disparado
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Te han disparado en: %s\n", disparo);
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 2 que le han disparado
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Es tu turno\n");
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 1 que cambia el turno
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Es turno del contrincante\n");
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                    // Cambio el turno
                                                    partidas[idPartida].turno = 0;
                                                    partidas[idPartida].turno2 = 1;
                                                    // turno = 2;
                                                }
                                                else if (tocado == 1) // Tocado
                                                {
                                                    // Ha tocado a un barco
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Tocado: %c,%d\n", col, fil);
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 2 que le han disparado
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Te han disparado: %s\n", disparo);
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Es tu turno\n");
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Es turno del contrincante\n");
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);
                                                }
                                                else if (tocado == 2) // Hundido
                                                {
                                                    // Ha tocado a un barco
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Hundido: %c,%d\n", col, fil);
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 2 que le han disparado
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Te han disparado en: %s\n", disparo);
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                    if(comprobarGanador(&partidas[idPartida].tableroBarcos2,barcos1)){
                                                        // Le envío al jugador 1 que ha ganado
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Has ganado\n");
                                                        send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                        // Le envío al jugador 2 que ha perdido
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Has perdido\n");
                                                        send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                        // Actualizo el estado de los jugadores
                                                        jugadores[pos1].estado = CONECTADO;
                                                        jugadores[pos2].estado = CONECTADO;

                                                        // Actualizo el estado de la partida
                                                        partidas[idPartida].socket1 = -1;
                                                        partidas[idPartida].socket2 = -1;
                                                        partidas[idPartida].estado = VACIA;
                                                    }
                                                    else
                                                    {
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Es tu turno\n");
                                                        send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Es turno del contrincante\n");
                                                        send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "-Err: No te encuentras en ninguna partida\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                        // hay q mandar de vuelta las cadenas al cliente para que muestre las matrices
                                    }

                                    // Turno de jugador 2
                                    else if (partidas[idPartida].turno2 == 1 && partidas[idPartida].turno == 0 && partidas[idPartida].socket2 == i)
                                    {
                                        if (jugadores[pos1].estado == JUGANDO && jugadores[pos2].estado == JUGANDO) // Si está jugando
                                        {
                                            // Compruebo si el disparo es válido
                                            int disparoValido = validarDisparo(&partidas[idPartida].tableroDisparos2, disparo);

                                            if (disparoValido == 0)
                                            {
                                                // Ya se ha disparado en la coordenada, vuelve a intentarlo
                                                bzero(buffer, sizeof(buffer));
                                                sprintf(buffer, "-Err: Ya has disparado en esa coordenada\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                            else if (disparoValido == -1)
                                            {
                                                // El disparo no es válido
                                                bzero(buffer, sizeof(buffer));
                                                sprintf(buffer, "-Err: Disparo no válido\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                            else
                                            {
                                                // El disparo es válido
                                                // Compruebo si ha tocado a algún barco
                                                int tocado = disparar(&partidas[idPartida].tableroDisparos2, &partidas[idPartida].tableroBarcos1, barcos1, disparo);

                                                if (tocado == 0) // Agua
                                                {
                                                    // No ha tocado a ningún barco
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Agua: %c,%d\n", col, fil);
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 1 que le han disparado
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Te han disparado en: %s\n", disparo);
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 1 que le han disparado
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Es tu turno\n");
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 2 que cambia el turno
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Es turno del contrincante\n");
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                    // Cambio el turno
                                                    partidas[idPartida].turno2 = 0;
                                                    partidas[idPartida].turno = 1;
                                                }
                                                else if (tocado == 1) // Tocado
                                                {
                                                    // Ha tocado a un barco
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Tocado: %c,%d\n", col, fil);
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 1 que le han disparado
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Te han disparado %s\n", disparo);
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Es tu turno\n");
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 1 que cambia el turno
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Es turno del contrincante\n");
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);
                                                }
                                                else if (tocado == 2) // Hundido
                                                {
                                                    // Ha tocado a un barco
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Hundido: %c,%d\n", col, fil);
                                                    send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                    // Le envío al jugador 1 que le han disparado
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. Te han disparado en: %s\n", disparo);
                                                    send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                    if(comprobarGanador(&partidas[idPartida].tableroBarcos1,barcos2)){
                                                        // Le envío al jugador 1 que ha ganado
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Has ganado\n");
                                                        send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                        // Le envío al jugador 2 que ha perdido
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Has perdido\n");
                                                        send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);

                                                        // Actualizo el estado de los jugadores
                                                        jugadores[pos1].estado = CONECTADO;
                                                        jugadores[pos2].estado = CONECTADO;

                                                        // Actualizo el estado de la partida:) MIRA WAEPERA Q ESTOY PROBANDOQUE ME TENGO QUE IR
                                                        //HAGO COMMIT
                                                        partidas[idPartida].socket1 = -1;
                                                        partidas[idPartida].socket2 = -1;
                                                        partidas[idPartida].estado = VACIA;
                                                    }
                                                    else
                                                    {
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Es tu turno\n");
                                                        send(partidas[idPartida].socket2, buffer, sizeof(buffer), 0);

                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Es turno del contrincante\n");
                                                        send(partidas[idPartida].socket1, buffer, sizeof(buffer), 0);
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "-Err: No te encuentras en ninguna partida\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                    }
                                    else
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-Err: No es tu turno\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                }
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: Formato del mensaje de disparo incorrecto\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                            }

                            // Registrar usuario
                            else if (strncmp(buffer, "REGISTRO", 8) == 0)
                            {
                                char usuario[20];
                                char password[20];
                                printf("El usuario con socket %d ha introducido la orden REGISTRO\n", i);
                                // Registro -u juan -p 1234
                                if (sscanf(buffer, "REGISTRO -u %s -p %s", usuario, password) == 2)
                                {
                                    // Compruebo si el usuario ya existe
                                    bool existe = existeUsuario(usuario);

                                    if (existe)
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-Err: El usuario ya existe\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    else
                                    {
                                        // Guardo el usuario en el fichero
                                        guardarUsuario(usuario, password);

                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Usuario registrado correctamente\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                }
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: Formato del mensaje de registro incorrecto\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                            }

                            // Poner nombre de usuario
                            else if (strncmp(buffer, "USUARIO", 7) == 0)
                            {
                                // Guardo el nombre del usuario introducido
                                char nombre[50];
                                sscanf(buffer, "USUARIO %s", nombre);
                                printf("El usuario con socket %d ha introducido la orden USUARIO con el nombre %s\n", i, nombre);

                                // Comrpuebo si existe el ususario en el fichero
                                int pos = buscarSocket(jugadores, MAX_CLIENTS, i);
                                bool existe = existeUsuario(nombre);

                                bool usuario_encontrado = comprobar_usuario(jugadores, MAX_CLIENTS, i, nombre);

                                if (existe && usuario_encontrado == true)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: El usuario ya se encuentra logueado\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else if (existe && usuario_encontrado == false)
                                {
                                    // cambia el estado del jugador a USUARIO_ENCONTRADO
                                    jugadores[pos].estado = USUARIO_ENCONTRADO;

                                    // Guardo el nombre del usuario en el vector de jugadores
                                    strcpy(jugadores[pos].nombre_user, nombre);

                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "+Ok. Usuario correcto, introduce la contrasena\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: El usuario no existe\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                bzero(buffer, sizeof(buffer));
                            }

                            // Poner contraseña
                            else if (strncmp(buffer, "PASSWORD", 8) == 0)
                            {
                                char password[50];
                                sscanf(buffer, "PASSWORD %s", password);

                                // Obtengo el nombre con el socket
                                int pos = buscarSocket(jugadores, MAX_CLIENTS, i);

                                // Compruebo si el usuario ha introducido antes el nombre de usuario
                                if (jugadores[pos].estado != USUARIO_ENCONTRADO)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: Debes introducir el usuario antes de la contraseña\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else
                                {
                                    // Compruebo si la contraseña es correcta
                                    bool correcta = passwordCorrecta(jugadores[pos].nombre_user, password); // pos por i

                                    if (correcta)
                                    {
                                        // cambia el estado del jugador a LOGUEADO
                                        jugadores[pos].estado = LOGUEADO; // pos por i
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Usuario logueado correctamente\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    else
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-Err: Contraseña incorrecta\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                }
                                bzero(buffer, sizeof(buffer));
                            }
                            else
                            {
                                // sprintf(identificador, "<%d>: %s", i, buffer);
                                // bzero(buffer, sizeof(buffer));
                                // strcpy(buffer, identificador);
                                // printf("%s\n", buffer);
                                // for (j = 0; j < numClientes; j++)
                                //     if (arrayClientes[j] != i)
                                //         send(arrayClientes[j], buffer, sizeof(buffer), 0);
                            }
                        }
                        // Si el cliente introdujo ctrl+c
                        if (recibidos == 0)
                        {
                            printf("El socket %d, ha introducido ctrl+c\n", i);
                            // Eliminar ese socket
                            salirCliente(i, &readfds, &numClientes, arrayClientes);
                        }
                    }
                }
            }
        }
    }

    close(sd);
    return 0;
}

void salirCliente(int socket, fd_set *readfds, int *numClientes, int arrayClientes[])
{

    char buffer[250];
    int j;

    close(socket);
    FD_CLR(socket, readfds);

    // Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j] == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j + 1]);

    (*numClientes)--;

    bzero(buffer, sizeof(buffer));
    sprintf(buffer, "Desconexión del cliente <%d>", socket);

    for (j = 0; j < (*numClientes); j++)
        if (arrayClientes[j] != socket)
            send(arrayClientes[j], buffer, sizeof(buffer), 0);
}

void manejador(int signum)
{
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT, manejador);

    // Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}

// un if que es para el caso de registrar un usuario
//  else if (strncmp(buffer, "REGISTRO",8) == 0)
//  {
//      char usuario[20];
//      char password[20];
//      //Regitro -u juan -p 1234
//      sscanf(buffer, "REGISTRO -u %s -p %s", usuario, password)
//  }
