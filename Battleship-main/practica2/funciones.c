#include "funciones.h"

//-----------------Funciones de usuario----------------
//
bool existeUsuario(char *usuario)
{
    FILE *fich;
    char linea[100];
    char *nombre;
    char *password;
    fich = fopen("usuarios.txt", "r");
    if (fich == NULL)
    {
        printf("Error al abrir el fichero\n");
        return false;
    }

    while (fgets(linea, 100, fich))
    {
        nombre = strtok(linea, ",");
        password = strtok(linea, ",");
        if (strcmp(nombre, usuario) == 0)
        {
            fclose(fich);
            return true;
        }
    }
    fclose(fich);
    return false;
}

bool passwordCorrecta(char *usuario, char *password)
{
    FILE *fich;
    char linea[100];
    char *nombre;
    char *passwordFichero;
    fich = fopen("usuarios.txt", "r");
    if (fich == NULL)
    {
        printf("Error al abrir el fichero\n");
        return false;
    }

    while (fgets(linea, 100, fich))
    {
        char *resto;

        nombre = strtok_r(linea, ",", &resto);
        passwordFichero = strtok(resto, "\n");

        if (strcmp(nombre, usuario) == 0)
        {
            if (strcmp(passwordFichero, password) == 0)
            {
                fclose(fich);
                return true;
            }
        }
    }
    fclose(fich);
    return false;
}

// Funcion que controla que no se repitan los usuarios
// True si se repite
bool comprobar_usuario(struct jugador *jugadores, int tam, int socket, char *nombre)
{
    // int cont=0;
    for (int i = 0; i < tam; i++)
    {
        if ((jugadores[i].estado == LOGUEADO || jugadores[i].estado == JUGANDO || jugadores[i].estado == ESPERANDO) && (strcmp(jugadores[i].nombre_user, nombre) == 0))
        {
            // cont++;
            return true;
        }
        // esta parte hay q implementar que cuando un jugador haga ^c lo ponga a no_conectado para q cuando vuelva a entrar le deje, eso o borrarlo del struct de jugadores
        else if (jugadores[i].estado == NO_CONECTADO && (strcmp(jugadores[i].nombre_user, nombre) == 0))
        {
            return false;
        }
    }
    /*if(cont>1)
    {
        return true;
    }
    else
    {
        return false;
    }*/
    return false;
}

// Me falta hacer la funcion que pida el nombre del usuario que lo introduce

//Recibe el nombre del usuario y la contraseña y lo guarda en el fichero
void guardarUsuario(char *usuario, char *password)
{
    FILE *fich;
    fich = fopen("usuarios.txt", "a");
    if (fich == NULL)
    {
        printf("Error al abrir el fichero\n");
        return;
    }
    fprintf(fich, "%s,%s\n", usuario, password);
    fclose(fich);
}

//-----------------Funciones partidas-------------------
//
void inicializar_vector_partidas(struct partida *partidas, int tam)
{
    for (size_t i = 0; i < tam; i++)
    {
        partidas[i].socket1 = NO_CONECTADO;
        partidas[i].socket2 = NO_CONECTADO;

        partidas[i].estado = VACIA;
    }
}

bool comprobar_partidas(struct partida *partidas, int tam)
{
    for (size_t i = 0; i < tam; i++)
    {
        if (partidas[i].estado == VACIA || partidas[i].estado == SEMILLENA)
        {
            return true;
        }
    }
    return false;
}

