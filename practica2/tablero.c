#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILAS 10
#define COLUMNAS 10

// Estructura para representar una cuadrícula
typedef struct {
    char tablero[FILAS][COLUMNAS];
} Cuadricula;

// Estructura para representar un barco
typedef struct {
    int tamano;
    char simbolo;
    int salud; // Nueva propiedad para rastrear la salud del barco
} Barco;

// Estructura para representar las estadísticas de un jugador
typedef struct {
    int disparos_totales;
    int aciertos;
    int fallos;
} EstadisticasJugador;

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

int disparar(Cuadricula* cuadricula_disparo, Cuadricula* cuadricula_barco, Barco barcos[], char* coordenadas, int turno) {
    int fila, columna;

    if (strlen(coordenadas) == 2 || (strlen(coordenadas) == 3 && coordenadas[1] == '0')) {
        if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J') {
            if (coordenadas[1] >= '1' && ((coordenadas[2] == '\0') || (coordenadas[1] == '1' && coordenadas[2] == '0'))) {
                fila = (coordenadas[1] == '1' && coordenadas[2] == '0') ? 9 : coordenadas[1] - '1';
                columna = coordenadas[0] - 'A';
            } else {
                printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
                return -1; // Indicar que las coordenadas no son válidas
            }
        } else {
            printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
            return -1; // Indicar que las coordenadas no son válidas
        }
    } else {
        printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
        return -1; // Indicar que las coordenadas no son válidas
    }

    if (cuadricula_disparo->tablero[fila][columna] == 'X' ||
        cuadricula_disparo->tablero[fila][columna] == 'F' ||
        cuadricula_disparo->tablero[fila][columna] == 'O') {
        printf("Ya has disparado en esas coordenadas. Inténtalo de nuevo.\n");
        return -1; // Ya se disparó en esta ubicación
    }

    if (cuadricula_barco->tablero[fila][columna] == 'B') {
        cuadricula_disparo->tablero[fila][columna] = 'X'; // Disparo acertado
        cuadricula_barco->tablero[fila][columna] = 'O';    // Barco golpeado

        // Reducir la salud del barco golpeado
        for (int i = 0; i < 5; i++) {
            if (barcos[i].simbolo == 'B') {
                barcos[i].salud--;
                if (barcos[i].salud == 0) {
                    printf("¡Barco hundido! Coordenadas: %c%d\n", coordenadas[0], fila + 1);
                }
            }
        }

        return 1;
    } else {
        cuadricula_disparo->tablero[fila][columna] = 'F'; // Disparo fallido
        return 0;
    }
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

