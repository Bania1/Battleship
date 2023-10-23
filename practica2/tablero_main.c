#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include 

int main() {
    struct jugador player1;
    strcpy(player1.name, "Jugador 1");

    initializeBoard(&player1);
    placeShipsRandomly(&player1);

    // Imprime el tablero después de colocar los barcos
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", player1.board[i][j]);
        }
        printf("\n");
    }

    return 0;
}