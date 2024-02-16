#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

pid_t pid1, pid2, m_pid;

// Función para manejar la señal SIGINT (Ctrl + C)
void sigint_handler()
{
    printf("\nTerminando proceso padre...\n");

    FILE *archivo;
    char linea[100];

    int total_read = 0;
    int total_write = 0;
    int total_seek = 0;
    int total_llamadas = 0;

    archivo = fopen("./syscalls.log", "r");
    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }

    while (fgets(linea, sizeof(linea), archivo))
    {
        total_llamadas++;

        if (strstr(linea, "read"))
        {
            total_read++;
        }
        else if (strstr(linea, "write"))
        {
            total_write++;
        }
        else if (strstr(linea, "seek"))
        {
            total_seek++;
        }
    }

    // Imprime el resultado
    printf("Total de llamadas de read: %d\n", total_read);
    printf("Total de llamadas de write: %d\n", total_write);
    printf("Total de llamadas de seek: %d\n", total_seek);
    printf("Total de llamadas al sistema: %d\n", total_llamadas);
    fclose(archivo);

    kill(pid1, SIGINT);
    kill(pid2, SIGINT);
    kill(m_pid, SIGINT);
    exit(0);
}


int main()
{
    signal(SIGINT, sigint_handler);

    pid1 = fork();
    if (pid1 == 0)
    {
        execl("./child.bin", "child.bin", NULL);
        exit(0);
    }
    else
    {
        pid2 = fork();
        if (pid2 == 0)
        {
            execl("./child.bin", "child.bin", NULL);
            exit(0);
        }
        else
        {
            m_pid = fork();
            if (m_pid == 0)
            {
                char command[100];
                sprintf(command, "%s %d %d %s", "sudo stap monitor.stp", pid1, pid2, "> syscalls.log");
                system(command);
                exit(0);
            }
            else
            {
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
                waitpid(m_pid, NULL, 0);
            }
        }
    }
    return 0;
}