int main() {
    // Inicializar la generación de números aleatorios
    srand(time(NULL));

    // Mostrar el menú de bienvenida
    int opcion;
    do {
        mostrarMenuBienvenida();
        scanf("%d", &opcion);
        getchar(); // Limpiar el búfer de entrada

        switch (opcion) {
            case 1:
                // Crear los barcos, cuadrículas y jugar
                Barco barcos[] = {{4, 'B', 0}, {3, 'B', 0}, {3, 'B', 0}, {2, 'B', 0}, {2, 'B', 0}};

                // Crear las cuadrículas para dos jugadores: barcos y disparos
                Cuadricula jugador1_barcos, jugador2_barcos, jugador1_disparos, jugador2_disparos;

                EstadisticasJugador stats1, stats2;

                inicializarCuadricula(&jugador1_barcos);
                inicializarCuadricula(&jugador2_barcos);
                inicializarCuadricula(&jugador1_disparos);
                inicializarCuadricula(&jugador2_disparos);

                // Colocar los barcos en las cuadrículas
                for (int i = 0; i < 5; i++) {
                    colocarBarco(&jugador1_barcos, &barcos[i]);
                    colocarBarco(&jugador2_barcos, &barcos[i]);
                }

                // Juego: alternar los turnos de los jugadores
                int turno = 1;
                int ganador = 0;
                while (!ganador) {
                    Cuadricula* cuadricula_barcos_propia;
                    Cuadricula* cuadricula_disparos_propia;
                    Cuadricula* cuadricula_barcos_contrincante;
                    Cuadricula* cuadricula_disparos_contrincante;

                    EstadisticasJugador* statspropias;

                    if (turno == 1) {
                        cuadricula_barcos_propia = &jugador1_barcos;
                        cuadricula_disparos_propia = &jugador1_disparos;
                        cuadricula_barcos_contrincante = &jugador2_barcos;
                        cuadricula_disparos_contrincante = &jugador2_disparos;
                        statspropias = &stats1;
                    } else {
                        cuadricula_barcos_propia = &jugador2_barcos;
                        cuadricula_disparos_propia = &jugador2_disparos;
                        cuadricula_barcos_contrincante = &jugador1_barcos;
                        cuadricula_disparos_contrincante = &jugador1_disparos;
                        statspropias = &stats2;
                    }

                    printf("\nTurno del Jugador %d\n", turno);
                    printf("Tu cuadrícula de barcos:\n");
                    imprimirCuadricula(cuadricula_barcos_propia);
                    printf("Tu cuadrícula de disparos:\n");
                    imprimirCuadricula(cuadricula_disparos_propia);

                    // Lógica para que el jugador realice un disparo
                    int disparo_exitoso = 0;
                    while (!disparo_exitoso) {
                        char coordenadas[4]; // Incluyendo espacio para el carácter nulo
                        printf("Introduce coordenadas para disparar (por ejemplo, A1): ");
                        scanf("%s", coordenadas);

                        int resultado = disparar(cuadricula_disparos_propia, cuadricula_barcos_contrincante, barcos, coordenadas, turno);
                        if (resultado == 1) {
                            printf("¡Disparo acertado! Coordenadas: %s\n", coordenadas);
                            statspropias->aciertos++;
                            statspropias->disparos_totales++;
                        } else if (resultado == 0) {
                            printf("Disparo fallido. Coordenadas: %s\n", coordenadas);
                            statspropias->fallos++;
                            statspropias->disparos_totales++;
                        } else if (resultado == -1) {
                            // Coordenadas no válidas, el jugador debe intentar de nuevo
                        }
                        if (resultado != -1) {
                            disparo_exitoso = 1;
                        }
                    }

                    // Verificar si el jugador ganó
                    int hundidos = 1;
                    for (int i = 0; i < 5; i++) {
                        Barco barco = barcos[i];
                        for (int fila = 0; fila < FILAS; fila++) {
                            for (int columna = 0; columna < COLUMNAS; columna++) {
                                if (cuadricula_barcos_contrincante->tablero[fila][columna] == barco.simbolo) {
                                    hundidos = 0;
                                    break;
                                }
                            }
                            if (!hundidos) {
                                break;
                            }
                        }
                        if (hundidos) {
                            printf("¡Barco hundido! Coordenadas: %c%d\n", barco.simbolo, i + 1);
                            break;
                        }
                    }

                    if (hundidos) {
                        ganador = turno;
                    } else {
                        turno = (turno == 1) ? 2 : 1;
                    }
                }

                printf("\n¡El Jugador %d ha ganado! ¡Felicidades!\n", ganador);
                mostrarEstadisticas(stats1, stats2);

                break;

            case 2:
                printf("\nGracias por jugar. Hasta luego.\n");
                return 0; // Salir del programa
                break;

            default:
                printf("\nOpción no válida. Intente de nuevo.\n");
        }
    } while (opcion != 2);

    return 0;
}


// // Función para mostrar el menú de bienvenida
// void mostrarMenuBienvenida()
// {
//     printf("\n=============================");
//     printf("\n       Bienvenido a");
//     printf("\n     Battleship Game");
//     printf("\n=============================\n");
//     printf("\n1. Jugar\n");
//     printf("2. Salir\n");
//     printf("\nElija una opción: ");
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <string.h>

// #define FILAS 10
// #define COLUMNAS 10

// // Estructura para representar una cuadrícula
// typedef struct 
// {
//     char tablero[FILAS][COLUMNAS];
// } Cuadricula;

// // Estructura para representar un barco
// typedef struct 
// {
//     int tamano;
//     int salud;
//     char simbolo;
// } Barco;

// // Estructura para representar las estadísticas de un jugador
// typedef struct {
//     int disparos_totales;
//     int aciertos;
//     int fallos;
// } EstadisticasJugador;

// // Función para mostrar el menú de bienvenida
// void mostrarMenuBienvenida()
// {
//     printf("\n=============================");
//     printf("\nBienvenido a Hundir la Flota\n");
//     printf("=============================\n");
//     printf("\n1. Jugar\n");
//     printf("2. Salir\n");
//     printf("\nElija una opción: ");
// }

// // Función para inicializar una cuadricula con agua (~)
// void inicializarCuadricula(Cuadricula* cuadricula) 
// {
//     for (int i = 0; i < FILAS; i++) 
//     {
//         for (int j = 0; j < COLUMNAS; j++) 
//         {
//             cuadricula->tablero[i][j] = '~';
//         }
//     }
// }

