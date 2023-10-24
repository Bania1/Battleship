#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILAS 10
#define COLUMNAS 10

// Estructura para representar una cuadrícula
typedef struct 
{
    char tablero[FILAS][COLUMNAS];
} Cuadricula;

// Estructura para representar un barco
typedef struct 
{
    int tamano;
    char simbolo;
} Barco;

// Función para inicializar una cuadricula con agua (~)
void inicializarCuadricula(Cuadricula* cuadricula) 
{
    for (int i = 0; i < FILAS; i++) 
    {
        for (int j = 0; j < COLUMNAS; j++) 
        {
            cuadricula->tablero[i][j] = '~';
        }
    }
}

// Función para imprimir una cuadricula con referencias de números y letras
void imprimirCuadricula(Cuadricula* cuadricula) 
{
    printf("   A B C D E F G H I J\n");
    for (int i = 0; i < FILAS; i++) 
    {
        printf("%2d ", i + 1);
        for (int j = 0; j < COLUMNAS; j++) 
        {
            printf("%c ", cuadricula->tablero[i][j]);
        }
        printf("\n");
    }
}

// Función para verificar si un espacio está disponible para colocar un barco
int esEspacioDisponible(Cuadricula* cuadricula, int fila, int columna, int orientacion, int tamano) 
{
    if (orientacion == 0) 
    {
        if (columna + tamano > COLUMNAS) 
        {
            return 0;  // Fuera de la cuadrícula
        }
        for (int i = fila - 1; i <= fila + 1; i++) 
        {
            for (int j = columna - 1; j <= columna + tamano; j++) 
            {
                if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS) 
                {
                    if (cuadricula->tablero[i][j] != '~') 
                    {
                        return 0;  // Colisión con otro barco
                    }
                }
            }
        }
    } 
    else 
    {
        if (fila + tamano > FILAS) 
        {
            return 0;  // Fuera de la cuadrícula
        }
        for (int i = fila - 1; i <= fila + tamano; i++) 
        {
            for (int j = columna - 1; j <= columna + 1; j++) 
            {
                if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS) 
                {
                    if (cuadricula->tablero[i][j] != '~') 
                    {
                        return 0;  // Colisión con otro barco
                    }
                }
            }
        }
    }
    return 1;  // Espacio disponible
}

// Función para colocar un barco en una cuadricula
int colocarBarco(Cuadricula* cuadricula, Barco* barco) 
{
    int fila, columna, orientacion;

    // Generar coordenadas aleatorias y orientación
    do 
    {
        fila = rand() % FILAS;
        columna = rand() % COLUMNAS;
        orientacion = rand() % 2;
    } while (!esEspacioDisponible(cuadricula, fila, columna, orientacion, barco->tamano));

    // Colocar el barco
    if (orientacion == 0) 
    {
        for (int i = 0; i < barco->tamano; i++) 
        {
            cuadricula->tablero[fila][columna + i] = barco->simbolo;
        }
    } 
    else 
    {
        for (int i = 0; i < barco->tamano; i++) 
        {
            cuadricula->tablero[fila + i][columna] = barco->simbolo;
        }
    }
    return 1;  // Barco colocado con éxito
}

int disparar(Cuadricula* cuadricula_disparo, Cuadricula* cuadricula_barco, char* coordenadas) 
{
    int fila = coordenadas[1] - '1';
    int columna = coordenadas[0] - 'A';

    if (cuadricula_disparo->tablero[fila][columna] == 'X' || cuadricula_disparo->tablero[fila][columna] == 'F' || cuadricula_disparo->tablero[fila][columna] == 'O') 
    {
        return -1;  // Ya se disparó en esta ubicación
    }

    if (cuadricula_barco->tablero[fila][columna] == 'B') 
    {
        cuadricula_disparo->tablero[fila][columna] = 'X';  // Disparo acertado
        cuadricula_barco->tablero[fila][columna] = 'O';     // Barco golpeado
        return 1;
    } 
    else 
    {
        cuadricula_disparo->tablero[fila][columna] = 'F';  // Disparo fallido
        return 0;
    }
}


