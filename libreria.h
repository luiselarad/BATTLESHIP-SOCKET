#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>
#include <termios.h>
//#include <curses.h>


//MACROS

#define MAXI 10 //TAMAÑO MAXIMO X DEL MAPA
#define MAXJ 10 //TAMAÑO MAXIMO Y DEL MAPA
#define AGUA '~' //CARACTER DEL AGUA
#define OCULTO '.' //CARACTER OCULTO
#define DISPARO 'X' //CARACTER DEL DISPARO
#define SELECTOR 'O' //CARACTER DEL SELECTOR PARA SELECCIONAR DISPARO

#define CPORTAAVIONES 'P' //CARACTER DEL PORTAACVIONES
#define TPORTAAVIONES 5 //TAMAÑO DEL PORTAAVIONES
#define MPORTAAVIONES 1 //CANTIDAD MAXIMA DE PORTAAVIONES EN TABLERO

#define CBUQUE 'B' //CARACTER DEL BUQUE
#define TBUQUE 3 //TAMAÑO DEL BUQUE
#define MBUQUE 1 //CANTIDAD MAXIMA DE BUQUES EN EL TABLERO

#define CLANCHA 'L' //CARACTER DE LA LANCHA
#define TLANCHA 1 //TAMAÑO DE LA LANCHA
#define MLANCHA 1 //CANTIDAD MAXIMA DE LANCHAS EN EL TABLERO

#define TBUFFER 256 //TAMAÑO DEL BUFFER


struct CASILLAS{
	int i;
	int j;
	int estado;
};

struct LANCHAS{
	struct CASILLAS acasillas[TLANCHA];
	int estado;
};

struct BUQUES{
	struct CASILLAS acasillas[TBUQUE];
	int estado;
};

struct PORTAAVIONES{
	struct CASILLAS acasillas[TPORTAAVIONES];
	int estado;
};


//ARREGLOS PARA BARCO HUNDIDO
struct BUQUES abuques[MBUQUE];
struct PORTAAVIONES aportaaviones[MPORTAAVIONES];
struct LANCHAS alanchas[MLANCHA];


int mygetch(){
	struct termios oldt, newt;
	int ch;
	
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO );
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

void limpiar(){
	system("clear");
	/*clear();
	refresh();*/
}

void inicializar_abuques()
{
	int i;int j;
	for(i = 0; i < MBUQUE; i++)
	{
		abuques[i].estado = -1;
		for(j = 0; j < TBUQUE; j++)
		{
			abuques[i].acasillas[j].i = 0;
			abuques[i].acasillas[j].j = 0;
			abuques[i].acasillas[j].estado = 0;
		}
	}
	
}

void inicializar_aportaaviones()
{
	int i;int j;
	for(i = 0; i < MPORTAAVIONES; i++)
	{
		aportaaviones[i].estado = -1;
		for(j = 0; j < TPORTAAVIONES; j++)
		{
			aportaaviones[i].acasillas[j].i = 0;
			aportaaviones[i].acasillas[j].j = 0;
			aportaaviones[i].acasillas[j].estado = 0;
		}
	}
	
}

void inicializar_alanchas() //FUNCIONAL
{
	int i;
	int j;
	for(i = 0; i < MLANCHA; i++)
	{
		alanchas[i].estado = -1;
		for(j = 0; j < TLANCHA; j++)
		{
			alanchas[i].acasillas[j].i = 0;
			alanchas[i].acasillas[j].j = 0;
			alanchas[i].acasillas[j].estado = 0;
		}
	}
	
}

void imprimir_alanchas() //FUNCIONAL
{
	int i;
	int j;
	for(i = 0; i < MLANCHA; i++)
	{
		printf("alanchas[%i].estado = %i\n",i,alanchas[i].estado);
		for(j = 0; j < TLANCHA; j++)
		{
			printf("alanchas[%i].acasillas[%i].i = %i\n",i,j,alanchas[i].acasillas[j].i);
			printf("alanchas[%i].acasillas[%i].j = %i\n",i,j,alanchas[i].acasillas[j].j);
			printf("alanchas[%i].acasillas[%i].estado = %i\n",i,j,alanchas[i].acasillas[j].estado);
		}
		printf("\n\n");
	}
	
}

