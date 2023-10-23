#include <stdio.h>
#include <stdlib.h>
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
} Barco;

// Función para inicializar una cuadricula con agua (~)
void inicializarCuadricula(Cuadricula* cuadricula) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            cuadricula->tablero[i][j] = '~';
        }
    }
}

// Función para imprimir una cuadricula con referencias de números y letras
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
    return 1;  // Barco colocado con éxito
}

// Función para verificar si un espacio está disponible para colocar un barco
int esEspacioDisponible(Cuadricula* cuadricula, int fila, int columna, int orientacion, int tamano) {
    if (orientacion == 0) {
        if (columna + tamano > COLUMNAS) {
            return 0;  // Fuera de la cuadrícula
        }
        for (int i = -1; i <= tamano; i++) {
            for (int j = -1; j <= 1; j++) {
                int x = fila + i;
                int y = columna + j;
                if (x >= 0 && x < FILAS && y >= 0 && y < COLUMNAS) {
                    if (cuadricula->tablero[x][y] != '~') {
                        return 0;  // Colisión con otro barco
                    }
                }
            }
        }
    } else {
        if (fila + tamano > FILAS) {
            return 0;  // Fuera de la cuadrícula
        }
        for (int i = -1; i <= tamano; i++) {
            for (int j = -1; j <= 1; j++) {
                int x = fila + j;
                int y = columna + i;
                if (x >= 0 && x < FILAS && y >= 0 && y < COLUMNAS) {
                    if (cuadricula->tablero[x][y] != '~') {
                        return 0;  // Colisión con otro barco
                    }
                }
            }
        }
    }
    return 1;  // Espacio disponible
}

int main() {
    // Inicializar la generación de números aleatorios
    srand(time(NULL));

    // Definir los barcos
    Barco barcos[] = {{4, '1'}, {3, '2'}, {3, '3'}, {2, '4'}, {2, '5'}};

    // Crear las cuadrículas para dos jugadores
    Cuadricula jugador1, jugador2;
    inicializarCuadricula(&jugador1);
    inicializarCuadricula(&jugador2);

    // Colocar los barcos en las cuadrículas
    for (int i = 0; i < 5; i++) {
        colocarBarco(&jugador1, &barcos[i]);
        colocarBarco(&jugador2, &barcos[i]);
    }

    // Imprimir las cuadrículas para ambos jugadores
    printf("Cuadrícula del Jugador 1:\n");
    imprimirCuadricula(&jugador1);
    printf("\nCuadrícula del Jugador 2:\n");
    imprimirCuadricula(&jugador2);

    return 0;
}


// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>

// #define BOARD_SIZE 10
// #define SHIPS 5
// #define EMPTY '~'
// #define SHIP 'B'
// #define SPACE ' '

// typedef struct {
//     char playerBoard[BOARD_SIZE][BOARD_SIZE]; // Tablero para colocar barcos
//     char shotBoard[BOARD_SIZE][BOARD_SIZE];   // Tablero para anotar disparos
// } Player;

// typedef struct {
//     int player1;
//     int player2;
// } Match;

// // Inicializa un tablero con agua ('~')
// void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             board[i][j] = EMPTY;
//         }
//     }
// }

// // Imprime un tablero
// void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
//     printf("  A B C D E F G H I J\n");
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         printf("%d ", i);
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             printf("%c ", board[i][j]);
//         }
//         printf("\n");
//     }
// }

// // Verifica si se puede colocar un barco en una posición y dirección dadas
// int canPlaceShip(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, int size, char direction) {
//     if (direction == 'H') {
//         if (col + size > BOARD_SIZE) {
//             return 0; // No cabe en el tablero
//         }
//         for (int i = col; i < col + size; i++) {
//             if (board[row][i] == SHIP) {
//                 return 0; // Colisión con otro barco
//             }
//         }
//     } else if (direction == 'V') {
//         if (row + size > BOARD_SIZE) {
//             return 0; // No cabe en el tablero
//         }
//         for (int i = row; i < row + size; i++) {
//             if (board[i][col] == SHIP) {
//                 return 0; // Colisión con otro barco
//             }
//         }
//     }
//     return 1; // Puede colocarse
// }

// // Coloca un barco en el tablero en una posición y dirección aleatorias
// void placeShip(char board[BOARD_SIZE][BOARD_SIZE], int size) {
//     int row, col;
//     char direction;
//     do {
//         row = rand() % BOARD_SIZE;
//         col = rand() % BOARD_SIZE;
//         direction = (rand() % 2 == 0) ? 'H' : 'V';
//     } while (!canPlaceShip(board, row, col, size, direction));

