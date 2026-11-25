// MundoServidor.h: clase CMundo del SERVIDOR
// (mueve la pelota y las raquetas, cuenta los puntos y avisa al logger)
//////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include <pthread.h>
#include "Plano.h"
#include "Esfera.h"
#include "Raqueta.h"

class CMundo  
{
public:
	void Init();
	CMundo();
	virtual ~CMundo();	
	
	void InitGL();	
	void OnKeyboardDown(unsigned char key, int x, int y);
	void OnTimer(int value);
	void OnDraw();	

	Esfera esfera;
	std::vector<Plano> paredes;
	Plano fondo_izq;
	Plano fondo_dcho;
	Raqueta jugador1;
	Raqueta jugador2;

	int puntos1;
	int puntos2;

	int fd_fifo;		// FIFO hacia el logger (practica 2)
	int fd_sc;		// FIFO servidor -> cliente (coordenadas)
	int fd_cs;		// FIFO cliente -> servidor (teclas)
	pthread_t thid1;	// hilo que recibe las teclas del cliente

	void RecibeComandosJugador();	// lo ejecuta el hilo
};
