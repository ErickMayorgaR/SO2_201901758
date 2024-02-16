#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

char random_char() {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t charset_size = sizeof(charset) - 1;
    return charset[rand() % charset_size];
}

int main() {
    srand(time(NULL)); // Inicializar la semilla para números aleatorios

    int fd = open("practica1.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    // Ciclo para simular llamadas de sistema
    for (int i = 0; i < 10; ++i) {
        int random_num = rand() % 3; // Generar un número aleatorio entre 0 y 2

        // Realizar la llamada de sistema correspondiente según el número aleatorio
        switch (random_num) {
            case 0: {
                char random_string[9]; // Buffer para la cadena aleatoria
                for (int j = 0; j < 8; ++j) {
                    random_string[j] = random_char(); // Generar un caracter aleatorio
                }
                random_string[8] = '\0'; // Terminador de cadena
                write(fd, random_string, strlen(random_string)); // Escribir en el archivo
                printf("Proceso hijo escribió: %s\n", random_string);
                break;
            }
            case 1: {
                char read_buffer[9]; // Buffer para la lectura
                read(fd, read_buffer, 8); // Leer del archivo
                read_buffer[8] = '\0'; // Terminador de cadena
                printf("Proceso hijo leyó: %s\n", read_buffer);
                break;
            }
            case 2: {
                lseek(fd, 0, SEEK_SET); // Reposicionar el puntero de lectura/escritura al principio del archivo
                printf("Proceso hijo reposicionó el puntero\n");
                break;
            }
        }

        sleep(rand() % 3 + 1); // Esperar entre 1 y 3 segundos
    }

    // Cerrar el archivo
    close(fd);

    return 0;
}