void imprimir_abuques()
{
	int i;int j;
	for(i = 0; i < MBUQUE; i++)
	{
		printf("abuques[%i].estado = %i\n",i,abuques[i].estado);
		for(j = 0; j < TBUQUE; j++)
		{
			printf("abuques[%i].acasillas[%i].i = %i\n",i,j,abuques[i].acasillas[j].i);
			printf("abuques[%i].acasillas[%i].j = %i\n",i,j,abuques[i].acasillas[j].j);
			printf("abuques[%i].acasillas[%i].estado = %i\n",i,j,abuques[i].acasillas[j].estado);
		}
	}
	
}

void posicion_lancha(int i, int j) //FUNCIONAL
{
	int x,y;
	for(x = 0; x < MLANCHA; x++)
	{
		if(alanchas[x].estado == -1)
		{
			alanchas[x].acasillas[0].i = i;
			alanchas[x].acasillas[0].j = j;
			alanchas[x].estado = 0;
			return;
		}
	}
}

void posicion_buque(int i, int j, char orientacion)
{
	int x,y;
	int auxi, auxj;
	auxi = i;
	auxj = j;
	for(x = 0; x < MBUQUE; x++)
	{
		if(abuques[x].estado == -1)
		{
			switch(orientacion)
			{
				case 'w':
					for(y = 0; y < TBUQUE; y++)
					{
						abuques[x].acasillas[y].i = auxi;
						abuques[x].acasillas[y].j = auxj;
						auxi--;
					}
					break;
				case 's':
					for(y = 0; y < TBUQUE; y++)
					{
						abuques[x].acasillas[y].i = auxi;
						abuques[x].acasillas[y].j = auxj;
						auxi++;
					}
					break;
				case 'a':
					for(y = 0; y < TBUQUE; y++)
					{
						abuques[x].acasillas[y].i = auxi;
						abuques[x].acasillas[y].j = auxj;
						auxj--;
					}
					break;
				case 'd':
					for(y = 0; y < TBUQUE; y++)
					{
						abuques[x].acasillas[y].i = auxi;
						abuques[x].acasillas[y].j = auxj;
						auxj++;
					}
					break;
			}
			abuques[x].estado = 0;
			return;
		}
	}
}

void posicion_portaaviones(int i, int j, char orientacion)
{
	int x,y;
	int auxi, auxj;
	auxi = i;
	auxj = j;
	for(x = 0; x < MPORTAAVIONES; x++)
	{
		if(aportaaviones[x].estado == -1)
		{
			switch(orientacion)
			{
				case 'w':
					for(y = 0; y < TPORTAAVIONES; y++)
					{
						aportaaviones[x].acasillas[y].i = auxi;
						aportaaviones[x].acasillas[y].j = auxj;
						auxi--;
					}
					break;
				case 's':
					for(y = 0; y < TPORTAAVIONES; y++)
					{
						aportaaviones[x].acasillas[y].i = auxi;
						aportaaviones[x].acasillas[y].j = auxj;
						auxi++;
					}
					break;
				case 'a':
					for(y = 0; y < TPORTAAVIONES; y++)
					{
						aportaaviones[x].acasillas[y].i = auxi;
						aportaaviones[x].acasillas[y].j = auxj;
						auxj--;
					}
					break;
				case 'd':
					for(y = 0; y < TPORTAAVIONES; y++)
					{
						aportaaviones[x].acasillas[y].i = auxi;
						aportaaviones[x].acasillas[y].j = auxj;
						auxj++;
					}
					break;
			}
			aportaaviones[x].estado = 0;
			return;
		}
	}
}