// Esta funcion busca las partidas disponibles
int buscarPartida(struct jugador *user, struct partida *partidas, int numPartida, int socket, int tam)
{
    // Busca si hay un jugador en una partida
    for (size_t i = 0; i < tam; i++)
    {
        if (partidas[i].estado == SEMILLENA)
        {
            partidas[i].socket2 = socket; // JUGADOR 2
            partidas[i].estado = INICIADA;
            return SEMILLENA;
        }
    }
    // Busca una partida VACIA
    for (size_t i = 0; i < tam; i++)
    {
        if (partidas[i].estado == VACIA)
        {
            partidas[i].socket1 = socket; // JUGADOR 1
            partidas[i].estado = SEMILLENA;
            return VACIA;
        }
    }
    // Si no hay partidas disponibles
    return COMPLETA;

    // Si el jugador esta en una partida
    /*for(size_t i = 0; i < tam; i++)
    {
        if(partidas[i].estado == SEMILLENA)
        {
            // partidas[i].socket2 = socket; //JUGADOR 2
            // partidas[i].estado = INICIADA;
            return true;
        }
    } */
    // if(partidas[numPartida].estado==SEMILLENA)
    // {
    //     partidas[numPartida].socket2 = socket; //JUGADOR 2
    //     partidas[numPartida].estado = INICIADA;
    //     return ;
    // }
    // else if(partidas[numPartida].estado==VACIA)
    // {
    //     partidas[numPartida].socket1 = socket; //JUGADOR 1
    //     partidas[numPartida].estado = SEMILLENA;
    //     return false;
    // }
    /*    //Si no hay partidas semillena, se crea una nueva
    for(size_t i = 0; i < tam; i++)
    {
        if(partidas[i].estado == VACIA)
        {
            //partidas[i].socket1 = socket; //JUGADOR 1
            partidas[i].estado = SEMILLENA;
            return false;
        }
    }

    return false; */
}

/*void rellenar_datos_partidas(struct partida * partidas, struct jugador * jugadores, int pos1, int pos2, int num_partida)
{
    partidas[num_partida].socket1 = jugadores[pos1].socket;
    partidas[num_partida].socket2 = jugadores[pos2].socket;
    partidas[num_partida].estado = INICIADA;
}*/

// void actualizarEstado_partida(struct partida * partidas, struct jugador * jugadores, int tam)
// {
//     for(size_t i = 0; i < tam; i++)
//     {
//         if(partidas[i].estado == VACIA)
//         {
//             if(jugadores[buscarSocket(jugadores, tam, partidas[i].socket1)].estado == NO_CONECTADO || jugadores[buscarSocket(jugadores, tam, partidas[i].socket2)].estado == NO_CONECTADO)
//             {
//                 partidas[i].estado = NO_CONECTADO;
//                 jugadores[buscarSocket(jugadores, tam, partidas[i].socket1)].estado = NO_CONECTADO;
//                 jugadores[buscarSocket(jugadores, tam, partidas[i].socket2)].estado = NO_CONECTADO;
//             }
//         }
//     }
// }

// Ver si es necesario
//  void crearPartida(struct partida * partidas, struct jugador * jugadores, int socket, int tam)
//  {
//      for(size_t i = 0; i < tam; i++)
//      {
//          if(partidas[i].estado == NO_CONECTADO)
//          {
//              partidas[i].socket1 = socket;
//              partidas[i].estado = ESPERANDO;
//              jugadores[buscarSocket(jugadores, tam, socket)].estado = ESPERANDO;
//              return;
//          }
//      }
//  }

//-----------------Funciones de servidor----------------
//
void inicializar_vector_jugadores(struct jugador *jugadores, int tam)
{
    for (int i = 0; i < tam; i++)
    {
        jugadores[i].socket = -1;
        jugadores[i].estado = NO_CONECTADO;
    }
}

void guardarNuevoJugador(struct jugador *jugadores, int tam, int socket, int pos)
{
    /*for(int i=0; i<tam; i++)
    {
        if(jugadores[i].estado == NO_CONECTADO)
        {
            jugadores[i].socket = socket;
            jugadores[i].estado = CONECTADO;
        }
    }*/
    jugadores[pos - 1].socket = socket;
    jugadores[pos - 1].estado = CONECTADO;
}

int buscarSocket(struct jugador *jugadores, int tam, int socket)
{
    for (int i = 0; i < tam; i++)
    {
        if (jugadores[i].socket == socket)
        {
            return i;
        }
    }
    return -1;
}

int buscarSocketDisponible(struct jugador *jugadores, int tam, int pos)
{
    for (int i = 0; i < tam; i++)
    {
        if (jugadores[i].estado == ESPERANDO && jugadores[i].socket != pos)
        {
            return jugadores[i].socket;
        }
    }
    return -1;
    // for(int i=0; i<tam; i++)
    // {
    //     if(partidas[i].socket2 == socket)
    //     {
    //         return partidas[i].socket1;
    //     }
    // }
}