int main() 
{
    // Inicializar la generación de números aleatorios
    srand(time(NULL));

    // Definir los barcos con la letra "B"
    Barco barcos[] = {{4, 'B'}, {3, 'B'}, {3, 'B'}, {2, 'B'}, {2, 'B'}};

    // Crear las cuadrículas para dos jugadores: barcos y disparos
    Cuadricula jugador1_barcos, jugador2_barcos, jugador1_disparos, jugador2_disparos;
    inicializarCuadricula(&jugador1_barcos);
    inicializarCuadricula(&jugador2_barcos);
    inicializarCuadricula(&jugador1_disparos);
    inicializarCuadricula(&jugador2_disparos);

    // Colocar los barcos en las cuadrículas
    for (int i = 0; i < 5; i++) 
    {
        colocarBarco(&jugador1_barcos, &barcos[i]);
        colocarBarco(&jugador2_barcos, &barcos[i]);
    }

    // Juego: alternar los turnos de los jugadores
    int turno = 1;
    int ganador = 0;
    while (!ganador) 
    {
        Cuadricula* cuadricula_barcos_propia;
        Cuadricula* cuadricula_disparos_propia;
        Cuadricula* cuadricula_barcos_contrincante;
        Cuadricula* cuadricula_disparos_contrincante;

        if (turno == 1) 
        {
            cuadricula_barcos_propia = &jugador1_barcos;
            cuadricula_disparos_propia = &jugador1_disparos;
            cuadricula_barcos_contrincante = &jugador2_barcos;
            cuadricula_disparos_contrincante = &jugador2_disparos;
        } 
        else 
        {
            cuadricula_barcos_propia = &jugador2_barcos;
            cuadricula_disparos_propia = &jugador2_disparos;
            cuadricula_barcos_contrincante = &jugador1_barcos;
            cuadricula_disparos_contrincante = &jugador1_disparos;
        }

        printf("\nTurno del Jugador %d\n", turno);
        printf("Tu cuadrícula de barcos:\n");
        imprimirCuadricula(cuadricula_barcos_propia);
        printf("Tu cuadrícula de disparos:\n");
        imprimirCuadricula(cuadricula_disparos_propia);

        // Lógica para que el jugador realice un disparo
        int disparo_exitoso = 0;
        while (!disparo_exitoso) 
        {
            char coordenadas[3];
            printf("Introduce coordenadas para disparar (por ejemplo, A1): ");
            scanf("%s", coordenadas);

            if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J' && coordenadas[1] >= '1' && coordenadas[1] <= '0' + FILAS) 
            {
                int resultado = disparar(cuadricula_disparos_propia, cuadricula_barcos_contrincante, coordenadas);
                if (resultado == 1) 
                {
                    printf("¡Disparo acertado!\n");
                } 
                else if (resultado == 0) 
                {
                    printf("Disparo fallido\n");
                } 
                else 
                {
                    printf("Ya has disparado en esas coordenadas, intenta de nuevo.\n");
                }
                disparo_exitoso = 1;
            } 
            else 
            {
                printf("Coordenadas no válidas. Inténtalo de nuevo.\n");
            }
        }

        // Verificar si el jugador ganó
        int hundidos = 1;
        for (int i = 0; i < 5; i++) 
        {
            Barco barco = barcos[i];
            for (int fila = 0; fila < FILAS; fila++) 
            {
                for (int columna = 0; columna < COLUMNAS; columna++) 
                {
                    if (cuadricula_barcos_contrincante->tablero[fila][columna] == barco.simbolo) 
                    {
                        hundidos = 0;
                        break;
                    }
                }
                if (!hundidos) 
                {
                    break;
                }
            }
            if (hundidos) 
            {
                break;
            }
        }

        if (hundidos) 
        {
            ganador = turno;
        } 
        else 
        {
            turno = (turno == 1) ? 2 : 1;
        }
    }

    printf("¡El Jugador %d ha ganado! ¡Felicidades!\n", ganador);

    return 0;
}