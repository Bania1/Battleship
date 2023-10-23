#include "funciones.h"

//-----------------Funciones de usuario----------------
//
bool existeUsuario(char * usuario)
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

    while (fgets(linea,100,fich))
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

bool passwordCorrecta(char * usuario, char * password)
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

    while (fgets(linea,100,fich))
    {
        char * resto;
        
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

//Funcion que controla que no se repitan los usuarios
//True si se repite
bool comprobar_usuario(struct jugador * jugadores, int tam,int socket, char * nombre)
{
    int cont=0;
    for(int i=0; i<tam; i++)
    {
        if(jugadores[i].estado == LOGUEADO && (strcmp(jugadores[i].nombre_user,nombre) == 0))
        {
            //cont++;
            return true;
        }
        //esta parte hay q implementar que cuando un jugador haga ^c lo ponga a no_conectado para q cuando vuelva a entrar le deje, eso o borrarlo del struct de jugadores
        else if(jugadores[i].estado == NO_CONECTADO && (strcmp(jugadores[i].nombre_user,nombre) == 0))
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
void inicializar_vector_partidas(struct partida * partidas, int tam)
{
    for (size_t i = 0; i < tam; i++)
    {
        partidas[i].socket1 = NO_CONECTADO;
        partidas[i].socket2 = NO_CONECTADO;

        partidas[i].estado = VACIA;
    }
}

bool comprobar_partidas(struct partida * partidas, int tam)
{
    for(size_t i = 0; i < tam; i++)
    {
        if(partidas[i].estado == VACIA  || partidas[i].estado == SEMILLENA)
        {
            return true;
        }
    }
    return false;
}

//Esta funcion busca las partidas semillena o iniciada
//false --> jugador solo
//true --> jugador con otro jugador
bool buscarPartida(struct jugador * user, struct partida * partidas, int socket, int tam)
{
    for(size_t i = 0; i < tam; i++)
    {
        if(partidas[i].estado == SEMILLENA)
        {
            partidas[i].socket2 = socket;
            partidas[i].estado = INICIADA;
            return true;
        }
    }  
    
    for(size_t i = 0; i < tam; i++)
    {
        if(partidas[i].estado == VACIA)
        {
            partidas[i].socket1 = socket;
            partidas[i].estado = SEMILLENA;
            return false;
        }
    }      

    return false;
}

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

//Ver si es necesario
// void crearPartida(struct partida * partidas, struct jugador * jugadores, int socket, int tam)
// {   
//     for(size_t i = 0; i < tam; i++)
//     {
//         if(partidas[i].estado == NO_CONECTADO)
//         {
//             partidas[i].socket1 = socket;
//             partidas[i].estado = ESPERANDO;
//             jugadores[buscarSocket(jugadores, tam, socket)].estado = ESPERANDO;
//             return;
//         }
//     }
// }

//-----------------Funciones de servidor----------------
//
void inicializar_vector_jugadores(struct jugador * jugadores, int tam)
{
    for(int i=0; i<tam; i++)
    {
        jugadores[i].socket = -1;
        jugadores[i].estado = NO_CONECTADO;
    }
}

void guardarNuevoJugador(struct jugador * jugadores,int tam,int socket,int pos)
{
    /*for(int i=0; i<tam; i++)
    {
        if(jugadores[i].estado == NO_CONECTADO)
        {
            jugadores[i].socket = socket;
            jugadores[i].estado = CONECTADO;
        }
    }*/
    jugadores[pos-1].socket = socket;
    jugadores[pos-1].estado = CONECTADO;   
}

int buscarSocket(struct jugador * jugadores,int tam, int socket)
{
    for(int i=0; i<tam; i++)
    {
        if(jugadores[i].socket == socket)
        {
            return i;
        }
    }
    return -1;
}

int buscarSocketDisponible(struct jugador * jugadores, int tam,int s)
{
    for(int i=0; i<tam; i++)
    {
        if(jugadores[i].estado == ESPERANDO && jugadores[i].socket != s)
        {
            return jugadores[i].socket;
        }
    }
    return -1;
}