//     if (direction == 'H') {
//         for (int i = col; i < col + size; i++) {
//             board[row][i] = SHIP;
//         }
//     } else if (direction == 'V') {
//         for (int i = row; i < row + size; i++) {
//             board[i][col] = SHIP;
//         }
//     }
// }

// // Coloca todos los barcos aleatoriamente en el tablero del jugador
// void placeShipsRandomly(Player *player) {
//     initializeBoard(player->playerBoard);
//     for (int i = 0; i < SHIPS; i++) {
//         int shipSize = 2 + (i % 2); // Tamaño de los barcos: 2 o 3
//         placeShip(player->playerBoard, shipSize);
//     }

//     initializeBoard(player->shotBoard);
// }

// int main() {
//     srand(time(NULL));

//     Player players[2];
//     Match matches[1];

//     for (int i = 0; i < 2; i++) {
//         placeShipsRandomly(&players[i]);
//     }

//     printf("Player 1 Board:\n");
//     printBoard(players[0].playerBoard);

//     printf("\nPlayer 1 Shot Board:\n");
//     printBoard(players[0].shotBoard);

//     printf("\nPlayer 2 Board:\n");
//     printBoard(players[1].playerBoard);

//     printf("\nPlayer 2 Shot Board:\n");
//     printBoard(players[1].shotBoard);

//     return 0;
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>

// #define BOARD_SIZE 10
// #define SHIPS 5
// #define EMPTY '~'
// #define SHIP 'B'
// #define SPACE ' '

// typedef struct {
//     char board[BOARD_SIZE][BOARD_SIZE];
// } Board;

// typedef struct {
//     Board playerBoard;
//     Board shotBoard;
// } Player;

// void initializeBoard(Board *board) {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             board->board[i][j] = EMPTY;
//         }
//     }
// }

// void printBoard(Board *board) {
//     printf("  A B C D E F G H I J\n");
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         printf("%d ", i);
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             printf("%c ", board->board[i][j]);
//         }
//         printf("\n");
//     }
// }

// int canPlaceShip(Board *board, int row, int col, int size, char direction) {
//     if (direction == 'H') {
//         if (col + size > BOARD_SIZE) {
//             return 0;
//         }
//         for (int i = col; i < col + size; i++) {
//             if (board->board[row][i] == SHIP) {
//                 return 0;
//             }
//         }
//     } else if (direction == 'V') {
//         if (row + size > BOARD_SIZE) {
//             return 0;
//         }
//         for (int i = row; i < row + size; i++) {
//             if (board->board[i][col] == SHIP) {
//                 return 0;
//             }
//         }
//     }
//     return 1;
// }

// void placeShip(Board *board, int size) {
//     int row, col;
//     char direction;
//     do {
//         row = rand() % BOARD_SIZE;
//         col = rand() % BOARD_SIZE;
//         direction = (rand() % 2 == 0) ? 'H' : 'V';
//     } while (!canPlaceShip(board, row, col, size, direction));

//     if (direction == 'H') {
//         for (int i = col; i < col + size; i++) {
//             board->board[row][i] = SHIP;
//         }
//     } else if (direction == 'V') {
//         for (int i = row; i < row + size; i++) {
//             board->board[i][col] = SHIP;
//         }
//     }
// }

// void placeShipsRandomly(Player *player) {
//     initializeBoard(&player->playerBoard);
//     for (int i = 0; i < SHIPS; i++) {
//         int shipSize = 2 + (i % 2); // Alternates between 2 and 3
//         placeShip(&player->playerBoard, shipSize);
//     }
// }

// int main() {
//     srand(time(NULL));

//     Player players[2];

//     for (int i = 0; i < 2; i++) {
//         placeShipsRandomly(&players[i]);
//     }

//     // Print the player boards
//     printf("Player 1 Board:\n");
//     printBoard(&players[0].playerBoard);

//     printf("\nPlayer 2 Board:\n");
//     printBoard(&players[1].playerBoard);

//     return 0;
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <string.h>

// #define BOARD_SIZE 10
// #define NUM_SHIPS 5

// // Estructura para representar el tablero del jugador
// typedef struct {
//     char ships[BOARD_SIZE][BOARD_SIZE];   // Cuadrícula de barcos
//     char shots[BOARD_SIZE][BOARD_SIZE];   // Cuadrícula de disparos
// } Player;