// // Función para imprimir una cuadricula con referencias de números y letras
// void imprimirCuadricula(Cuadricula* cuadricula) 
// {
//     printf("   A B C D E F G H I J\n");
//     for (int i = 0; i < FILAS; i++) 
//     {
//         printf("%2d ", i + 1);
//         for (int j = 0; j < COLUMNAS; j++) 
//         {
//             printf("%c ", cuadricula->tablero[i][j]);
//         }
//         printf("\n");
//     }
// }

// // Función para verificar si un espacio está disponible para colocar un barco
// int esEspacioDisponible(Cuadricula* cuadricula, int fila, int columna, int orientacion, int tamano) 
// {
//     if (orientacion == 0) 
//     {
//         if (columna + tamano > COLUMNAS) 
//         {
//             return 0;  // Fuera de la cuadrícula
//         }
//         for (int i = fila - 1; i <= fila + 1; i++) 
//         {
//             for (int j = columna - 1; j <= columna + tamano; j++) 
//             {
//                 if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS) 
//                 {
//                     if (cuadricula->tablero[i][j] != '~') 
//                     {
//                         return 0;  // Colisión con otro barco
//                     }
//                 }
//             }
//         }
//     } 
//     else 
//     {
//         if (fila + tamano > FILAS) 
//         {
//             return 0;  // Fuera de la cuadrícula
//         }
//         for (int i = fila - 1; i <= fila + tamano; i++) 
//         {
//             for (int j = columna - 1; j <= columna + 1; j++) 
//             {
//                 if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS) 
//                 {
//                     if (cuadricula->tablero[i][j] != '~') 
//                     {
//                         return 0;  // Colisión con otro barco
//                     }
//                 }
//             }
//         }
//     }
//     return 1;  // Espacio disponible
// }

// // Función para colocar un barco en una cuadricula
// int colocarBarco(Cuadricula* cuadricula, Barco* barco) 
// {
//     int fila, columna, orientacion;

//     // Generar coordenadas aleatorias y orientación
//     do 
//     {
//         fila = rand() % FILAS;
//         columna = rand() % COLUMNAS;
//         orientacion = rand() % 2;
//     } while (!esEspacioDisponible(cuadricula, fila, columna, orientacion, barco->tamano));

//     // Colocar el barco
//     if (orientacion == 0) 
//     {
//         for (int i = 0; i < barco->tamano; i++) 
//         {
//             cuadricula->tablero[fila][columna + i] = barco->simbolo;
//         }
//     } 
//     else 
//     {
//         for (int i = 0; i < barco->tamano; i++) 
//         {
//             cuadricula->tablero[fila + i][columna] = barco->simbolo;
//         }
//     }

//     // Configurar la salud inicial del barco
//     barco->salud = barco->tamano;

//     return 1;  // Barco colocado con éxito
// }

// int disparar(Cuadricula* cuadricula_disparo, Cuadricula* cuadricula_barco, Barco* barcos, char* coordenadas) 
// {
//     int fila, columna;

//     if (strlen(coordenadas) == 2) 
//     {
//         // Formato A1 - J9
//         if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J' &&
//             coordenadas[1] >= '1' && coordenadas[1] <= '9') 
//         {
//             fila = coordenadas[1] - '1';
//             columna = coordenadas[0] - 'A';
//         } 
//         else 
//         {
//             printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
//             return -1; // Indicar que las coordenadas no son válidas
//         }
//     } 
//     else if (strlen(coordenadas) == 3) 
//     {
//         // Formato A10 - J10
//         if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J' &&
//             coordenadas[1] == '1' && coordenadas[2] == '0') 
//         {
//             fila = 9; // Fila 10 se maneja como fila 9 en la cuadrícula
//             columna = coordenadas[0] - 'A';
//         } 
//         else 
//         {
//             printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
//             return -1; // Indicar que las coordenadas no son válidas
//         }
//     } 
//     else 
//     {
//         printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
//         return -1; // Indicar que las coordenadas no son válidas
//     }

//     if (cuadricula_disparo->tablero[fila][columna] == 'X' ||
//         cuadricula_disparo->tablero[fila][columna] == 'F' ||
//         cuadricula_disparo->tablero[fila][columna] == 'O') 
//     {
//         return -1; // Ya se disparó en esta ubicación
//     }

//     if (cuadricula_barco->tablero[fila][columna] == 'B') 
//     {
//         cuadricula_disparo->tablero[fila][columna] = 'X';  // Disparo acertado
//         cuadricula_barco->tablero[fila][columna] = 'O';     // Barco golpeado
        
