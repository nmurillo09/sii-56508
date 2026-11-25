// MundoServidor.cpp: implementacion de CMundo para el SERVIDOR
//////////////////////////////////////////////////////////////////////
#include <fstream>
#include "MundoServidor.h"
#include "glut.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

#define FIFO_SC "/tmp/fifo_servidor_cliente"	// servidor -> cliente
#define FIFO_CS "/tmp/fifo_cliente_servidor"	// cliente -> servidor

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMundo::CMundo()
{
	Init();
}

CMundo::~CMundo()
{
	close(fd_fifo);		// logger
	close(fd_sc);		// coordenadas hacia el cliente
	unlink(FIFO_SC);	// la creo yo, la borro yo
	close(fd_cs);		// teclas del cliente
}

void CMundo::InitGL()
{
	//Habilitamos las luces, la renderizacion y el color de los materiales
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	
	glMatrixMode(GL_PROJECTION);
	gluPerspective( 40.0, 800/600.0f, 0.1, 150);
}

void print(char *mensaje, int x, int y, float r, float g, float b)
{
	glDisable (GL_LIGHTING);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT) );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glColor3f(r,g,b);
	glRasterPos3f(x, glutGet(GLUT_WINDOW_HEIGHT)-18-y, 0);
	int len = strlen (mensaje );
	for (int i = 0; i < len; i++) 
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, mensaje[i] );
		
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable( GL_DEPTH_TEST );
}
void CMundo::OnDraw()
{
	//Borrado de la pantalla	
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Para definir el punto de vista
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	
	gluLookAt(0.0, 0, 17,  // posicion del ojo
		0.0, 0.0, 0.0,      // hacia que punto mira  (0,0,0) 
		0.0, 1.0, 0.0);      // definimos hacia arriba (eje Y)    

	/////////////////
	///////////
	//		AQUI EMPIEZA MI DIBUJO
	char cad[100];
	sprintf(cad,"Jugador1: %d",puntos1);
	print(cad,10,0,1,1,1);
	sprintf(cad,"Jugador2: %d",puntos2);
	print(cad,650,0,1,1,1);
	int i;
	for(i=0;i<paredes.size();i++)
		paredes[i].Dibuja();

	fondo_izq.Dibuja();
	fondo_dcho.Dibuja();
	jugador1.Dibuja();
	jugador2.Dibuja();
	esfera.Dibuja();

	/////////////////
	///////////
	//		AQUI TERMINA MI DIBUJO
	////////////////////////////

	//Al final, cambiar el buffer
	glutSwapBuffers();
}

void CMundo::OnTimer(int value)
{	
	jugador1.Mueve(0.025f);
	jugador2.Mueve(0.025f);
	esfera.Mueve(0.025f);
	int i;
	for(i=0;i<paredes.size();i++)
	{
		paredes[i].Rebota(esfera);
		paredes[i].Rebota(jugador1);
		paredes[i].Rebota(jugador2);
	}

	jugador1.Rebota(esfera);
	jugador2.Rebota(esfera);
	if(fondo_izq.Rebota(esfera))
	{
		esfera.centro.x=0;
		esfera.centro.y=rand()/(float)RAND_MAX;
		esfera.velocidad.x=2+2*rand()/(float)RAND_MAX;
		esfera.velocidad.y=2+2*rand()/(float)RAND_MAX;
		puntos2++;
char mensaje[100];
sprintf(mensaje, "Jugador 2 marca 1 punto, lleva un total de %d puntos.\n", puntos2);
write(fd_fifo, mensaje, strlen(mensaje));

	}

	if(fondo_dcho.Rebota(esfera))
	{
		esfera.centro.x=0;
		esfera.centro.y=rand()/(float)RAND_MAX;
		esfera.velocidad.x=-2-2*rand()/(float)RAND_MAX;
		esfera.velocidad.y=-2-2*rand()/(float)RAND_MAX;
		puntos1++;
char mensaje[100];
sprintf(mensaje,"Jugador 1 marca 1 punto, lleva un total de %d puntos\n", puntos1);
write(fd_fifo, mensaje, strlen(mensaje));
	}

	// Enviar las coordenadas al cliente (al final del OnTimer)
	char cad[200];
	sprintf(cad,"%f %f %f %f %f %f %f %f %f %f %d %d",
		esfera.centro.x, esfera.centro.y,
		jugador1.x1, jugador1.y1, jugador1.x2, jugador1.y2,
		jugador2.x1, jugador2.y1, jugador2.x2, jugador2.y2,
		puntos1, puntos2);
	write(fd_sc, cad, sizeof(cad));

if(puntos1>=3 || puntos2>=3)
{
char fin[100];
sprintf(fin, "Fin de la partida: gana el jugador %d\n", puntos1>=3 ? 1:2);
write(fd_fifo, fin, strlen(fin));
exit(0);
}
}

