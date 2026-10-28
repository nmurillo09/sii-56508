#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char **argv) {
    int fd;
    char buffer[1024];
int n;

    // borrar FIFO por si existiera previamente
    unlink("/tmp/fifo_logger");

    /* crear el FIFO */
    if (mkfifo("/tmp/fifo_logger", 0600)<0) {
        perror("No puede crearse el FIFO");
        return(1);
    }
    /* Abrir el FIFO */
    if ((fd=open("/tmp/fifo_logger", O_RDONLY))<0) {
        perror("No puede abrirse el FIFO");
        return(1);
    }
    /* Leer de la FIFO */
    while ((n=read(fd, buffer, sizeof(buffer)-1))>0) {
	// X5: Imprimir el texto en la pantalla
buffer[n]='\0';
printf("%s", buffer);
fflush(stdout);
    }
    close(fd);
    unlink("/tmp/fifo_logger");
    return(0);
}