int hundido_lancha(int pos_i, int pos_j)
{
	int i, j;
	int hundido = 0;
	for(i=0 ; i < MLANCHA ; i++)
	{
		if(alanchas[i].estado == 0) //BARCO NO ESTA HUNDIDO
		{
			for(j=0; j < TLANCHA; j++)
			{
				if((alanchas[i].acasillas[j].i == pos_i) && (alanchas[i].acasillas[j].j == pos_j))
				{
					alanchas[i].acasillas[j].estado = 1; //MARCO QUE DISPARARON A ESA CASILLA
					//VALIDO SI EL BARCO TIENE TODAS SUS CASILLAS CON DISPARO POR LO TANTO EL BARCO FUE HUNDIDO
					for(j=0; j < TLANCHA; j++)
					{
						if(alanchas[i].acasillas[j].estado == 1) //LA CASILLA ESTA HUNDIDA
						{
							hundido++;
							if(hundido == TLANCHA) //SI TIENE TODAS LAS CASILLAS HUNDIDAS
								return 1; //BARCO HUNDIDO
						}
						else
							return 0; //UNA CASILLA CON ESTADO 0, SIN DISPARO
					}
					return 0; //AUN TIENE CASILLAS CON ESTADO 0, SIN DISPARO
				}
			}
		}
	}

	return 0; //AUN TIENE CASILLAS CON ESTADO 0, SIN DISPARO
}

int hundido_buque(int pos_i, int pos_j)
{
	int i, j;
	int hundido = 0;
	for(i=0 ; i < MBUQUE ; i++)
	{
		if(abuques[i].estado == 0) //BARCO NO ESTA HUNDIDO
		{
			for(j=0; j < TBUQUE; j++)
			{
				if((abuques[i].acasillas[j].i == pos_i) && (abuques[i].acasillas[j].j == pos_j))
				{
					abuques[i].acasillas[j].estado = 1; //MARCO QUE DISPARARON A ESA CASILLA
					//VALIDO SI EL BARCO TIENE TODAS SUS CASILLAS CON DISPARO POR LO TANTO EL BARCO FUE HUNDIDO
					for(j=0; j < TBUQUE; j++)
					{
						if(abuques[i].acasillas[j].estado == 1) //LA CASILLA ESTA HUNDIDA
						{
							hundido++;
							if(hundido == TBUQUE) //SI TIENE TODAS LAS CASILLAS HUNDIDAS
								return 1; //BARCO HUNDIDO
						}
						else
							return 0; //UNA CASILLA CON ESTADO 0, SIN DISPARO
					}
					return 0; //AUN TIENE CASILLAS CON ESTADO 0, SIN DISPARO
				}
			}
		}
	}

	return 0; //AUN TIENE CASILLAS CON ESTADO 0, SIN DISPARO
}

int hundido_portaaviones(int pos_i, int pos_j)
{
	int i, j;
	int hundido = 0;
	for(i=0 ; i < MPORTAAVIONES ; i++)
	{
		if(aportaaviones[i].estado == 0) //BARCO NO ESTA HUNDIDO
		{
			for(j=0; j < TPORTAAVIONES; j++)
			{
				if((aportaaviones[i].acasillas[j].i == pos_i) && (aportaaviones[i].acasillas[j].j == pos_j))
				{
					aportaaviones[i].acasillas[j].estado = 1; //MARCO QUE DISPARARON A ESA CASILLA
					//VALIDO SI EL BARCO TIENE TODAS SUS CASILLAS CON DISPARO POR LO TANTO EL BARCO FUE HUNDIDO
					for(j=0; j < TPORTAAVIONES; j++)
					{
						if(aportaaviones[i].acasillas[j].estado == 1) //LA CASILLA ESTA HUNDIDA
						{
							hundido++;
							if(hundido == TPORTAAVIONES) //SI TIENE TODAS LAS CASILLAS HUNDIDAS
								return 1; //BARCO HUNDIDO
						}
						else
							return 0; //UNA CASILLA CON ESTADO 0, SIN DISPARO
					}
					return 0; //AUN TIENE CASILLAS CON ESTADO 0, SIN DISPARO
				}
			}
		}
	}

	return 0; //AUN TIENE CASILLAS CON ESTADO 0, SIN DISPARO
}


