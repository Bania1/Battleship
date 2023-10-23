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
                                strcpy(buffer, "+0k. Usuario conectado\n");
                                send(new_sd, buffer, sizeof(buffer), 0);
                                for (j = 0; j < (numClientes - 1); j++)
                                {

                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "Nuevo Cliente conectado en <%d>", new_sd);
                                    send(arrayClientes[j], buffer, sizeof(buffer), 0);
                                }

                                // Aquí viene mi código (ojo, aquí socket es new_sd)
                                
                                guardarNuevoJugador(jugadores, MAX_CLIENTS, new_sd,numClientes);
                                
                    
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

                                if (jugadores[pos].estado != LOGUEADO)
                                {
                                    // Le envío al jugador que no puede jugar
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: Debes estas LOGUEADO para poder INICIAR-PARTIDA\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else
                                {
                                    // Hago toda la lógica de buscar partida para el jugador con socket i
                                    //
                                    // Busco si hay partidas en el vector de partidas
                                    bool hayPartidas = comprobar_partidas(partidas, MAX_MATCHES);
                                    
                                    // Si hay partidas
                                    if (hayPartidas)
                                    {
                                        bool encontrado = buscarPartida(jugadores, partidas,numPartidas, i, MAX_MATCHES);

                                        if(encontrado)
                                        {
                                            //Partida SEMILLENA encontrada
                                            numPartidas++;
                                            int waiting_player_socket = buscarSocketDisponible(jugadores, MAX_CLIENTS,i);
                                            printf("Partida SEMILLENA encontrada\n");
                                            printf("El jugador %d ha encontrado partida con el jugador %d\n", i, waiting_player_socket);

                                            int pos2 = buscarSocket(jugadores, MAX_CLIENTS, i); //jugador 2
                                            int pos1 = buscarSocket(jugadores, MAX_CLIENTS, waiting_player_socket); //jugador 1

                                            //Le envío al jugador que ha encontrado partida
                                            //Le envío al otro jugador que ha encontrado partida
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+Ok! Encontrada partida SEMILLENA\n");
                                            send(jugadores[pos2].socket,buffer,sizeof(buffer),0);
                                           
                                            /*bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+Ok! Encontrada partida\n");
                                            send(jugadores[pos1].socket,buffer,sizeof(buffer),0);
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+Ok! Encontrada partida\n");
                                            send(jugadores[pos2].socket,buffer,sizeof(buffer),0);*/
                                            
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+Ok! Comienza la partida\n");
                                            send(jugadores[pos1].socket,buffer,sizeof(buffer),0);
                                            
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+Ok! Comienza la partida\n");
                                            send(jugadores[pos2].socket,buffer,sizeof(buffer),0);

                                            //Actualizo el estado de los jugadores
                                            jugadores[pos1].estado = JUGANDO;
                                            jugadores[pos2].estado = JUGANDO; 
                                            
                                            //rellenar_datos_partidas(partidas,jugadores,pos1,pos2,numPartidas);
                                            //numPartidas++;

                                            //Imprimo los vectores de jugadores
                                            printf("\npartida %d con jugador 1: %d y jugador 2: %d \n\n", numPartidas, partidas[numPartidas-1].socket1, partidas[numPartidas-1].socket2);
                                            imprimirJugadores(jugadores, MAX_CLIENTS);
                                            imprimirPartidas(partidas, MAX_MATCHES);                                            
                                        }
                                        else
                                        {
                                            //Partida VACIA encontrada
                                            printf("Partida VACIA encontrada\n");
                                            printf("El jugador %d ha encontrado partida VACIA\n", i);

                                            //Le envío al jugador que ha encontrado partida
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+Ok! Encontrada partida VACIA\n");
                                            send(i,buffer,sizeof(buffer),0);

                                            //Actualizo el estado de los jugadores
                                            //buscar la pos de una partida por socket de un jugador
                                            //printf("i: %d\n", i);

                                            int pos1_1 = buscarSocket(jugadores, MAX_CLIENTS, i);
                                            
                                            jugadores[pos1_1].estado = ESPERANDO;
                                            // printf("pos: %d\n", pos1_1);
                                        }
                                    }
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

                                bool usuario_encontrado = comprobar_usuario(jugadores,MAX_CLIENTS,i,nombre);
                                
                                if (existe && usuario_encontrado==true)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: El usuario ya se encuentra logueado\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else if (existe && usuario_encontrado==false)
                                {
                                    // cambia el estado del jugador a USUARIO_ENCONTRADO
                                    jugadores[pos].estado = USUARIO_ENCONTRADO;
                                    //jugadores[i].estado = USUARIO_ENCONTRADO;

                                    // Guardo el nombre del usuario en el vector de jugadores
                                    strcpy(jugadores[pos].nombre_user, nombre);

                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "+OK! Usuario correcto, introduce la contrasena\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: El usuario no existe\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                            }

                            // Poner contraseña
                            else if (strncmp(buffer, "PASSWORD", 8) == 0)
                            {
                                char password[50];
                                sscanf(buffer, "PASSWORD %s", password);

                                // Obtengo el nombre con el socket
                                int pos = buscarSocket(jugadores, MAX_CLIENTS, i);

                                // Compruebo si el usuario ha introducido antes el nombre de usuario
                                if (jugadores[pos].estado != USUARIO_ENCONTRADO)                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "-Err: Debes introducir el usuario antes de la contraseña\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else
                                {
                                    // Compruebo si la contraseña es correcta
                                    bool correcta = passwordCorrecta(jugadores[pos].nombre_user, password);//pos por i

                                    if (correcta)
                                    {
                                        // cambia el estado del jugador a LOGUEADO
                                        jugadores[pos].estado = LOGUEADO;//pos por i
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+OK! Usuario logueado correctamente\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    else
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-Err: Contraseña incorrecta\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                }
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