void CMundo::OnKeyboardDown(unsigned char key, int x, int y)
{
	switch(key)
	{
//	case 'a':jugador1.velocidad.x=-1;break;
//	case 'd':jugador1.velocidad.x=1;break;
	case 's':jugador1.velocidad.y=-4;break;
	case 'w':jugador1.velocidad.y=4;break;
	case 'l':jugador2.velocidad.y=-4;break;
	case 'o':jugador2.velocidad.y=4;break;

	}
}

// Funcion que ejecuta el hilo: llama al metodo del objeto
void* hilo_comandos(void* d)
{
	CMundo* p=(CMundo*) d;
	p->RecibeComandosJugador();
	return NULL;
}

// Bucle del hilo: espera las teclas del cliente y mueve las raquetas
void CMundo::RecibeComandosJugador()
{
	while (1) {
		usleep(10);
		char cad[100];
		int n = read(fd_cs, cad, sizeof(cad));
		if (n <= 0)		// el cliente ha cerrado la tuberia
			break;
		unsigned char key;
		sscanf(cad,"%c",&key);
		if(key=='s')jugador1.velocidad.y=-4;
		if(key=='w')jugador1.velocidad.y=4;
		if(key=='l')jugador2.velocidad.y=-4;
		if(key=='o')jugador2.velocidad.y=4;
		if(key=='x')jugador1.velocidad.y=0;	// parar (lo envia el bot)
	}
}

void CMundo::Init()
{
	// FIFO con el logger (practica 2)
	fd_fifo=open("/tmp/fifo_logger", O_WRONLY);
	if(fd_fifo<0)
		perror("No puede abrirse el FIFO (¿has lanzado el logger?)");

	// FIFO servidor -> cliente: la crea el servidor
	unlink(FIFO_SC);
	if (mkfifo(FIFO_SC, 0600) < 0)
		perror("No puede crearse la FIFO servidor-cliente");
	fd_sc = open(FIFO_SC, O_WRONLY);	// espera a que el cliente la abra
	if (fd_sc < 0)
		perror("No puede abrirse la FIFO servidor-cliente");

	// FIFO cliente -> servidor: la crea el cliente
	fd_cs = open(FIFO_CS, O_RDONLY);
	if (fd_cs < 0)
		perror("No puede abrirse la FIFO cliente-servidor");

	puntos1=0;
	puntos2=0;
	Plano p;
//pared inferior
	p.x1=-7;p.y1=-5;
	p.x2=7;p.y2=-5;
	paredes.push_back(p);

//superior
	p.x1=-7;p.y1=5;
	p.x2=7;p.y2=5;
	paredes.push_back(p);

	fondo_izq.r=0;
	fondo_izq.x1=-7;fondo_izq.y1=-5;
	fondo_izq.x2=-7;fondo_izq.y2=5;

	fondo_dcho.r=0;
	fondo_dcho.x1=7;fondo_dcho.y1=-5;
	fondo_dcho.x2=7;fondo_dcho.y2=5;

	//a la izq
	jugador1.g=0;
	jugador1.x1=-6;jugador1.y1=-1;
	jugador1.x2=-6;jugador1.y2=1;

	//a la dcha
	jugador2.g=0;
	jugador2.x1=6;jugador2.y1=-1;
	jugador2.x2=6;jugador2.y2=1;

	// Hilo que recibe las teclas del cliente
	pthread_create(&thid1, NULL, hilo_comandos, this);
}