//         // Encontrar el barco afectado por el disparo
//         for (int i = 0; i < 5; i++) 
//         {
//             if (barcos[i].simbolo == 'B' && barcos[i].salud > 0) 
//             {
//                 if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J' && coordenadas[1] >= '1' && coordenadas[1] <= '9') 
//                 {
//                     if (i != 3 && i != 4) 
//                     {
//                         if (i == 0 && (fila == (coordenadas[1] - '1')) && (columna <= ((coordenadas[0] - 'A') + barcos[i].tamano - 1) && columna >= (coordenadas[0] - 'A'))) 
//                         {
//                             barcos[i].salud--;
//                         } 
//                         else if (i == 1 && (fila <= ((coordenadas[1] - '1') + barcos[i].tamano - 1) && fila >= (coordenadas[1] - '1')) && (columna == (coordenadas[0] - 'A'))) 
//                         {
//                             barcos[i].salud--;
//                         } 
//                         else if (i == 2 && (fila <= ((coordenadas[1] - '1') + barcos[i].tamano - 1) && fila >= (coordenadas[1] - '1')) && (columna == (coordenadas[0] - 'A'))) 
//                         {
//                             barcos[i].salud--;
//                         }
//                     } 
//                     else if (i != 0 && i != 1 && i != 2) 
//                     {
//                         if (i == 3 && (fila == (coordenadas[1] - '1')) && (columna <= ((coordenadas[0] - 'A') + barcos[i].tamano - 1) && columna >= (coordenadas[0] - 'A'))) 
//                         {
//                             barcos[i].salud--;
//                         } 
//                         else if (i == 4 && (fila <= ((coordenadas[1] - '1') + barcos[i].tamano - 1) && fila >= (coordenadas[1] - '1')) && (columna == (coordenadas[0] - 'A'))) 
//                         {
//                             barcos[i].salud--;
//                         }
//                     }
//                 }
//             }
//         }

//         // Comprobar si el barco se ha hundido
//         for (int i = 0; i < 5; i++) 
//         {
//             if (barcos[i].simbolo == 'B' && barcos[i].salud == 0) 
//             {
//                 printf("¡Hundiste un barco de tamaño %d!\n", barcos[i].tamano);
//             }
//         }

//         return 1;
//     } 
//     else 
//     {
//         cuadricula_disparo->tablero[fila][columna] = 'F';  // Disparo fallido
//         return 0;
//     }
// }

// // Función para mostrar las estadísticas de los jugadores
// void mostrarEstadisticas(EstadisticasJugador jugador1, EstadisticasJugador jugador2) 
// {
//     printf("\nEstadísticas del Jugador 1:\n");
//     printf("Disparos totales: %d\n", jugador1.disparos_totales);
//     printf("Disparos acertados: %d\n", jugador1.aciertos);
//     printf("Disparos fallidos: %d\n", jugador1.fallos);
    
//     printf("\nEstadísticas del Jugador 2:\n");
//     printf("Disparos totales: %d\n", jugador2.disparos_totales);
//     printf("Disparos acertados: %d\n", jugador2.aciertos);
//     printf("Disparos fallidos: %d\n", jugador2.fallos);
// }

// int main() 
// {
//     // Inicializar la generación de números aleatorios
//     srand(time(NULL));

//     // Mostrar el menú de bienvenida
//     int opcion;
//     do
//     {
//         mostrarMenuBienvenida();
//         scanf("%d", &opcion);
//         getchar(); // Limpiar el búfer de entrada

//         switch (opcion)
//         {
//         case 1:
//             // Definir los barcos con la letra "B"
//             Barco barcos[] = {{4, 4, 'B'}, {3, 3, 'B'}, {3, 3, 'B'}, {2, 2, 'B'}, {2, 2, 'B'}};

//             // Crear las cuadrículas para dos jugadores: barcos y disparos
//             Cuadricula jugador1_barcos, jugador2_barcos, jugador1_disparos, jugador2_disparos;
            
//             EstadisticasJugador stats1, stats2;
            
//             inicializarCuadricula(&jugador1_barcos);
//             inicializarCuadricula(&jugador2_barcos);
//             inicializarCuadricula(&jugador1_disparos);
//             inicializarCuadricula(&jugador2_disparos);

//             // Colocar los barcos en las cuadrículas
//             for (int i = 0; i < 5; i++) 
//             {
//                 colocarBarco(&jugador1_barcos, &barcos[i]);
//                 colocarBarco(&jugador2_barcos, &barcos[i]);
//             }

//             // Juego: alternar los turnos de los jugadores
//             int turno = 1;
//             int ganador = 0;
//             while (!ganador) 
//             {
//                 Cuadricula* cuadricula_barcos_propia;
//                 Cuadricula* cuadricula_disparos_propia;
//                 Cuadricula* cuadricula_barcos_contrincante;
//                 Cuadricula* cuadricula_disparos_contrincante;

//                 // Estadísticas del jugador
//                 EstadisticasJugador* statspropias;

