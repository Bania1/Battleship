#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "funciones.h"

int main()
{

	/*----------------------------------------------------
		Descriptor del socket y buffer de datos
	-----------------------------------------------------*/
	int sd;
	struct sockaddr_in sockname;
	char buffer[MSG_SIZE];
	socklen_t len_sockname;
	fd_set readfds, auxfds;
	int salida;
	int fin = 0;

	/* --------------------------------------------------
		Se abre el socket
	---------------------------------------------------*/
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
		exit(1);
	}

	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2065);
	sockname.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);

	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror("Error de conexión");
		exit(1);
	}

	// Inicializamos las estructuras
	FD_ZERO(&auxfds);
	FD_ZERO(&readfds);

	FD_SET(0, &readfds);
	FD_SET(sd, &readfds);

	Cuadricula *tableroBarcos=(Cuadricula *)malloc(sizeof(Cuadricula));
	inicializarCuadricula(tableroBarcos);

	Cuadricula *tableroDisparos=(Cuadricula *)malloc(sizeof(Cuadricula));
	inicializarCuadricula(tableroDisparos);
	
	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do
	{
		auxfds = readfds;
		salida = select(sd + 1, &auxfds, NULL, NULL, NULL);

		// Tengo mensaje desde el servidor
		if (FD_ISSET(sd, &auxfds))
		{

			bzero(buffer, sizeof(buffer));
			recv(sd, buffer, sizeof(buffer), 0);
			
			char buffer_copia[sizeof(buffer)];
			//char buffer_copia2[sizeof(buffer)];
    		strcpy(buffer_copia, buffer); 
			//strcpy(buffer_copia2, buffer); 

			char * token = strtok(buffer_copia," ");
			//char * token2 = strtok(buffer_copia2,";");
			//printf("\n%s\n", buffer);

			if (strcmp(buffer, "Demasiados clientes conectados\n") == 0)
			{
				printf("\n%s\n", buffer);
				fin = 1;
			}
			else if (strcmp(buffer, "Desconexión servidor\n") == 0)
			{
				printf("\n%s\n", buffer);
				fin = 1;
			}
			else if (strcmp(buffer, "+Ok. Empezamos partida\n") == 0)
			{
				
				//bzero(buffer, sizeof(buffer));
				printf("\n%s\n", buffer);
				bzero(buffer, sizeof(buffer));
				recv(sd, buffer, sizeof(buffer), 0);
				//printf("\n%s\n", buffer);

				printf("Tu tablero de barcos:\n");
				stringBarcosToMatriz(buffer,tableroBarcos);
				imprimirCuadricula(tableroBarcos);

				printf("\nTu tablero de disparos:\n");
				imprimirCuadricula(tableroDisparos);
				
				//printf("\n%s\n", buffer);
				//bzero(buffer, sizeof(buffer));
				//recv(sd, buffer, sizeof(buffer), 0);
				//stringBarcosToMatriz(buffer,&partidas.tableroBarcos1);
				//imprimirCuadricula(&partidas.tableroBarcos1);
			}
			else if (strncmp(buffer, "+Ok. Agua:", 10) == 0)
			{
				printf("\n%s\n", buffer);

				char col;
				int fil;

				sscanf(buffer, "+Ok. Agua: %c,%d", &col, &fil);
				//Concateno la columna y la fila
				
				int fila, columna;
				char coordenadas[4];
				sprintf(coordenadas, "%c%d", col, fil);

				if (strlen(coordenadas) == 2 || (strlen(coordenadas) == 3 && coordenadas[2] == '0')) 
				{
					if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J') 
					{
						if (coordenadas[1] >= '1' && ((coordenadas[2] == '\0') || (coordenadas[1] == '1' && coordenadas[2] == '0'))) 
						{
							fila = (coordenadas[1] == '1' && coordenadas[2] == '0') ? 9 : coordenadas[1] - '1';
							columna = coordenadas[0] - 'A';
						} 
					}
				}				
				//bzero(buffer, sizeof(buffer));
				//recv(sd, buffer, sizeof(buffer), 0);

				printf("Tu tablero de barcos:\n");
				//stringBarcosToMatriz(buffer,tableroBarcos);
				imprimirCuadricula(tableroBarcos);

				printf("\nTu tablero de disparos:\n");
				tableroDisparos->tablero[fila][columna]='F';
				imprimirCuadricula(tableroDisparos);

				bzero(buffer, sizeof(buffer));
				recv(sd, buffer, sizeof(buffer), 0);
				printf("\n%s\n", buffer);

			}
			else if (strncmp(buffer, "+Ok. Tocado:",12) == 0)
			{
				printf("\n%s\n", buffer);

				char col;
				int fil;

				sscanf(buffer, "+Ok. Tocado: %c,%d", &col, &fil);
				//Concateno la columna y la fila
				
				int fila, columna;
				char coordenadas[4];
				sprintf(coordenadas, "%c%d", col, fil);

				if (strlen(coordenadas) == 2 || (strlen(coordenadas) == 3 && coordenadas[2] == '0')) 
				{
					if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J') 
					{
						if (coordenadas[1] >= '1' && ((coordenadas[2] == '\0') || (coordenadas[1] == '1' && coordenadas[2] == '0'))) 
						{
							fila = (coordenadas[1] == '1' && coordenadas[2] == '0') ? 9 : coordenadas[1] - '1';
							columna = coordenadas[0] - 'A';
						} 
					}
				}				
				//bzero(buffer, sizeof(buffer));
				//recv(sd, buffer, sizeof(buffer), 0);

				printf("Tu tablero de barcos:\n");
				//stringBarcosToMatriz(buffer,tableroBarcos);
				imprimirCuadricula(tableroBarcos);

				printf("\nTu tablero de disparos:\n");
				tableroDisparos->tablero[fila][columna]='X';
				imprimirCuadricula(tableroDisparos);

				bzero(buffer, sizeof(buffer));
				recv(sd, buffer, sizeof(buffer), 0);
				printf("\n%s\n", buffer);

			}
			else if (strncmp(buffer, "+Ok. Hundido:\n",13) == 0)
			{
				printf("\n%s\n", buffer);

				char col;
				int fil;

				sscanf(buffer, "+Ok. Tocado: %c,%d", &col, &fil);
				//Concateno la columna y la fila
				
				int fila, columna;
				char coordenadas[4];
				sprintf(coordenadas, "%c%d", col, fil);

				if (strlen(coordenadas) == 2 || (strlen(coordenadas) == 3 && coordenadas[2] == '0')) 
				{
					if (coordenadas[0] >= 'A' && coordenadas[0] <= 'J') 
					{
						if (coordenadas[1] >= '1' && ((coordenadas[2] == '\0') || (coordenadas[1] == '1' && coordenadas[2] == '0'))) 
						{
							fila = (coordenadas[1] == '1' && coordenadas[2] == '0') ? 9 : coordenadas[1] - '1';
							columna = coordenadas[0] - 'A';
						} 
					}
				}				
				//bzero(buffer, sizeof(buffer));
				//recv(sd, buffer, sizeof(buffer), 0);

				printf("Tu tablero de barcos:\n");
				//stringBarcosToMatriz(buffer,tableroBarcos);
				imprimirCuadricula(tableroBarcos);

				printf("\nTu tablero de disparos:\n");
				tableroDisparos->tablero[fila][columna]='X';
				imprimirCuadricula(tableroDisparos);


				bzero(buffer, sizeof(buffer));
				recv(sd, buffer, sizeof(buffer), 0);
				if(strcmp(buffer,"+Ok. Has ganado\n")==0 || strcmp(buffer,"+Ok. Has perdido\n")==0)
				{
					printf("\n%s\n", buffer);
					fin=1;
				}
				else
				{
					printf("\n%s\n", buffer);
				}
			}
			else if(strcmp(token,"+Ok.") ==0)
			{
				printf("\n%s\n", buffer);
			}
			else if(strcmp(token,"-Err:")==0)
			{
				printf("\n%s\n", buffer);
			}
			
			//bzero(buffer, sizeof(buffer));
			//printf("Mensaje del servidor: %s\n", buffer);
			// bzero(buffer, sizeof(buffer));
			//char * token = strtok(buffer, " ");
			//recv(sd, buffer, sizeof(buffer), 0);

			//Creacion del tablero
			//read ( sd, buffer, sizeof(buffer));
			
		}
		else
		{

			// He introducido información por teclado
			if (FD_ISSET(0, &auxfds))
			{
				bzero(buffer, sizeof(buffer));

				fgets(buffer, sizeof(buffer), stdin);

				if (strcmp(buffer, "SALIR\n") == 0)
				{
					fin = 1;
				}

				send(sd, buffer, sizeof(buffer), 0);
			}
		}

	} while (fin == 0);

	close(sd);
	free(tableroBarcos);
	return 0;
}