int ctoi(char letra)
{
	return letra - '0';
}

char itoc(int digito)
{
	return digito + '0';
}


void instrucciones()
{
	printf("\n");
	printf("MOVIMIENTOS: \n");
	printf("W : ARRIBA.\n");
	printf("S : ABAJO.\n");
	printf("A : IZQUIERDA.\n");
	printf("D : DERECHA.\n");
	printf("Q o E : SELECCIONAR.\n");
	printf("\n");
}

void error(const char *msg)
{
	printf("\nPresione una tecla para continuar...\n");
    perror(msg);
    exit(1);
}

//FUNCION QUE INICIALIZA LA MATRIZ CORRESPONDIENTE AL MAPA EN PURA AGUA
void inicializar_mapa(char mapa[MAXI][MAXJ])
{
    int i;
    int j;

    for(i = 0; i < MAXI; i++)
    {
        for(j = 0; j < MAXJ; j++)
        {
            mapa[i][j] = AGUA;
        }
    }
}

//FUNCION QUE INICIALIZA LA MATRIZ CORRESPONDIENTE AL MAPA EN PURO OCULTO
void inicializar_mapa_disparo(char mapa[MAXI][MAXJ])
{
    int i;
    int j;

    for(i = 0; i < MAXI; i++)
    {
        for(j = 0; j < MAXJ; j++)
        {
            mapa[i][j] = OCULTO;
        }
    }
}

void imprimir_mapa(char mapa[MAXI][MAXJ])
{
    int i;
    int j;
    printf("\n\n");
    printf("   ");
    for(j = 0; j < MAXJ; j++)
    {
        printf("     %i ",j);
    }
    for(i = 0; i < MAXI; i++)
    {
        printf("\n \n");
        
	printf("   %i", i);

        for(j = 0; j < MAXJ; j++)
        {
            printf("    %c  ", mapa[i][j]);
        }
    }
    printf("\n \n");
}

char marcar_paso(char mapa[MAXI][MAXJ], int i, int j)
{
    char anterior = mapa[i][j];
    mapa[i][j] = SELECTOR;
    return anterior;
}

void pinta_disparo(char mapa[MAXI][MAXJ], int i, int j, char tipo)
{
    mapa[i][j] = tipo;
}

char tipo_barco_herido(char mapa[MAXI][MAXJ], int i, int j){

	if(mapa[i][j] == AGUA || mapa[i][j] == DISPARO)
        	return DISPARO;
    	return mapa[i][j];
}

void pinta_barco(char mapa[MAXI][MAXJ], int i, int j, char tipo)
{
    char upper_tipo = toupper(tipo);
    switch(upper_tipo)
    {
        case CLANCHA:
            mapa[i][j] = CLANCHA;
            break;
        case CBUQUE:
            mapa[i][j] = CBUQUE;
            break;
        case CPORTAAVIONES:
            mapa[i][j] = CPORTAAVIONES;
            break;
     }
}

int escribe_disparo(char tipo, int i, int j){

    switch(tipo)
    {
        case CLANCHA:
            printf("\nLE HAN DADO A TU LANCHA EN: [%i][%i] \n",i,j);
			if(hundido_lancha(i,j)) //VERIFICO SI EL DISPARO ME HUNDIO EL BARCO
			{
				printf("\nHAN HUNDIDO TU LANCHA\n");
				return 1;
			}
            break;
        case CBUQUE:
            printf("\nLE HAN DADO A TU BUQUE EN: [%i][%i] \n",i,j);
			if(hundido_buque(i,j)) //VERIFICO SI EL DISPARO ME HUNDIO EL BARCO
			{				
				printf("\nHAN HUNDIDO TU BUQUE \n");
				return 2;
			}
            break;
        case CPORTAAVIONES:
            printf("\nLE HAN DADO A TU PORTAAVIONES EN: [%i][%i] \n",i,j);
			if(hundido_portaaviones(i,j)) //VERIFICO SI EL DISPARO ME HUNDIO EL BARCO
			{
				printf("\nHAN HUNDIDO TU PORTAAVIONES \n");
				return 3;
			}
            break;
		case DISPARO:
            printf("\nEL ENEMIGO HA FALLADO, DISPARO A: [%i][%i] \n",i,j);
			return -1;
            break;
     }
}