// // Estructura para representar una partida
// typedef struct {
//     int player1Index;  // Índice del jugador 1 en el vector de jugadores
//     int player2Index;  // Índice del jugador 2 en el vector de jugadores
// } Match;

// // Estructura para representar un jugador
// typedef struct {
//     char name[20];  // Nombre del jugador
//     Player player;  // Tablero del jugador
// } GamePlayer;

// // Coloca los barcos en el tablero del jugador
// void placeShipsRandomly(Player* player) {
//     int shipSizes[] = {4, 3, 3, 2, 2};  // Tamaños de los barcos
//     srand(time(NULL));

//     for (int shipIndex = 0; shipIndex < NUM_SHIPS; shipIndex++) {
//         int shipSize = shipSizes[shipIndex];

//         // Intenta colocar el barco aleatoriamente hasta que tenga éxito
//         while (1) {
//             int isHorizontal = rand() % 2;  // 0 para vertical, 1 para horizontal
//             int row, col;

//             if (isHorizontal) {
//                 row = rand() % BOARD_SIZE;
//                 col = rand() % (BOARD_SIZE - shipSize + 1);
//             } else {
//                 row = rand() % (BOARD_SIZE - shipSize + 1);
//                 col = rand() % BOARD_SIZE;
//             }

//             // Comprueba si las posiciones están disponibles
//             int isValidPlacement = 1;
//             for (int i = 0; i < shipSize; i++) {
//                 if (isHorizontal && player->ships[row][col + i] != '~') {
//                     isValidPlacement = 0;
//                     break;
//                 } else if (!isHorizontal && player->ships[row + i][col] != '~') {
//                     isValidPlacement = 0;
//                     break;
//                 }
//             }

//             if (isValidPlacement) {
//                 // Coloca el barco en el tablero de barcos
//                 for (int i = 0; i < shipSize; i++) {
//                     if (isHorizontal) {
//                         player->ships[row][col + i] = 'B';
//                     } else {
//                         player->ships[row + i][col] = 'B';
//                     }
//                 }
//                 break;
//             }
//         }
//     }
// }

// // Inicializa el tablero de disparos del jugador
// void initShots(Player* player) {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             player->shots[i][j] = '~';
//         }
//     }
// }

// // Imprime el tablero del jugador
// void printBoard(const Player* player) {
//     printf("  0 1 2 3 4 5 6 7 8 9\n");
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         printf("%d ", i);
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             printf("%c ", player->ships[i][j]);
//         }
//         printf("\n");
//     }
// }

// // Imprime el tablero de disparos del jugador
// void printShots(const Player* player) {
//     printf("  0 1 2 3 4 5 6 7 8 9\n");
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         printf("%d ", i);
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             printf("%c ", player->shots[i][j]);
//         }
//         printf("\n");
//     }
// }

// int main() {
//     GamePlayer players[2];
//     Match match;

//     // Inicializa los tableros de ambos jugadores
//     for (int i = 0; i < 2; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             for (int k = 0; k < BOARD_SIZE; k++) {
//                 players[i].player.ships[j][k] = '~'; // '~' representa agua
//             }
//         }
//         initShots(&players[i].player);
//     }

//     // Coloca los barcos aleatori
//     for (int i = 0; i < 2; i++) {
//         placeShipsRandomly(&players[i].player);
//     }

//     // Muestra los tableros de ambos jugadores
//     printf("Tablero del Jugador 1:\n");
//     printBoard(&players[0].player);

//     printf("\nTablero de Disparos del Jugador 1:\n");
//     printShots(&players[0].player);

//     printf("Tablero del Jugador 2:\n");
//     printBoard(&players[1].player);

//     printf("\nTablero de Disparos del Jugador 2:\n");
//     printShots(&players[1].player);

//     return 0;
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <string.h>

// #define BOARD_SIZE 10
// #define NUM_SHIPS 5

// // Estructura para representar el tablero
// typedef struct {
//     char ships[BOARD_SIZE][BOARD_SIZE];  // Cuadrícula para barcos
//     char shots[BOARD_SIZE][BOARD_SIZE];  // Cuadrícula para disparos
// } Board;

// // Estructura para representar un jugador
// typedef struct {
//     char name[20];  // Nombre del jugador
//     Board board;    // Tablero del jugador
// } GamePlayer;