void imprimirJugadores(struct jugador *jugadores, int tam)
{
    for (int i = 0; i < tam; i++)
    {
        printf("Socket: %d\n", jugadores[i].socket);
        printf("Estado: %d\n", jugadores[i].estado);
        printf("Nombre: %s\n", jugadores[i].nombre_user);
    }
}

void imprimirPartidas(struct partida *partidas, int tam)
{
    for (int i = 0; i < tam; i++)
    {
        printf("Socket1: %d\n", partidas[i].socket1);
        printf("Socket2: %d\n", partidas[i].socket2);
        printf("Estado: %d\n", partidas[i].estado);
    }
}

int buscarIDPartida(struct partida *partidas, int tam, int socket_a_buscar)
{
    for (size_t i = 0; i < tam; i++)
    {
        if (partidas[i].socket1 == socket_a_buscar || partidas[i].socket2 == socket_a_buscar)
        {
            return i;
        }
    }
    return -1;
}

void imprimirCuadriculaBuffer(Cuadricula* cuadricula, char * buffer) {
    // Inicializa el búfer como una cadena vacía
    buffer[0] = '\0';

    // Agrega la primera fila con letras de columna al búfer
    //strcat(buffer, "   A B C D E F G H I J\n");

    for (int i = 0; i < FILAS; i++) {
        //char fila[4];
        //snprintf(fila, sizeof(fila), "%2d ", i + 1);
        //strcat(buffer, fila);

        for (int j = 0; j < COLUMNAS; j++) {
            char celda[4];
            snprintf(celda, sizeof(celda), "%c ", cuadricula->tablero[i][j]);
            strcat(buffer, celda);
        }

        strcat(buffer, ";");
    }
}

//funciones del tablero
// Función para mostrar el menú de bienvenida
void mostrarMenuBienvenida() {
    printf("\n=============================");
    printf("\n        Batalla Naval\n");
    printf("=============================\n");
    printf("\n1. Jugar\n");
    printf("2. Salir\n");
    printf("\nElija una opción: ");
}

// Función para inicializar una cuadrícula con agua (~)
void inicializarCuadricula(Cuadricula* cuadricula) {
    /*for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            cuadricula->tablero[i][j] = (char*)malloc(sizeof(char));
            cuadricula->idBarco[i][j] = (int*)malloc(sizeof(int));
        }
    }*/
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            cuadricula->tablero[i][j] = '~';
            cuadricula->idBarco[i][j] = -1;
        }
    }
}

// Función para imprimir una cuadrícula con referencias de números y letras
void imprimirCuadricula(Cuadricula* cuadricula) {
    printf("   A B C D E F G H I J\n");
    for (int i = 0; i < FILAS; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < COLUMNAS; j++) {
            printf("%c ", cuadricula->tablero[i][j]);
        }
        printf("\n");
    }
}

// Función para verificar si un espacio está disponible para colocar un barco
int esEspacioDisponible(Cuadricula* cuadricula, int fila, int columna, int orientacion, int tamano) {
    if (orientacion == 0) {
        if (columna + tamano > COLUMNAS) {
            return 0; // Fuera de la cuadrícula
        }
        for (int i = fila - 1; i <= fila + 1; i++) {
            for (int j = columna - 1; j <= columna + tamano; j++) {
                if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS) {
                    if (cuadricula->tablero[i][j] != '~') {
                        return 0; // Colisión con otro barco
                    }
                }
            }
        }
    } else {
        if (fila + tamano > FILAS) {
            return 0; // Fuera de la cuadrícula
        }
        for (int i = fila - 1; i <= fila + tamano; i++) {
            for (int j = columna - 1; j <= columna + 1; j++) {
                if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS) {
                    if (cuadricula->tablero[i][j] != '~') {
                        return 0; // Colisión con otro barco
                    }
                }
            }
        }
    }
    return 1; // Espacio disponible
}