void marcar_barco(char mapa[MAXI][MAXJ], int i, int j, char tipo, char orientacion, char tamano)
{
    switch(orientacion)
    {
        case 'w':
            for(; ((i >= 0) && (tamano > 0)); i--, tamano--)
            {
                pinta_barco(mapa ,i, j, tipo);
            }
            break;
        case 's':
            for(; ((i < MAXI) && (tamano > 0)); i++, tamano--)
            {
                pinta_barco(mapa, i, j, tipo);
            }
            break;
        case 'a':
            for(; ((j >= 0) && (tamano > 0)); j--, tamano--)
            {
                pinta_barco(mapa, i, j, tipo);
            }
            break;
        case 'd':
            for(; ((j < MAXJ) && (tamano > 0)); j++, tamano--)
            {
                pinta_barco(mapa, i, j, tipo);
            }
            break;
        case 'p':
            pinta_barco(mapa, i, j,tipo);
            break;
    }
}

int es_agua(char mapa[MAXI][MAXJ], int i, int j)
{
    if(mapa[i][j] == AGUA)
        return 1;
    return 0;
}

int es_barco(char mapa[MAXI][MAXJ], int i, int j)
{
    if(mapa[i][j] == AGUA || mapa[i][j] == DISPARO)
        return 0;
    return 1;
}

//VALIDACION DE CUPO DEL BARCO EN LA ORIENTACION Y POSICION INICIAL EN EL MAPA
int validar_posicion(char mapa[MAXI][MAXJ], int i, int j, char orientacion, int tamano)
{
    int valido = 0; //SI VALIDO ES IGUAL A EL TAMAÑO DEL BARCO, EL BARCO ENTRO
    switch(orientacion)
    {
        case 'w': //ARRIBA
            for(; i >= 0; i--)
            {
                if(es_agua(mapa, i, j)) //SI SE PUEDE COLOCAR EL BARCO EN ESA CASILLA
                {
                    valido++;
                    if(valido == tamano) //EL BARCO ENTRO COMPLETO
                    {
                        return 1;
                    }
                }
                else
                {
                    return 0; //NO ES AGUA EL BARCO NO SE PUEDE COLOCAR
                }
            }
            break;
        case 's':
            for(; i < MAXI; i++)
            {
                if(es_agua(mapa, i, j)) //SI SE PUEDE COLOCAR EL BARCO EN ESA CASILLA
                {
                    valido++;
                    if(valido == tamano) //EL BARCO ENTRO COMPLETO
                        return 1;
                }
                else
                {
                    return 0; //NO ES AGUA EL BARCO NO SE PUEDE COLOCAR
                }
            }
            break;
        case 'a':
            for(; j >= 0; j--)
            {
                if(es_agua(mapa, i, j)) //SI SE PUEDE COLOCAR EL BARCO EN ESA CASILLA
                {
                    valido++;
                    if(valido == tamano) //EL BARCO ENTRO COMPLETO
                        return 1;
                }
                else
                {
                    return 0; //NO ES AGUA EL BARCO NO SE PUEDE COLOCAR
                }
            }
            break;
        case 'd':
            for(; j < MAXJ; j++)
            {
                if(es_agua(mapa, i, j)) //SI SE PUEDE COLOCAR EL BARCO EN ESA CASILLA
                {
                    valido++;
                    if(valido == tamano) //EL BARCO ENTRO COMPLETO
                        return 1;
                }
                else
                {
                    return 0; //NO ES AGUA EL BARCO NO SE PUEDE COLOCAR
                }
            }
            break;
        case 'p':
            if(es_agua(mapa, i, j))
            {
                return 1;
            }
            else
            {
                return 0;
            }
            break;
        default:
            break;
    }
    return 0; //NO ES AGUA EL BARCO NO SE PUEDE COLOCAR
}


