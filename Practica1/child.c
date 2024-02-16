#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>


volatile sig_atomic_t sign = 0;
int total = 0;
int c_read = 0;
int c_write = 0;
int seek = 0;

char random_char() {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t charset_size = sizeof(charset) - 1;
    return charset[rand() % charset_size];
}

void sigint_handler()
{
    printf("\nTerminando proceso hijo...\n");
    sign = -1;
    exit(0);
}

int main() {
    srand(time(NULL)); // Inicializar la semilla para números aleatorios

    int fd = open("practica1.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    while (!sign) {
        int random_num = rand() % 3;

        switch (random_num) {
            case 0: {
                
                char random_string[9]; 
                for (int j = 0; j < 8; ++j) {
                    random_string[j] = random_char(); // Generar un caracter aleatorio
                }
                random_string[8] = '\0'; 
                write(fd, random_string, strlen(random_string)); // Escribir en el archivo  
                write(fd, "\n", 1);           
                printf("\n");
                printf("Proceso hijo escribió: %s", random_string);
                printf("\n");
                break;
            }
            case 1: {
                char read_buffer[9]; 
                read(fd, read_buffer, 8); 
                read_buffer[8] = '\0'; 
                printf("\n");
                printf("Proceso hijo leyó: %s", read_buffer);
                printf("\n");
                break;
            }
            case 2: {
                lseek(fd, 0, SEEK_SET); 
                printf("\n");
                printf("Proceso hijo reposicionó el puntero");
                printf("\n");
                break;
            }
        }

        sleep(rand() % 3 + 1); // Esperar entre 1 y 3 segundos
    }

    close(fd);
    return 0;
}