// Función para colocar un barco en una cuadricula
void colocarBarco(Cuadricula* cuadricula, Barco* barco, int i) {
    int fila, columna, orientacion;
    srand(time(NULL)+i);
    
    // Generar coordenadas aleatorias y orientación
    do {
        fila = rand() % FILAS;
        columna = rand() % COLUMNAS;
        orientacion = rand() % 2;
    } while (!esEspacioDisponible(cuadricula, fila, columna, orientacion, barco->tamano));
    // Colocar el barco
    if (orientacion == 0) {
        for (int i = 0; i < barco->tamano; i++) {
            cuadricula->tablero[fila][columna + i] = barco->simbolo;
            cuadricula->idBarco[fila][columna+i]=barco->idBarco;
        }
    } else {
        for (int i = 0; i < barco->tamano; i++) {
            cuadricula->tablero[fila + i][columna] = barco->simbolo;
            cuadricula->idBarco[fila+i][columna]=barco->idBarco;
        }
    }
    
    // Inicializar la salud del barco
    barco->salud = barco->tamano;

    //return 1; // Barco colocado con éxito
}

int disparar(Cuadricula* cuadricula_disparo, Cuadricula* cuadricula_barco, Barco barcos[], char* coordenadas) {
    int fila, columna;

    if (strlen(coordenadas) == 2 || (strlen(coordenadas) == 3 && coordenadas[2] == '0')) {
        if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J') {
            if (coordenadas[1] >= '1' && ((coordenadas[2] == '\0') || (coordenadas[1] == '1' && coordenadas[2] == '0'))) {
                fila = (coordenadas[1] == '1' && coordenadas[2] == '0') ? 9 : coordenadas[1] - '1';
                columna = coordenadas[0] - 'A';
            }
        }
    }

    if (cuadricula_barco->tablero[fila][columna] == 'B') 
    {
        cuadricula_disparo->tablero[fila][columna] = 'X'; // Disparo acertado y actualiza la cuadricula de disparos
        cuadricula_barco->tablero[fila][columna] = 'O';    // Barco golpeado y actualiza la cuadricula de barcos

        // Reducir la salud del barco golpeado
        int posBarco=cuadricula_barco->idBarco[fila][columna];
        barcos[posBarco].salud--;

        //printf("id:%d  salud:%d\n",barcos[posBarco].idBarco,barcos[posBarco].salud);
        if (barcos[posBarco].salud == 0) 
        {
            //printf("¡Barco hundido! Coordenadas: %c%d\n", coordenadas[0], fila + 1);
            return 2; // Barco hundido
        }

        return 1; // Barco golpeado
    } 
    else 
    {
        cuadricula_disparo->tablero[fila][columna] = 'F'; // Disparo fallido y actualiza la cuadricula de disparos
        return 0; // Disparo fallido
    }
}

int validarDisparo(Cuadricula* cuadricula_disparo, char* coordenadas) 
{
    int fila, columna;

    if (strlen(coordenadas) == 2 || (strlen(coordenadas) == 3 && coordenadas[2] == '0')) 
    {
        if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J') 
        {
            if (coordenadas[1] >= '1' && ((coordenadas[2] == '\0') || (coordenadas[1] == '1' && coordenadas[2] == '0'))) 
            {
                fila = (coordenadas[1] == '1' && coordenadas[2] == '0') ? 9 : coordenadas[1] - '1';
                columna = coordenadas[0] - 'A';
            } 
            else 
            {
                //printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
                return -1; // Indicar que las coordenadas no son válidas
            }
        } 
        else 
        {
            //printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
            return -1; // Indicar que las coordenadas no son válidas
        }
    } 
    else 
    {
        //printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
        return -1; // Indicar que las coordenadas no son válidas
    }

    if (cuadricula_disparo->tablero[fila][columna] == 'X' ||
        cuadricula_disparo->tablero[fila][columna] == 'F' ||
        cuadricula_disparo->tablero[fila][columna] == 'O') 
    {
        //printf("Ya has disparado en esas coordenadas. Inténtalo de nuevo.\n");
        return 0; // Ya se disparó en esta ubicación
    }

    return 1;
}