//                 if (turno == 1) 
//                 {
//                     cuadricula_barcos_propia = &jugador1_barcos;
//                     cuadricula_disparos_propia = &jugador1_disparos;
//                     cuadricula_barcos_contrincante = &jugador2_barcos;
//                     cuadricula_disparos_contrincante = &jugador2_disparos;
//                     statspropias = &stats1;
//                 } 
//                 else 
//                 {
//                     cuadricula_barcos_propia = &jugador2_barcos;
//                     cuadricula_disparos_propia = &jugador2_disparos;
//                     cuadricula_barcos_contrincante = &jugador1_barcos;
//                     cuadricula_disparos_contrincante = &jugador1_disparos;
//                     statspropias = &stats2;
//                 }

//                 printf("\nTurno del Jugador %d\n", turno);
//                 printf("Tu cuadrícula de barcos:\n");
//                 imprimirCuadricula(cuadricula_barcos_propia);
//                 printf("Tu cuadrícula de disparos:\n");
//                 imprimirCuadricula(cuadricula_disparos_propia);

//                 // Lógica para que el jugador realice un disparo
//                 int disparo_exitoso = 0;
//                 while (!disparo_exitoso) 
//                 {
//                     char coordenadas[4];
//                     printf("Introduce coordenadas para disparar (por ejemplo, A1): ");
//                     scanf("%s", coordenadas);

//                     int resultado = disparar(cuadricula_disparos_propia, cuadricula_barcos_contrincante, barcos, coordenadas);
//                     if (resultado == 1) 
//                     {
//                         printf("¡Disparo acertado!\n");
                        
//                         // Actualizar las estadísticas del jugador
//                         statspropias->aciertos++;
//                         statspropias->disparos_totales++;
//                     } 
//                     else if (resultado == 0) 
//                     {
//                         printf("Disparo fallido\n");
                        
//                         // Actualizar las estadísticas del jugador
//                         statspropias->fallos++;
//                         statspropias->disparos_totales++;
//                     } 
//                     else if (resultado == -1) 
//                     {
//                         printf("Ya has disparado en esas coordenadas, intenta de nuevo.\n");
//                     }
//                     disparo_exitoso = 1;
//                 }

//                 // Verificar si el jugador ganó
//                 int hundidos = 1;
//                 for (int i = 0; i < 5; i++) 
//                 {
//                     Barco barco = barcos[i];
//                     for (int fila = 0; fila < FILAS; fila++) 
//                     {
//                         for (int columna = 0; columna < COLUMNAS; columna++) 
//                         {
//                             if (cuadricula_barcos_contrincante->tablero[fila][columna] == barco.simbolo) 
//                             {
//                                 hundidos = 0;
//                                 break;
//                             }
//                         }
//                         if (!hundidos) 
//                         {
//                             break;
//                         }
//                     }
//                 }

//                 if (hundidos) 
//                 {
//                     printf("¡El Jugador %d ha ganado!\n", turno);
//                     ganador = turno;
//                 } 
//                 else 
//                 {
//                     turno = (turno == 1) ? 2 : 1;
//                 }
//             }

//             mostrarEstadisticas(stats1, stats2);
//             break;

//         case 2:
//             printf("Gracias por jugar. Hasta luego.\n");
//             break;

//         default:
//             printf("Opción no válida. Inténtalo de nuevo.\n");
//             break;
//         }
//     } while (opcion != 2);

//     return 0;
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <string.h>

// #define FILAS 10
// #define COLUMNAS 10

// // Estructura para representar una cuadrícula
// typedef struct 
// {
//     char tablero[FILAS][COLUMNAS];
// } Cuadricula;

// // Estructura para representar un barco
// typedef struct 
// {
//     int tamano;
//     char simbolo;
//     int salud; // Nuevo campo para rastrear la salud del barco
// } Barco;

// // Estructura para representar las estadísticas de un jugador
// typedef struct {
//     int disparos_totales;
//     int aciertos;
//     int fallos;
// } EstadisticasJugador;

// // Función para mostrar el menú de bienvenida
// void mostrarMenuBienvenida()
// {
//     printf("\n=============================");
//     printf("\nBienvenido a Hundir la Flota\n");
//     printf("================================================================\n");
//     printf ("XXXXX   XXXX  XXXXXX XXXXXX XX     XXXXXX  XXXXX XX  XX XX XXXX\n");
//     printf ("XX  XX XX  XX   XX     XX   XX     XX     XX     XX  XX XX XX  XX\n");
//     printf ("XXXXX  XX  XX   XX     XX   XX     XXXX    XXXX  XXXXXX XX XXXX\n"); 
//     printf ("XX  XX XXXXXX   XX     XX   XX     XX         XX XX  XX XX XX\n");
//     printf ("XXXXX  XX  XX   XX     XX   XXXXXX XXXXXX XXXXX  XX  XX XX XX\n");
//     printf("================================================================\n");
//     printf("\n1. Jugar\n");
//     printf("2. Salir\n");
//     printf("\nElija una opción: ");
// }