// // Coloca los barcos de forma aleatoria en el tablero del jugador
// void placeShipsRandomly(Board* board) {
//     int shipSizes[] = {4, 3, 3, 2, 2};  // Tamaños de los barcos
//     srand(time(NULL));

//     for (int shipIndex = 0; shipIndex < NUM_SHIPS; shipIndex++) {
//         int shipSize = shipSizes[shipIndex];

//         // Intenta colocar el barco aleatoriamente hasta que tenga éxito
//         while (1) {
//             int isHorizontal = rand() % 2;  // 0 para vertical, 1 para horizontal
//             int row, col;

//             if (isHorizontal) {
//                 row = rand() % BOARD_SIZE;
//                 col = rand() % (BOARD_SIZE - shipSize + 1);
//             } else {
//                 row = rand() % (BOARD_SIZE - shipSize + 1);
//                 col = rand() % BOARD_SIZE;
//             }

//             // Comprueba si las posiciones están disponibles
//             int isValidPlacement = 1;
//             for (int i = 0; i < shipSize; i++) {
//                 if (isHorizontal && board->ships[row][col + i] != '~') {
//                     isValidPlacement = 0;
//                     break;
//                 } else if (!isHorizontal && board->ships[row + i][col] != '~') {
//                     isValidPlacement = 0;
//                     break;
//                 }
//             }

//             if (isValidPlacement) {
//                 // Coloca el barco en el tablero
//                 for (int i = 0; i < shipSize; i++) {
//                     if (isHorizontal) {
//                         board->ships[row][col + i] = 'B';
//                     } else {
//                         board->ships[row + i][col] = 'B';
//                     }
//                 }
//                 break;
//             }
//         }
//     }
// }

// // Inicializa el tablero de disparos del jugador
// void initializeShots(Board* board) {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             board->shots[i][j] = ' ';
//         }
//     }
// }

// // Imprime el tablero de disparos del jugador
// void printShots(const Board* board) {
//     printf("Tablero de Disparos:\n");
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             printf("%c ", board->shots[i][j]);
//         }
//         printf("\n");
//     }
// }

// // Realiza un disparo en el tablero del oponente
// void performShot(Board* opponentBoard, int row, int col) {
//     if (opponentBoard->ships[row][col] == 'B') {
//         opponentBoard->shots[row][col] = 'X'; // Barco alcanzado
//     } else {
//         opponentBoard->shots[row][col] = 'O'; // Agua
//     }
// }

// int main() {
//     GamePlayer players[2];

//     for (int i = 0; i < 2; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             for (int k = 0; k < BOARD_SIZE; k++) {
//                 players[i].board.ships[j][k] = '~';  // '~' representa agua
//             }
//         }
//         placeShipsRandomly(&players[i].board);
//         initializeShots(&players[i].board);
//     }

//     printf("Tablero de Barcos del Jugador 1:\n");
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             printf("%c ", players[0].board.ships[i][j]);
//         }
//         printf("\n");
//     }

//     printf("\nTablero de Barcos del Jugador 2:\n");
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             printf("%c ", players[1].board.ships[i][j]);
//         }
//         printf("\n");
//     }

//     // Simulación de un disparo
//     printf("\nSimulación de un disparo del Jugador 1\n");

//     // Simulación de un disparo del Jugador 1 al Jugador 2
//     int row = 2;
//     int col = 3;
//     printf("\nJugador 1 dispara a la casilla (%d, %d).\n", row, col);
//     performShot(&players[1].board, row, col);

//     // Imprimir el Tablero de Disparos del Jugador 2
//     printShots(&players[1].board);

//     // Alternar turno al Jugador 2
//     printf("\nTurno del Jugador 2\n");

//     // Simulación de un disparo del Jugador 2 al Jugador 1
//     row = 4;
//     col = 5;
//     printf("\nJugador 2 dispara a la casilla (%d, %d).\n", row, col);
//     performShot(&players[0].board, row, col);

//     // Imprimir el Tablero de Disparos del Jugador 1
//     printShots(&players[0].board);

//     // Alternar turno al Jugador 1
//     printf("\nTurno del Jugador 1\n");

//     // Simulación de un disparo del Jugador 1 al Jugador 2
//     row = 3;
//     col = 7;
//     printf("\nJugador 1 dispara a la casilla (%d, %d).\n", row, col);
//     performShot(&players[1].board, row, col);

//     // Imprimir el Tablero de Disparos del Jugador 2
//     printShots(&players[1].board);

//     // Continuar alternando los turnos como sea necesario

//     return 0;
// }