//VALIDACIONES DE BARCOS TIPO - ORIENTACION
int validar_barco(char mapa[MAXI][MAXJ], int i, int j, char tipo, char orientacion)
{
    switch(tipo)
    {
        case 'L': //LANCHA
            if(validar_posicion(mapa, i, j, orientacion, TLANCHA))
            {
                marcar_barco(mapa, i, j, tipo, orientacion, TLANCHA);
                return 1;
            }
            break;
        case 'B': //BUQUE
            if(validar_posicion(mapa, i, j, orientacion, TBUQUE))
            {
                marcar_barco(mapa, i, j, tipo, orientacion, TBUQUE);
                return 1;
            }
            break;
        case 'P': //PORTAAVIONES
            if(validar_posicion(mapa, i, j, orientacion, TPORTAAVIONES))
            {
                marcar_barco(mapa, i, j, tipo, orientacion, TPORTAAVIONES);
                return 1;
            }
            break;
    }
    return 0;
}

char seleccionar_orientacion()
{
    printf(" SELECCIONE LA ORIENTACION DE SU BARCO:\n");
    printf(" W: ARRIBA\n");
    printf(" S: ABAJO\n");
    printf(" A: IZQUIERDA\n");
    printf(" D: DERECHA\n");
    printf(" N: NINGUNA\n");
    while(1)
    {
        char orientacion = tolower(mygetch());
        switch(orientacion){
            case 'w':
                return 'w';
            case 's':
                return 's';
            case 'a':
                return 'a';
            case 'd':
                return 'd';
            case 'n':
                return 'n';
            default:
                orientacion = ' ';
                break;
        }
    }

}