// // Función para inicializar una cuadricula con agua (~)
// void inicializarCuadricula(Cuadricula* cuadricula) 
// {
//     for (int i = 0; i < FILAS; i++) 
//     {
//         for (int j = 0; j < COLUMNAS; j++) 
//         {
//             cuadricula->tablero[i][j] = '~';
//         }
//     }
// }

// // Función para imprimir una cuadricula con referencias de números y letras
// void imprimirCuadricula(Cuadricula* cuadricula) 
// {
//     printf("   A B C D E F G H I J\n");
//     for (int i = 0; i < FILAS; i++) 
//     {
//         printf("%2d ", i + 1);
//         for (int j = 0; j < COLUMNAS; j++) 
//         {
//             printf("%c ", cuadricula->tablero[i][j]);
//         }
//         printf("\n");
//     }
// }

// // Función para verificar si un espacio está disponible para colocar un barco
// int esEspacioDisponible(Cuadricula* cuadricula, Cuadricula* cuadricula_disparo, Cuadricula* cuadricula_barco, int fila, int columna, int orientacion, int tamano) 
// {
//     if (orientacion == 0) 
//     {
//         if (columna + tamano > COLUMNAS) 
//         {
//             return 0;  // Fuera de la cuadrícula
//         }
//         for (int i = fila - 1; i <= fila + 1; i++) 
//         {
//             for (int j = columna - 1; j <= columna + tamano; j++) 
//             {
//                 if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS) 
//                 {
//                     if (cuadricula->tablero[i][j] != '~') 
//                     {
//                         return 0;  // Colisión con otro barco
//                     }
//                 }
//             }
//         }
//     } 
//     else 
//     {
//         if (fila + tamano > FILAS) 
//         {
//             return 0;  // Fuera de la cuadrícula
//         }
//         for (int i = fila - 1; i <= fila + tamano; i++) 
//         {
//             for (int j = columna - 1; j <= columna + 1; j++) 
//             {
//                 if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS) 
//                 {
//                     if (cuadricula->tablero[i][j] != '~') 
//                     {
//                         return 0;  // Colisión con otro barco
//                     }
//                 }
//             }
//         }
//     }
//     if (cuadricula_barco->tablero[fila][columna] == 'B') 
//     {
//         cuadricula_disparo->tablero[fila][columna] = 'X';  // Disparo acertado
//         cuadricula_barco->tablero[fila][columna] = 'O';     // Barco golpeado

//         // Encuentra el barco y disminuye su salud
//         for (int i = 0; i < 5; i++) 
//         {
//             if (barcos[i].simbolo == 'B') 
//             {
//                 barcos[i].salud--;

//                 // Verifica si el barco se ha hundido
//                 if (barcos[i].salud == 0) 
//                 {
//                     printf("¡Barco hundido!\n");
//                 }
//             }
//         }

//         return 1;
//     } 
//     else 
//     {
//         cuadricula_disparo->tablero[fila][columna] = 'F';  // Disparo fallido
//         return 0;
//     }
//     return 1;  // Espacio disponible
// }

// // Función para colocar un barco en una cuadricula
// int colocarBarco(Cuadricula* cuadricula, Barco* barco) 
// {
//     int fila, columna, orientacion;

//     // Generar coordenadas aleatorias y orientación
//     do 
//     {
//         fila = rand() % FILAS;
//         columna = rand() % COLUMNAS;
//         orientacion = rand() % 2;
//     } while (!esEspacioDisponible(cuadricula, fila, columna, orientacion, barco->tamano));

//     // Colocar el barco
//     if (orientacion == 0) 
//     {
//         for (int i = 0; i < barco->tamano; i++) 
//         {
//             cuadricula->tablero[fila][columna + i] = barco->simbolo;
//         }
//     } 
//     else 
//     {
//         for (int i = 0; i < barco->tamano; i++) 
//         {
//             cuadricula->tablero[fila + i][columna] = barco->simbolo;
//         }
//     }
//     return 1;  // Barco colocado con éxito
// }

// int disparar(Cuadricula* cuadricula_disparo, Cuadricula* cuadricula_barco, char* coordenadas) 
// {
//     int fila, columna;

