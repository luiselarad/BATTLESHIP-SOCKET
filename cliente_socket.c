#include "libreria.h"

int seleccionar_disparo(char mapa_disparos[MAXI][MAXJ], char buffer[TBUFFER])
{
    int i = MAXI / 2; //POSICION EN I INICIAL DEL SELECTOR
    int j = MAXJ / 2; //POSICION EN J INICIAL DEL SELECTOR
    char opcion = ' '; //VARIABLE DE LECTURA MOVIMIENTO
    char paso = mapa_disparos[i][j]; //PISO - VALOR DE LA CASILLA DONDE SE PONDRA EL SELECTOR
    mapa_disparos[i][j] = SELECTOR; //SE COLOCA EL SELECTOR
	imprimir_mapa(mapa_disparos);
    while (1) //CICLO DE LECTURA INFINITO HASTA OPCION VALIDA
    {
		printf("\n-DISPARANDO-\n");
        opcion = tolower(mygetch()); //LECTURA CARACTER AL PRESIONAR
        switch(opcion)
        {
            case 'w': //MOVER SELECTOR ARRIBA
                if((i-1 >= 0))
                {
                    mapa_disparos[i][j] = paso;
                    i--;
                    paso = marcar_paso(mapa_disparos, i, j);
                }
                break;
            case 's': //MOVER SELECTOR ABAJO
                if((i+1 < MAXI))
                {
                    mapa_disparos[i][j] = paso;
                    i++;
                    paso = marcar_paso(mapa_disparos, i, j);
                }
                break;
            case 'a': //MOVER SELECTOR IZQUIERDA
                if((j-1 >= 0))
                {
                    mapa_disparos[i][j] = paso;
                    j--;
                    paso = marcar_paso(mapa_disparos, i, j);
                }
                break;
            case 'd': //MOVER SELECTOR DERECHA
                if((j+1 < MAXJ))
                {
                    mapa_disparos[i][j] = paso;
                    j++;
                    paso = marcar_paso(mapa_disparos, i, j);
                }
                break;
			case 'e':
            case 'q':
					bzero(buffer,TBUFFER);
					buffer[0] = itoc(i);
					buffer[1] = itoc(j);
					return 1;
        }
        limpiar();
        imprimir_mapa(mapa_disparos);
    }
}

