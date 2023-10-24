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

int main() 
{
    // Inicializar la generación de números aleatorios
    srand(time(NULL));

    // Definir los barcos con la letra "B"
    Barco barcos[] = {{4, 'B'}, {3, 'B'}, {3, 'B'}, {2, 'B'}, {2, 'B'}};

    // Crear las cuadrículas para dos jugadores
    Cuadricula jugador1, jugador2;
    inicializarCuadricula(&jugador1);
    inicializarCuadricula(&jugador2);

    // Colocar los barcos en las cuadrículas
    for (int i = 0; i < 5; i++) 
    {
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