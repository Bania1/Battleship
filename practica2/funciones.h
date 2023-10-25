#ifndef FUNCIONES_H
#define FUNCIONES_H

//-------------Librerias----------------
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

//-------------Constantes----------------
//

//Estados de los jugadores
#define NO_CONECTADO -1
#define CONECTADO 1
#define USUARIO_ENCONTRADO 2
#define LOGUEADO 3
#define ESPERANDO 4
#define JUGANDO 5

//Estados de las partidas
#define VACIA 6 //partida vacia
#define SEMILLENA 7 //1 jugador en partida
#define INICIADA 8 //2 jugadores en partida
#define TERMINADA 9 //partida terminada
#define COMPLETA 10 //partida completa

//Mensajes y tamaños
#define MSG_SIZE 250
#define MAX_CLIENTS 30
#define MAX_MATCHES 10

//-------------Estructuras----------------
//
struct jugador
{
    int socket;
    int estado;
    char nombre_user[50];
    //char nombre[50];
    //y más cosas
    //a lo mejor un vector de partidos
    //a lo mejor un contador para los disparos
};

struct partida
{
    int socket1; //socket del jugador 1
    int socket2; //socket del jugador 2
    int estado;
    int turno;

    //y más cosas?

    
};

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

// Estructura para representar las estadísticas de un jugador
typedef struct {
    int disparos_totales;
    int aciertos;
    int fallos;
} EstadisticasJugador;


//-------------Funciones de usuario----------------
//
//Funcion que comprueba si existe usuario y controla que no se repitan los usuarios
bool existeUsuario(char *usuario);

bool passwordCorrecta(char * usuario, char * password);

//Funcion que comprueba si se repite un usuario
bool comprobar_usuario(struct jugador * jugadores, int tam,int socket, char * nombre);

//bool registrarUsuario(char * usuario, char * password);

//-------------Funciones partidas------------------
//
void inicializar_vector_partidas(struct partida * partidas, int tam);

//Esta funcion comprueba la peticion de los jugadores para iniciar partida
//Devuelve true --> Partida disponible
//Devuelve false --> Partida no disponible
bool comprobar_partidas(struct partida * partidas, int tam);

void crearPartida(struct partida * partidas, struct jugador * jugadores, int socket, int tam);

int buscarPartida(struct jugador *user, struct partida *partidas, int numPartida, int socket, int tam);

void actualizarEstado_partida(struct partida * partidas, struct jugador * jugadores, int tam);

// void rellenar_datos_partidas(struct partida * partidas, struct jugador * jugadores, int pos1, int pos2, int num_partida);

//-------------Funciones de servidor----------------
//
void inicializar_vector_jugadores(struct jugador * jugadores, int tam);

void guardarNuevoJugador(struct jugador * jugadores,int tam, int socket,int pos);

int buscarSocket(struct jugador * jugadores,int tam, int socket);

int buscarSocketDisponible(struct jugador * jugadores, int tam, int pos);

void imprimirJugadores(struct jugador * jugadores, int tam);

void imprimirPartidas(struct partida * partidas, int tam);

void imprimirCuadricula(Cuadricula* cuadricula) ;

//Hacer una funcion de buscar usuario

//La de registrar usuarios para el final

#endif