int main(int argc, char *argv[])
{
	limpiar();
	char mapa[MAXI][MAXJ];
	char mapa_disparo[MAXI][MAXJ];	
	int i = 0;
	int j = 0;	
	int aux = 0;
	char tbarco = DISPARO;

	inicializar_alanchas();
	inicializar_abuques();
	inicializar_aportaaviones();
		
	inicializar_mapa(mapa);
	inicializar_mapa_disparo(mapa_disparo);

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"USAR: %s -NombreHost- -Puerto- | EJEMPLO: %s localhost 1234 \n", argv[0], argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR: Apertura de Socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR: No existe tal Host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR: Conexion");

	//COMIENZA

	printf("\n-FASE DE COLOCACION DE BARCOS-\n");

	//ESPERA QUE EL SERVIDOR ORDENE SUS BARCOS
	printf("\nTurno del servidor...\n");
    bzero(buffer,TBUFFER);
    n = read(sockfd,buffer,TBUFFER-1); //1-READ
    if (n < 0) error("ERROR: Lectura desde el Socket");
	limpiar();

	//CLIENTE COLOCA SUS BARCOS
	printf("\n-TU TURNO-\n");
	printf("\nColoque sus barcos:\n");
	instrucciones();
	printf("Presione una tecla para continuar...\n");
	mygetch();
	colocar_barcos(mapa);
	imprimir_mapa(mapa);

	//ENVIO MI MAPA DE BARCOS AL SERVIDOR
	bzero(buffer,TBUFFER);
	mapa_buffer(mapa,buffer);
	n = write(sockfd,buffer,strlen(buffer)); //2-WRITE
    if (n < 0) error("ERROR: Escribiendo al socket");
	
	limpiar();
	imprimir_mapa(mapa);
	printf("\n-INICIO DEL JUEGO-\n");
	printf("\nEsperando jugada del servidor...\n");

	//EMPIEZA EL JUEGO
	
	while(1)
	{
		//CLIENTE LE DICE AL SERVIDOR QUE COMIENCE A DISPARAR
		bzero(buffer,TBUFFER);
		strncpy(buffer,"COMIENZA A DISPARAR",TBUFFER);
		n = write(sockfd,buffer,strlen(buffer)); //3-WRITE
		if (n < 0) error("ERROR: Escribiendo al socket");	
		
		//SERVIDOR COMIENZA DISPARANDO
		bzero(buffer,TBUFFER);
		n = read(sockfd,buffer,255); //4-READ
		if (n < 0) error("ERROR: Leyendo del socket");

		//RECIBO LA POSICION DONDE EL SERVIDOR DISPARO Y LA MARCO EN MI MAPA
		i = ctoi(buffer[0]);
		j = ctoi(buffer[1]);
		tbarco = DISPARO;
		aux = 0;			
		aux = es_barco(mapa,i,j);

		if (aux>0) tbarco = tipo_barco_herido(mapa, i, j);
			else tbarco = DISPARO;	

		pinta_disparo(mapa, i, j, DISPARO);
		
		//FEED BACK DISPARO DEL SERVIDOR EN MI MAPA
		limpiar();
		imprimir_mapa(mapa);
		
		//YA ACTUALICE MI MAPA
		bzero(buffer,TBUFFER);
		strncpy(buffer,"MAPA ACTUALIZADO",TBUFFER);
		n = write(sockfd,buffer,strlen(buffer)); //5-WRITE
		if (n < 0) error("ERROR: Escribiendo al socket");

		//RECIBO PETICION DE VERIFICACION HUNDIDO BARCO
		bzero(buffer,TBUFFER);
		n = read(sockfd,buffer,255); //6-READ
		if (n < 0) error("ERROR: Leyendo del socket");

		//ENVIO RESPUESTA DE VERIFICACION HUNDIDO BARCO
		aux = 0;
		aux = escribe_disparo(tbarco,i,j);
		switch(aux)
			{
				case 1: //SE HUNDIO UNA LANCHA
					bzero(buffer,TBUFFER);
					strncpy(buffer,"HAS HUNDIDO UNA LANCHA",TBUFFER);
					n = write(sockfd,buffer,strlen(buffer)); //7-WRITE
					if (n < 0) error("ERROR: Escribiendo al Socket.");
					break; 
				case 2:	//SE HUNDIO UN BUQUE
					bzero(buffer,TBUFFER);
					strncpy(buffer,"HAS HUNDIDO UN BUQUE",TBUFFER);
					n = write(sockfd,buffer,strlen(buffer)); //7-WRITE
					if (n < 0) error("ERROR: Escribiendo al Socket.");
					break;
				case 3: //SE HUNDIO UN PORTAAVIONES
					bzero(buffer,TBUFFER);
					strncpy(buffer,"HAS HUNDIDO UN PORTAAVIONES",TBUFFER);
					n = write(sockfd,buffer,strlen(buffer)); //7-WRITE
					if (n < 0) error("ERROR: Escribiendo al Socket.");
					break;
				default: //NO SE HUNDIO NADA
					bzero(buffer,TBUFFER);
					strncpy(buffer,"NO HAS HUNDIDO NADA",TBUFFER);
					n = write(sockfd,buffer,strlen(buffer)); //7-WRITE
					if (n < 0) error("ERROR: Escribiendo al Socket.");
					break;
			}

		//CONDICION DE PARADA, SI GANO SERVIDOR
		bzero(buffer,TBUFFER);
		n = read(sockfd,buffer,255); //8-READ
		if (n < 0) error("ERROR: Leyendo desde el Socket");
	 	
		if(strcmp(buffer,"NADIE HA GANADO") == 0) //EL SERVIDOR NO GANO
		{

				//CLIENTE DISPARA!!!!
				printf("\nPresione una tecla para continuar...\n");
				bzero(buffer,TBUFFER);
				//SELECCIONO MI I Y J DE MI DISPARO PARA ENVIARSELO AL CLIENTE Y QUE LO VALIDE
				seleccionar_disparo(mapa_disparo, buffer);
				//BUFFER[0] = i
				//BUFFER[1] = J
				n = write(sockfd,buffer,strlen(buffer)); //9-WRITE
				if (n < 0) error("ERROR: Escribiendo al Socket");
				i = 0;
				j = 0;
				i = ctoi(buffer[0]);
				j = ctoi(buffer[1]);	
				
				//RESPUESTA DEL SERVIDOR SOBRE MI DISPARO
				bzero(buffer,TBUFFER);
				n = read(sockfd,buffer,255); //10-READ
				if (n < 0) error("ERROR");

				if(strcmp(buffer, "¡HAS FALLADO EL DISPARO") == 0)
				{
					pinta_disparo(mapa_disparo, i, j, AGUA);
				
				}
				if(strcmp(buffer, "¡LE HAS DADO A UN ENEMIGO") == 0)
				{
					pinta_disparo(mapa_disparo, i, j, DISPARO);	
				}

				limpiar();
				imprimir_mapa(mapa_disparo);	
				printf("\n%s EN: [%i][%i]!\n", buffer, i, j);
				
				//ENVIO PETICION DE VERIFICACION SI HE HUNDIDO UN BARCO
				bzero(buffer,TBUFFER);
				strncpy(buffer,"HE HUNDIDO UN BARCO?",TBUFFER);
				n = write(sockfd,buffer,strlen(buffer)); //11-WRITE
				if (n < 0) error("ERROR: Escribiendo al Socket.");

				//RECIBO RESPUESTA A VERIFICACION SI HE HUNDIDO BARCO?
				bzero(buffer,TBUFFER);
				n = read(sockfd,buffer,TBUFFER-1); //12-READ
				if (n < 0) error("ERROR: Leyendo desde el socket.");
					
				if(strcmp(buffer, "HAS HUNDIDO UNA LANCHA") == 0)
				{	
					printf("%s\n", buffer); 
				}
				if(strcmp(buffer, "HAS HUNDIDO UN BUQUE") == 0)
				{	
					printf("%s\n", buffer); 
				}
				if(strcmp(buffer, "HAS HUNDIDO UN PORTAAVIONES") == 0)
				{	
					printf("%s\n", buffer); 
				}

				printf("\nPresione una tecla para continuar...\n");
				mygetch();
				limpiar();
				imprimir_mapa(mapa);
				printf("\nEsperando jugada del servidor...\n");
		}
		else
		{
			//GANO EL SERVIDOR
			printf("%s\n", buffer);
			break;
		}

	 	//CLIENTE GANO?
		bzero(buffer,TBUFFER);
		n = read(sockfd,buffer,255);  //13-READ
		if (n < 0) error("ERROR: Leyendo desde el Socket");
		
		if(strcmp(buffer, "HA GANADO EL CLIENTE") == 0)
		{
			printf("\n!HAS GANADO¡\n");
			break;
		}		
	}
	
	bzero(buffer,TBUFFER);
	strncpy(buffer,"TERMINO",TBUFFER);
	n = write(sockfd,buffer,strlen(buffer)); //14-WRITE
	if (n < 0) error("ERROR: Escribiendo al Socket.");
    close(sockfd);
    return 0;
}