//     if (strlen(coordenadas) == 2) 
//     {
//         // Formato A1 - J9
//         if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J' &&
//             coordenadas[1] >= '1' && coordenadas[1] <= '9') 
//         {
//             fila = coordenadas[1] - '1';
//             columna = coordenadas[0] - 'A';
//         } 
//         else 
//         {
//             printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
//             return -1; // Indicar que las coordenadas no son válidas
//         }
//     } 
//     else if (strlen(coordenadas) == 3) 
//     {
//         // Formato A10 - J10
//         if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J' &&
//             coordenadas[1] == '1' && coordenadas[2] == '0') 
//         {
//             fila = 9; // Fila 10 se maneja como fila 9 en la cuadrícula
//             columna = coordenadas[0] - 'A';
//         } 
//         else 
//         {
//             printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
//             return -1; // Indicar que las coordenadas no son válidas
//         }
//     } 
//     else 
//     {
//         printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
//         return -1; // Indicar que las coordenadas no son válidas
//     }

//     if (cuadricula_disparo->tablero[fila][columna] == 'X' ||
//         cuadricula_disparo->tablero[fila][columna] == 'F' ||
//         cuadricula_disparo->tablero[fila][columna] == 'O') 
//     {
//         return -1; // Ya se disparó en esta ubicación
//     }

//     if (cuadricula_barco->tablero[fila][columna] == 'B') 
//     {
//         cuadricula_disparo->tablero[fila][columna] = 'X';  // Disparo acertado
//         cuadricula_barco->tablero[fila][columna] = 'O';     // Barco golpeado
//         return 1;
//     } 
//     else 
//     {
//         cuadricula_disparo->tablero[fila][columna] = 'F';  // Disparo fallido
//         return 0;
//     }
// }


// // int disparar(Cuadricula* cuadricula_disparo, Cuadricula* cuadricula_barco, char* coordenadas) 
// // {
  
// //     int fila = coordenadas[1] - '1';
// //     int columna = coordenadas[0] - 'A';

// //     if (cuadricula_disparo->tablero[fila][columna] == 'X' || cuadricula_disparo->tablero[fila][columna] == 'F' || cuadricula_disparo->tablero[fila][columna] == 'O') 
// //     {
// //         return -1;  // Ya se disparó en esta ubicación
// //     }

// //     if (cuadricula_barco->tablero[fila][columna] == 'B') 
// //     {
// //         cuadricula_disparo->tablero[fila][columna] = 'X';  // Disparo acertado
// //         cuadricula_barco->tablero[fila][columna] = 'O';     // Barco golpeado
// //         return 1;
// //     } 
// //     else 
// //     {
// //         cuadricula_disparo->tablero[fila][columna] = 'F';  // Disparo fallido
// //         return 0;
// //     }
// // }

// // Función para mostrar las estadísticas de los jugadores
// void mostrarEstadisticas(EstadisticasJugador jugador1, EstadisticasJugador jugador2) 
// {
//     printf("\n-----------> Estadísticas del Jugador 1: <-----------\n");
//     printf("Disparos totales: %d\n", jugador1.disparos_totales);
//     printf("Disparos acertados: %d\n", jugador1.aciertos);
//     printf("Disparos fallidos: %d\n", jugador1.fallos);
    
//     printf("\n-----------> Estadísticas del Jugador 2: <-----------\n");
//     printf("Disparos totales: %d\n", jugador2.disparos_totales);
//     printf("Disparos acertados: %d\n", jugador2.aciertos);
//     printf("Disparos fallidos: %d\n", jugador2.fallos);
// }

// int main() 
// {
//     // Inicializar la generación de números aleatorios
//     srand(time(NULL));

//     // Mostrar el menú de bienvenida
//     int opcion;
//     do
//     {
//         mostrarMenuBienvenida();
//         scanf("%d", &opcion);
//         getchar(); // Limpiar el búfer de entrada

//         switch (opcion)
//         {
//         case 1:
//             // Crear los barcos, cuadrículas y jugar (código existente)
//             // Definir los barcos con la letra "B"
//             Barco barcos[] = {{4, 'B'}, {3, 'B'}, {3, 'B'}, {2, 'B'}, {2, 'B'}};

//             // Crear las cuadrículas para dos jugadores: barcos y disparos
//             Cuadricula jugador1_barcos, jugador2_barcos, jugador1_disparos, jugador2_disparos;
            
//             // Crear las estadísticas para dos jugadores
//             EstadisticasJugador stats1= {0, 0, 0};
//             EstadisticasJugador stats2= {0, 0, 0};
            
//             inicializarCuadricula(&jugador1_barcos);
//             inicializarCuadricula(&jugador2_barcos);
//             inicializarCuadricula(&jugador1_disparos);
//             inicializarCuadricula(&jugador2_disparos);

