// MundoCliente.cpp: implementacion de CMundo para el CLIENTE
//////////////////////////////////////////////////////////////////////
#include <fstream>
#include "MundoCliente.h"
#include "glut.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>

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
	close(fd_sc);		// coordenadas del servidor
	close(fd_cs);		// teclas hacia el servidor
	unlink(FIFO_CS);	// la creo yo, la borro yo

	pdatos->fin=1;		// avisar al bot (practica 3)
	munmap(pdatos, sizeof(DatosMemCompartida));
	unlink("/tmp/datos_bot");
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
	// Recibir las coordenadas del servidor.
	// Se leen TODOS los mensajes pendientes y se usa solo el ultimo,
	// para que el cliente no se quede atrasado respecto al servidor.
	char cad[200], ultimo[200];
	int n, recibido = 0;
	while ((n = read(fd_sc, cad, sizeof(cad))) > 0)
	{
		memcpy(ultimo, cad, sizeof(cad));
		recibido = 1;
	}
	if (n == 0)		// el servidor ha cerrado la tuberia: fin del juego
	{
		printf("El servidor ha terminado.\n");
		exit(0);
	}
	if (!recibido)		// todavia no ha llegado nada nuevo
		return;
	sscanf(ultimo,"%f %f %f %f %f %f %f %f %f %f %d %d",
		&esfera.centro.x, &esfera.centro.y,
		&jugador1.x1, &jugador1.y1, &jugador1.x2, &jugador1.y2,
		&jugador2.x1, &jugador2.y1, &jugador2.x2, &jugador2.y2,
		&puntos1, &puntos2);

	// Comunicacion con el bot (practica 3): ahora la accion se envia al servidor
	pdatos->esfera = esfera;
	pdatos->raqueta1 = jugador1;
	if (pdatos->accion == 1)
		OnKeyboardDown('w', 0, 0);
	else if (pdatos->accion == -1)
		OnKeyboardDown('s', 0, 0);
	else
		OnKeyboardDown('x', 0, 0);	// parar
}

void CMundo::OnKeyboardDown(unsigned char key, int x, int y)
{
	// Enviar la tecla pulsada al servidor
	char cad[100];
	sprintf(cad, "%c", key);
	write(fd_cs, cad, sizeof(cad));
}

void CMundo::Init()
{
	signal(SIGPIPE, SIG_IGN);	// si el servidor ya no esta, write falla sin matar al cliente

	// FIFO cliente -> servidor: la crea el cliente
	unlink(FIFO_CS);
	if (mkfifo(FIFO_CS, 0600) < 0)
		perror("No puede crearse la FIFO cliente-servidor");

	// FIFO servidor -> cliente: la ha creado el servidor
	fd_sc = open(FIFO_SC, O_RDONLY);
	if (fd_sc < 0)
		perror("No puede abrirse la FIFO servidor-cliente (¿has lanzado el servidor?)");
	fcntl(fd_sc, F_SETFL, O_NONBLOCK);	// read no se bloquea si no hay datos

	fd_cs = open(FIFO_CS, O_WRONLY);
	if (fd_cs < 0)
		perror("No puede abrirse la FIFO cliente-servidor");

	// Memoria compartida con el bot (practica 3)
	int fd_mem = open("/tmp/datos_bot", O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (fd_mem < 0)
		perror("No puede crearse el fichero compartido");
	ftruncate(fd_mem, sizeof(DatosMemCompartida));
	pdatos = (DatosMemCompartida*)mmap(NULL, sizeof(DatosMemCompartida),
	                PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, 0);
	if (pdatos == MAP_FAILED)
		perror("Error en la proyeccion mmap");
	close(fd_mem);
	pdatos->accion = 0;
	pdatos->fin = 0;

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
}