void seleccionar_colocar(char mapa[MAXI][MAXJ], char tipo)
{
    int i = MAXI / 2; //POSICION EN I INICIAL DEL SELECTOR
    int j = MAXJ / 2; //POSICION EN J INICIAL DEL SELECTOR
    char opcion = ' '; //VARIABLE DE LECTURA MOVIMIENTO
    char orientacion = 'p'; //VARIABLE DE LECTURA ORIENTACION BARCO, SI ORIENTACION ES P CORRESPONDE A PUNTO, EL BARCO ES DE UNA SOLA CASILLA
    char paso = mapa[i][j]; //PISO - VALOR DE LA CASILLA DONDE SE PONDRA EL SELECTOR
    mapa[i][j] = SELECTOR; //SE COLOCA EL SELECTOR
	imprimir_mapa(mapa);
    while (1) //CICLO DE LECTURA INFINITO HASTA COLOCAR TODOS LOS BARCOS
    {
		printf("\n-COLOCANDO BARCOS-\n");
        opcion = tolower(mygetch()); //LECTURA CARACTER AL PRESIONAR
        switch(opcion)
        {
            case 'w': //MOVER SELECTOR ARRIBA
                if((i-1 >= 0))
                {
                    mapa[i][j] = paso;
                    i--;
                    paso = marcar_paso(mapa, i, j);
                }
                break;
            case 's': //MOVER SELECTOR ABAJO
                if((i+1 < MAXI))
                {
                    mapa[i][j] = paso;
                    i++;
                    paso = marcar_paso(mapa, i, j);
                }
                break;
            case 'a': //MOVER SELECTOR IZQUIERDA
                if((j-1 >= 0))
                {
                    mapa[i][j] = paso;
                    j--;
                    paso = marcar_paso(mapa, i, j);
                }
                break;
            case 'd': //MOVER SELECTOR DERECHA
                if((j+1 < MAXJ))
                {
                    mapa[i][j] = paso;
                    j++;
                    paso = marcar_paso(mapa, i, j);
                }
                break;
			case 'e':
            case 'q':
                if(!(paso == AGUA)) //LA CASILLA SELECCIONADA NO ES VALIDA COMO PUNTO INICIAL PARA COLOCAR UN BARCO
                {
                    printf("\nLA CASILLA [%i][%i] NO ES VALIDA PARA COLOCAR UN BARCO\n", i, j);
                    printf("\nPresione una tecla para continuar...\n");
					mygetch();
                    break;
                }

                //SELECCIONAR ORIENTACION
                if(tipo != 'L')
                {
                    if ((orientacion = seleccionar_orientacion()) == 'n')
                    {
                        printf("\nSELECCIONE NUEVAMENTE UNA CASILLA\n");
                        printf("\nPresione una tecla para continuar...\n");
						mygetch();
                        break;
                    }
                }

                //BORRAR EL SELECTOR DEL MAPA
                mapa[i][j] = paso;

                //PINTAR BARCO USANDO TIPO , ORIENTACION , i y j
                if(validar_barco(mapa, i, j, tipo, orientacion))
                {
		            switch(tipo)
					{
						case CLANCHA:
							printf("\nSE HA COLOCADO TU LANCHA EN: [%i][%i] \n",i,j);
							posicion_lancha(i,j);
							break;
						case CBUQUE:
							printf("\nSE HA COLOCADO TU BUQUE EN: [%i][%i] \n",i,j);
							posicion_buque(i,j,orientacion);
							break;
						case CPORTAAVIONES:
							printf("\nSE HA COLOCADO TU PORTAAVIONES EN: [%i][%i] \n",i,j);
							posicion_portaaviones(i,j,orientacion);
							break;	
					}
                    return;
                }
                else
                {
                    printf("\nNO SE PUDO COLOCAR EL BARCO EN [%i][%i]\n", i, j);
					imprimir_mapa(mapa);
					printf("\nPresione una tecla para continuar...\n");
					mygetch();
                }
                i = MAXI / 2;
                j = MAXJ / 2;
                opcion = ' ';
                orientacion = 'p';
                break;
            default:
                opcion = ' ';
                break;
        }
        limpiar();
        imprimir_mapa(mapa);
    }
}

void colocar_barcos(char mapa[MAXI][MAXJ])
{
    int portaaviones = 0;
    int lanchas = 0;
    int buques = 0;

    while(portaaviones < MPORTAAVIONES)
    {
        seleccionar_colocar(mapa, CPORTAAVIONES);
        portaaviones++;
        limpiar();
        imprimir_mapa(mapa);
    }
    while(buques < MBUQUE)
    {
        seleccionar_colocar(mapa, CBUQUE);
        buques++;
        limpiar();
        imprimir_mapa(mapa);
    }
    while(lanchas < MLANCHA)
    {
        seleccionar_colocar(mapa, CLANCHA);
        lanchas++;
        limpiar();
        imprimir_mapa(mapa);
    }
}


//BUFFER - SOCKET

void buffer_mapa(char buffer[TBUFFER], char mapa[MAXI][MAXJ])
{
    int i,j;
    for(i=0;i<MAXI;i++)
    {
        for(j=0;j<MAXJ;j++)
        {
            mapa[i][j] = buffer[(i*MAXI) + j];
        }
    }
}

void mapa_buffer(char mapa[MAXI][MAXJ], char buffer[TBUFFER])
{
    bzero(buffer,TBUFFER);
    int i,j,k;

    for(k=0, i=j=-1;k<TBUFFER;k++)
    {
        if(k == MAXI * MAXJ)
            break;
        if(k % MAXI == 0)
            i++;
        j = k % MAXJ;
        buffer[k] = mapa[i][j];
    }
}