// Función para mostrar las estadísticas de los jugadores
void mostrarEstadisticas(EstadisticasJugador jugador1, EstadisticasJugador jugador2) {
    printf("\nEstadísticas del Jugador 1:\n");
    printf("Disparos totales: %d\n", jugador1.disparos_totales);
    printf("Disparos acertados: %d\n", jugador1.aciertos);
    printf("Disparos fallidos: %d\n", jugador1.fallos);

    printf("\nEstadísticas del Jugador 2:\n");
    printf("Disparos totales: %d\n", jugador2.disparos_totales);
    printf("Disparos acertados: %d\n", jugador2.aciertos);
    printf("Disparos fallidos: %d\n", jugador2.fallos);
}

bool comprobarGanador(Cuadricula* cuadricula_barco,Barco barcos[]){
    
    //numero total de casillas ocupadas por barcos
    int total = 0;
    for(int i=0;i<5;i++)
    {
        total += barcos[i].tamano;
    }

    for(int i=0;i<FILAS;i++)
    {
        for(int j=0;j<COLUMNAS;j++)
        {
            if(cuadricula_barco->tablero[i][j] == 'O')
            {
                total--;
            }
        }
    }

    if(total == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

char* matrizBarcosToString(Cuadricula* cuadricula)
{
    char *cadena = (char *)calloc(250, sizeof(char));

    int cont=0;
    
    for(int i=0;i<FILAS;i++)
    {
        for(int j=0;j<COLUMNAS;j++)
        {
            if(cuadricula->tablero[i][j]=='~')
            {
                strcat(cadena,"A");
                cont++;
            }
            else if(cuadricula->tablero[i][j]=='B')
            {
                strcat(cadena,"B");
                cont++;
            }
            else if(cuadricula->tablero[i][j]=='O')
            {
                strcat(cadena,"B");
                cont++;
            }
            if(cont%10==0)
            {
                strcat(cadena,";");
            }
        }
    }
    return cadena;
}

char* matrizDisparosToString(Cuadricula* cuadricula)
{
    //char cadena[1000] = "";
    char *cadena = malloc(1000 * sizeof(char)); // Asigna memoria dinámica
    if (cadena == NULL) {
        // Manejo de error si malloc falla
        exit(1); // o devuelve NULL o haz lo que sea apropiado en tu aplicación
    }
    int cont=0;

    for(int i=0;i<FILAS;i++)
    {
        for(int j=0;j<COLUMNAS;j++)
        {
            if(cuadricula->tablero[i][j]=='~')
            {
                strcat(cadena,"A");
                cont++;
            }
            else if(cuadricula->tablero[i][j]=='X')
            {
                strcat(cadena,"X");
                cont++;
            }
            else if(cuadricula->tablero[i][j]=='F')
            {
                strcat(cadena,"F");
                cont++;
            }
            if(cont%10==0)
            {
                strcat(cadena,";");
            }
        }
    }
    return cadena;
}

void stringBarcosToMatriz(char *cadena,Cuadricula *cuadricula)
{
    int cont = 0;
    for(int i = 0; i < FILAS; i++)
    {
        for(int j = 0; j < COLUMNAS; j++)
        {
            if(cadena[cont] == 'A')
            {
                cuadricula->tablero[i][j] = '~';
                cont++;
            }
            else if(cadena[cont] == 'B')
            {
                cuadricula->tablero[i][j] = 'B';
                cont++;
            }
            else if(cadena[cont] == ';')
            {
                cont++;
            }
        }
        // Ignora el punto y coma que separa las filas
        cont++;
    }
}

void stringDisparosToMatriz(char *cadena,Cuadricula *cuadricula)
{
    int cont = 0;

    for(int i = 0; i < FILAS; i++)
    {
        for(int j = 0; j < COLUMNAS; j++)
        {
            if(cadena[cont] == 'A')
            {
                cuadricula->tablero[i][j] = '~';
            }
            else if(cadena[cont] == 'X')
            {
                cuadricula->tablero[i][j] = 'X';
            }
            else if(cadena[cont] == 'F')
            {
                cuadricula->tablero[i][j] = 'F';
            }
            else
            {
                exit(1);
            }
            cont++;
        }
        // Ignora el punto y coma que separa las filas
        cont++;
    }
}