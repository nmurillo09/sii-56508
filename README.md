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
./tenis
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
## Ejecución con el logger (desde la práctica 2)
El juego envía cada punto al programa 'logger' através de la FIFO '/tmp/fifo_logger'. El logger crea la FIFO, así que debe lanzarse **antes** que el 
juego, en otro terminal:
    # Terminal 1
    ./logger
    # Terminal 2
    ./tenis
