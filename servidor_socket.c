#include "libreria.h"

//DISPARO
int disparo_es_efectivo(char mapa[MAXI][MAXJ], int i, int j)
{
    if(es_barco(mapa, i, j)) //LA CASILLA DONDE SE DISPARA ES UN BARCO
    {
        return 1;
    }
    else
    {
        if(es_agua(mapa, i, j)) //LA CASILLA DONDE SE DISPARA ES AGUA
        {
            return 0;
        }
    }
    return -1; //YA DISPARO A ESA CASILLA
}

int seleccionar_disparo(char mapa_disparos[MAXI][MAXJ], char mapa_barcos[MAXI][MAXJ], char buffer[TBUFFER])
{
    int efectivo;
    int i = MAXI / 2; //POSICION EN I INICIAL DEL SELECTOR
    int j = MAXJ / 2; //POSICION EN J INICIAL DEL SELECTOR
    char opcion = ' '; //VARIABLE DE LECTURA MOVIMIENTO
    char paso = mapa_disparos[i][j]; //PISO - VALOR DE LA CASILLA DONDE SE PONDRA EL SELECTOR
    mapa_disparos[i][j] = SELECTOR; //SE COLOCA EL SELECTOR
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
                efectivo = disparo_es_efectivo(mapa_barcos, i ,j);
                switch(efectivo)
                {
                    case -1:
						imprimir_mapa(mapa_disparos);
                        printf("\nYA HABIAS DISPARADO EN LA CASILLA: [%i][%i]\n", i, j);				
                        printf("\nPresione una tecla para continuar...\n");
						mygetch();
                        return -1;
                    case 0:
                       
                        pinta_disparo(mapa_disparos, i, j, AGUA);
                        pinta_disparo(mapa_barcos, i, j, DISPARO);
						bzero(buffer,TBUFFER);
						buffer[0] = itoc(i);
						buffer[1] = itoc(j);

						imprimir_mapa(mapa_disparos);
						printf("\n¡HAS FALLADO EL DISPARO EN: [%i][%i]!\n", i, j);
                        printf("\nPresione una tecla para continuar...\n");
						mygetch();
                        return 0;
                    case 1:
                        pinta_disparo(mapa_disparos, i, j, DISPARO);
                        pinta_disparo(mapa_barcos, i, j, DISPARO);
						bzero(buffer,TBUFFER);
						buffer[0] = itoc(i);
						buffer[1] = itoc(j);
						
						imprimir_mapa(mapa_disparos);
						printf("\n¡LE HAS DADO A UN ENEMIGO EN [%i][%i]!\n", i, j);
                        printf("\nPresione una tecla para continuar...\n");
						mygetch();
                        return 1;
                }
                break;
        }
        limpiar();
        imprimir_mapa(mapa_disparos);
    }
}

int seleccionar_disparo_cliente(char mapa_disparos[MAXI][MAXJ], char mapa_barcos[MAXI][MAXJ], int i, int j, char buffer[TBUFFER])
{
    int efectivo;
    efectivo = disparo_es_efectivo(mapa_barcos, i ,j);
    switch(efectivo)
    {
    	case -1:
			bzero(buffer,TBUFFER);
			strncpy(buffer,"YA HABIAS DISPARADO EN ESTA CASILLA",TBUFFER);
       		return -1;
      	case 0:
			bzero(buffer,TBUFFER);
			strncpy(buffer,"¡HAS FALLADO EL DISPARO",TBUFFER);
        	return 0;
       	case 1:
			bzero(buffer,TBUFFER);
			strncpy(buffer,"¡LE HAS DADO A UN ENEMIGO",TBUFFER);
        	return 1;
  	}
}

int gano(int puntaje)
{
    int puntaje_lancha = TLANCHA * MLANCHA;
    int puntaje_buque = TBUQUE * MBUQUE;
    int puntaje_portaaviones = TPORTAAVIONES * MPORTAAVIONES;

    if(puntaje == (puntaje_lancha + puntaje_buque + puntaje_portaaviones))
        return 1;
    return 0;
}



