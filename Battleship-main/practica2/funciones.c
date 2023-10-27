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
        passwordFichero = strtok(resto, ",");

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
        if (jugadores[i].estado == LOGUEADO && (strcmp(jugadores[i].nombre_user, nombre) == 0))
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

// Me falta la funcion de registar usuario

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
        /*printf("Password: %s\n", jugadores[i].password);
        printf("Puntuacion: %d\n", jugadores[i].puntuacion);
        printf("Partidas ganadas: %d\n", jugadores[i].partidas_ganadas);
        printf("Partidas perdidas: %d\n", jugadores[i].partidas_perdidas);
        printf("Partidas empatadas: %d\n", jugadores[i].partidas_empatadas);
        printf("Partidas abandonadas: %d\n", jugadores[i].partidas_abandonadas);
        printf("Partidas jugadas: %d\n", jugadores[i].partidas_jugadas);
        printf("Partidas en curso: %d\n", jugadores[i].partidas_en_curso);
        printf("Partidas en espera: %d\n", jugadores[i].partidas_en_espera);
        printf("Partidas en finalizadas: %d\n", jugadores[i].partidas_finalizadas);
        printf("Partidas en no conectado: %d\n", jugadores[i].partidas_no_conectado);
        printf("Partidas en logueado: %d\n", jugadores[i].partidas_logueado);
        printf("Partidas en conectado: %d\n", jugadores[i].partidas_conectado);
        printf("Partidas en iniciada: %d\n", jugadores[i].partidas_iniciada);
        printf("Partidas en semillena: %d\n", jugadores[i].partidas_semillena);
        printf("Partidas en vacia: %d\n", jugadores[i].partidas_vacia);
        printf("Partidas en esperando: %d\n", jugadores[i].partidas_esperando);
        printf("Partidas en abandonado: %d\n", jugadores[i].partidas_abandonado);
        printf("Partidas en finalizada: %d\n", jugadores[i].partidas_finalizada);
        printf("Partidas en no conectado: %d\n", jug*/
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

// Función para inicializar una cuadrícula con agua (~)
void inicializarCuadricula(Cuadricula* cuadricula) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            cuadricula->tablero[i][j] = '~';
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
int colocarBarco(Cuadricula* cuadricula, Barco* barco) {
    int fila, columna, orientacion;

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
        }
    } else {
        for (int i = 0; i < barco->tamano; i++) {
            cuadricula->tablero[fila + i][columna] = barco->simbolo;
        }
    }

    // Inicializar la salud del barco
    barco->salud = barco->tamano;

    return 1; // Barco colocado con éxito
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
