# Tenis – Sistemas Informáticos Industriales

Repositorio de prácticas de la asignatura **Sistemas Informáticos Industriales** (ETSIDI – UPM).

**Autor:** Nicolás Murillo · Matrícula 56508

## Descripción

Juego de tenis en 2D para dos jugadores que comparten teclado y pantalla, escrito en **C++** con **OpenGL** y **GLUT**. A lo largo de las 5 prácticas se irá ampliando con comunicación entre procesos y concurrencia.

## Controles

| Jugador | Subir | Bajar |
|---|---|---|
| Jugador 1 (izquierda) | `w` | `s` |
| Jugador 2 (derecha) | `o` | `l` |

Cuando la pelota llega al fondo del campo contrario, el jugador suma un punto.

## Requisitos

```bash
sudo apt install build-essential cmake freeglut3-dev git
```

## Compilación y ejecución

```bash
cd workspace
mkdir -p build && cd build
cmake ..
make
./logger      # y despues ./servidor, ./cliente y ./bot, cada uno en su terminal (ver Ejecucion)
```

## Estructura

```
workspace/
├── CMakeLists.txt   Reglas de compilación
├── include/         Cabeceras (.h)
└── src/             Código fuente (.cpp)
```

## Historial de prácticas

- **Práctica 1 (v1.1):** entorno de desarrollo (Git, CMake, Make). Movimiento de las raquetas y de la pelota.
- **Práctica 2 (v1.2):** comunicación entre procesos con una tubería con nombre (FIFO).
- **Práctica 3 (v1.3):** comunicación mediante fichero proyectado en memoria (mmap). Nuevo programa `bot` que controla automáticamente la raqueta 1 a partir de las posiciones de la pelota y la raqueta compartidas con el juego.
- **Práctica 4 (v1.4):** el juego se divide en `servidor` (física, puntos y logger) y `cliente` (dibujo, teclado y bot), comunicados por dos FIFO. El servidor usa un hilo (pthread) para recibir las teclas sin bloquear el juego. Orden de arranque: logger → servidor → cliente → bot.


## Ejecución (desde la práctica 4)

Hay que lanzar cuatro programas, cada uno en su terminal y en este orden, porque cada uno necesita algo que crea el anterior:

    # Terminal 1: el logger crea la FIFO /tmp/fifo_logger
    ./logger
    # Terminal 2: el servidor crea la FIFO servidor-cliente (su ventana aparece al lanzar el cliente)
    ./servidor
    # Terminal 3: el cliente crea la FIFO cliente-servidor y el fichero /tmp/datos_bot
    ./cliente
    # Terminal 4: el bot usa /tmp/datos_bot
    ./bot

Se juega desde la ventana del cliente: el jugador 2 con `o` y `l`; el jugador 1 lo controla el bot. A los 3 puntos terminan los cuatro programas.