int main(int argc, char *argv[])
{
	limpiar();
	char mapa[MAXI][MAXJ];
	char mapa_disparo[MAXI][MAXJ];
	int puntaje_servidor = 0;

	char mapa_cliente[MAXI][MAXJ];
	char mapa_cliente_disparo[MAXI][MAXJ];
	int cliente_i = 0;
	int cliente_j = 0;
	int aux = -1;
	int puntaje_cliente = 0;
	char tbarco = DISPARO;

	inicializar_alanchas();
	inicializar_abuques();
	inicializar_aportaaviones();

	inicializar_mapa(mapa);
	inicializar_mapa_disparo(mapa_disparo);

	inicializar_mapa(mapa_cliente);
	inicializar_mapa_disparo(mapa_cliente_disparo);

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[TBUFFER];
	struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2)
    {
        fprintf(stderr,"ERROR: No se ha indicado un puerto.\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR: Apertura de socket.");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0)
            error("ERROR: En Union.");
    listen(sockfd,1);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
            (struct sockaddr *) &cli_addr,
            &clilen);
    if (newsockfd < 0)
        error("ERROR: En Aceptar.");

	//COMIENZA

	printf("\n-FASE DE COLOCACION DE BARCOS-\n");

	//SERVIDOR ORDENA SUS BARCOS
	printf("\n-TU TURNO-\n");
	printf("\nColoque sus barcos:\n");
	instrucciones();
	printf("Presione una tecla para continuar...\n");
	mygetch();
	colocar_barcos(mapa);
	imprimir_mapa(mapa);

	//TURNO DEL CLIENTE DE ORDENAR SUS BARCOS
    	bzero(buffer,TBUFFER);
	strncpy(buffer,"-TU TURNO-",TBUFFER);
	n = write(newsockfd,buffer,strlen(buffer)); //1-WRITE
    	if (n < 0) error("ERROR: Escribiendo en Socket.");	

	//RECIBO EL MAPA CON LOS BARCOS DEL CLIENTE
	printf("\nEsperando que el cliente posicione su flota...\n");
	bzero(buffer,TBUFFER);
    n = read(newsockfd,buffer,TBUFFER-1); //2-READ
    if (n < 0) error("ERROR: Lectura desde el socket.");
    limpiar();
	buffer_mapa(buffer,mapa_cliente);
	imprimir_mapa(mapa);
	printf("\n-INICIO DEL JUEGO-\n");
	
	//EMPIEZA EL JUEGO
	while(1)
	{
		//ESPERANDO QUE EL CLIENTE ESTE LISTO PARA EL DISPARO DEL SERVER	
		bzero(buffer,TBUFFER);
    		n = read(newsockfd,buffer,TBUFFER-1); //3-READ
    		if (n < 0) error("ERROR: Leyendo desde el Socket.");
		
		printf("\nPresione una tecla para continuar...\n");

		//DISPARA EL SERVIDOR
		imprimir_mapa(mapa_disparo);
		bzero(buffer,TBUFFER);
		if(seleccionar_disparo(mapa_disparo, mapa_cliente, buffer))
			puntaje_servidor++;
		
		//ENVIO I-J AL CLIENTE PARA QUE ACTUALICE SU MAPA DE BARCOS
		//BUFFER[0] = i
		//BUFFER[1] = J
		n = write(newsockfd,buffer,strlen(buffer)); //4-WRITE
		if (n < 0) error("ERROR: Escribiendo desde el Socket.");

		//CLIENTE ACTUALIZO SU MAPA CON MI DISPARO
		bzero(buffer,TBUFFER);
    	n = read(newsockfd,buffer,TBUFFER-1); //5-READ
    	if (n < 0) error("ERROR: Leyendo desde el socket.");
		
		//ENVIO PETICION DE VERIFICACION SI HE HUNDIDO UN BARCO
		bzero(buffer,TBUFFER);
		strncpy(buffer,"HE HUNDIDO UN BARCO?",TBUFFER);
		n = write(newsockfd,buffer,strlen(buffer)); //6-WRITE
		if (n < 0) error("ERROR: Escribiendo al Socket.");

		//RECIBO RESPUESTA A VERIFICACION SI HE HUNDIDO BARCO?
		bzero(buffer,TBUFFER);
    	n = read(newsockfd,buffer,TBUFFER-1); //7-READ
    	if (n < 0) error("ERROR: Leyendo desde el socket.");
				
		limpiar();
		imprimir_mapa(mapa_disparo);

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

		//CONDICION DE PARADA, SI EL SERVIDOR GANO
		if(gano(puntaje_servidor))
		{
			printf("\nHA GANADO EL SERVIDOR\n");
			bzero(buffer,TBUFFER);
			strncpy(buffer,"HA GANADO EL SERVIDOR",TBUFFER);
			n = write(newsockfd,buffer,strlen(buffer)); //8-WRITE
			if (n < 0) error("ERROR: Escribiendo al Socket.");
			break;
		}
		else
		{
			//NADIE HA GANADO AUN
			bzero(buffer,TBUFFER);
			strncpy(buffer,"NADIE HA GANADO",TBUFFER);
			n = write(newsockfd,buffer,strlen(buffer)); //8-WRITE
			if (n < 0) error("ERROR: Escribiendo al Socket.");
		}
		
		imprimir_mapa(mapa);	
		printf("\nTurno del cliente...\n");
		
		//CLIENTE DISPARA
		bzero(buffer,TBUFFER);
    		n = read(newsockfd,buffer,TBUFFER-1); //9-READ
    		if (n < 0) error("ERROR: Leyendo desde el socket.");
			cliente_i = 0;
			cliente_j = 0;
			cliente_i = ctoi(buffer[0]);
			cliente_j = ctoi(buffer[1]);
			
			aux = 0;
			aux = seleccionar_disparo_cliente(mapa_cliente_disparo, mapa, cliente_i, cliente_j, buffer);
			//FEED BACK DISPARO DEL CLIENTE EN MI MAPA
			limpiar();
			if (aux > 0)
			{
				tbarco = tipo_barco_herido(mapa, cliente_i, cliente_j);
			}
			else
			{
				tbarco = DISPARO;
			}

			switch(aux)
			{
				case -1: //DISPARO EN UNA CASILLA DONDE YA DISPARO
				case 0: //DISPARO CAYO EN AGUA
					pinta_disparo(mapa_cliente_disparo, cliente_i, cliente_j, AGUA);
		   			pinta_disparo(mapa, cliente_i, cliente_j, DISPARO);
					n = write(newsockfd,buffer,strlen(buffer)); //10-WRITE
					if (n < 0) error("ERROR: Escribiendo al socket.");
					break;
				case 1: //DISPARO CAYO EN UN BARCO
					pinta_disparo(mapa_cliente_disparo, cliente_i, cliente_j, DISPARO);
        				pinta_disparo(mapa, cliente_i, cliente_j, DISPARO);
					n = write(newsockfd,buffer,strlen(buffer)); //10-WRITE
					if (n < 0) error("ERROR: Escribiendo al socket");
					puntaje_cliente++;
					break;
			}
			imprimir_mapa(mapa);

			//RECIBO PETICION DE VERIFICACION HUNDIDO BARCO
			bzero(buffer,TBUFFER);
			n = read(newsockfd,buffer,255); //11-READ
			if (n < 0) error("ERROR: Leyendo del socket");
			
			//ENVIO RESPUESTA DE VERIFICACION HUNDIDO BARCO
			aux = 0;
			aux = escribe_disparo(tbarco, cliente_i, cliente_j);
			switch(aux)
			{
				case 1: //SE HUNDIO UNA LANCHA
					bzero(buffer,TBUFFER);
					strncpy(buffer,"HAS HUNDIDO UNA LANCHA",TBUFFER);
					n = write(newsockfd,buffer,strlen(buffer)); //12-WRITE
					if (n < 0) error("ERROR: Escribiendo al Socket.");
					break; 
				case 2:	//SE HUNDIO UN BUQUE
					bzero(buffer,TBUFFER);
					strncpy(buffer,"HAS HUNDIDO UN BUQUE",TBUFFER);
					n = write(newsockfd,buffer,strlen(buffer)); //12-WRITE
					if (n < 0) error("ERROR: Escribiendo al Socket.");
					break;
				case 3: //SE HUNDIO UN PORTAAVIONES
					bzero(buffer,TBUFFER);
					strncpy(buffer,"HAS HUNDIDO UN PORTAAVIONES",TBUFFER);
					n = write(newsockfd,buffer,strlen(buffer)); //12-WRITE
					if (n < 0) error("ERROR: Escribiendo al Socket.");
					break;
				default: //NO SE HUNDIO NADA 
					bzero(buffer,TBUFFER);
					strncpy(buffer,"NO HAS HUNDIDO NADA",TBUFFER);
					n = write(newsockfd,buffer,strlen(buffer)); //12-WRITE
					if (n < 0) error("ERROR: Escribiendo al Socket.");
					break;
			}
			printf("\nEsperando por el cliente...\n");

		//CONDICION DE PARADA, SI EL CLIENTE GANO
		if(gano(puntaje_cliente))
		{
			printf("\nHA GANADO EL CLIENTE\n");
			bzero(buffer,TBUFFER);
			strncpy(buffer,"HA GANADO EL CLIENTE",TBUFFER);
			n = write(newsockfd,buffer,strlen(buffer)); //13-WRITE
			if (n < 0) error("ERROR: Escribiendo al Socket.");

			break;
		}
		else
		{
			//NADIE HA GANADO AUN
			bzero(buffer,TBUFFER);
			strncpy(buffer,"NADIE HA GANADO",TBUFFER);
			n = write(newsockfd,buffer,strlen(buffer)); //13-WRITE
			if (n < 0) error("ERROR: Escribiendo al Socket.");
		}
	}
	

	//ESPERANDO QUE CLIENTE TERMINE
	bzero(buffer,TBUFFER);
	n = read(newsockfd,buffer,255); //14-READ
	if (n < 0) error("ERROR: Leyendo del socket");

	close(newsockfd);
    close(sockfd);

    return 0;
}