//             // Colocar los barcos en las cuadrículas
//             for (int i = 0; i < 5; i++) 
//             {
//                 colocarBarco(&jugador1_barcos, &barcos[i]);
//                 colocarBarco(&jugador2_barcos, &barcos[i]);
//             }

//             // Juego: alternar los turnos de los jugadores
//             int turno = 1;
//             int ganador = 0;
//             while (!ganador) 
//             {
//                 Cuadricula* cuadricula_barcos_propia;
//                 Cuadricula* cuadricula_disparos_propia;
//                 Cuadricula* cuadricula_barcos_contrincante;
//                 Cuadricula* cuadricula_disparos_contrincante;

//                 // Estadísticas del jugador
//                 EstadisticasJugador* statspropias;

//                 if (turno == 1) 
//                 {
//                     cuadricula_barcos_propia = &jugador1_barcos;
//                     cuadricula_disparos_propia = &jugador1_disparos;
//                     cuadricula_barcos_contrincante = &jugador2_barcos;
//                     cuadricula_disparos_contrincante = &jugador2_disparos;
//                     statspropias = &stats1;
//                 } 
//                 else 
//                 {
//                     cuadricula_barcos_propia = &jugador2_barcos;
//                     cuadricula_disparos_propia = &jugador2_disparos;
//                     cuadricula_barcos_contrincante = &jugador1_barcos;
//                     cuadricula_disparos_contrincante = &jugador1_disparos;
//                     statspropias = &stats2;
//                 }

//                 printf("\n-----------> Turno del Jugador %d <-----------\n\n", turno);
//                 printf("Tu cuadrícula de barcos:\n\n");
//                 imprimirCuadricula(cuadricula_barcos_propia);
//                 printf("\nTu cuadrícula de disparos:\n\n");
//                 imprimirCuadricula(cuadricula_disparos_propia);

//                 // Lógica para que el jugador realice un disparo
//                 int disparo_exitoso = 0;
//                 while (!disparo_exitoso) 
//                 {
//                     char coordenadas[3];
//                     printf("\nIntroduce coordenadas para disparar (por ejemplo, A1): ");
//                     scanf("%s", coordenadas);

//                     if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J' && coordenadas[1] >= '1' && coordenadas[1] <= '0' + FILAS) 
//                     {
//                         int resultado = disparar(cuadricula_disparos_propia, cuadricula_barcos_contrincante, coordenadas);
//                         if (resultado == 1) 
//                         {
//                             printf("-----------> ¡Disparo acertado!\n");
                            
//                             // Actualizar las estadísticas del jugador
//                             statspropias->aciertos++;
//                             statspropias->disparos_totales++;
//                         } 
//                         else if (resultado == 0) 
//                         {
//                             printf("-----------> Disparo fallido\n");
                            
//                             // Actualizar las estadísticas del jugador
//                             statspropias->fallos++;
//                             statspropias->disparos_totales++;
//                         } 
//                         else 
//                         {
//                             printf("Ya has disparado en esas coordenadas, intenta de nuevo.\n");
//                         }
//                         disparo_exitoso = 1;
//                     } 
//                     else 
//                     {
//                         printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
//                     }
//                 }

//                 // Verificar si el jugador ganó
//                 int hundidos = 1;
//                 for (int i = 0; i < 5; i++) 
//                 {
//                     Barco barco = barcos[i];
//                     for (int fila = 0; fila < FILAS; fila++) 
//                     {
//                         for (int columna = 0; columna < COLUMNAS; columna++) 
//                         {
//                             if (cuadricula_barcos_contrincante->tablero[fila][columna] == barco.simbolo) 
//                             {
//                                 hundidos = 0;
//                                 break;
//                             }
//                         }
//                         if (!hundidos) 
//                         {
//                             break;
//                         }
//                     }
//                     if (hundidos) 
//                     {
//                         break;
//                     }
//                 }

//                 if (hundidos) 
//                 {
//                     ganador = turno;
//                 } 
//                 else 
//                 {
//                     turno = (turno == 1) ? 2 : 1;
//                 }
//             }

//             printf("\n¡El Jugador %d ha ganado! ¡Felicidades!\n", ganador);
            
//             // Mostrar las estadísticas de los jugadores
//             printf("\n-----------------------------------\n");
//             mostrarEstadisticas(stats1, stats2);

//             break;

//         case 2:
//             printf("\nGracias por jugar. Hasta luego.\n");
//             return 0; // Salir del programa
//             break;

//         default:
//             printf("\nOpción no válida. Intente de nuevo.\n");
//         }
//     } while (opcion != 2);

    
// }