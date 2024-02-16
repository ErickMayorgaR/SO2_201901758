#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// Función para manejar la señal SIGINT (Ctrl + C)
void sigint_handler(int signum)
{
    printf("\nTerminando proceso padre...\n");
    // Aquí puedes imprimir el número total de llamadas al sistema que realizaron los procesos hijo
    // También puedes imprimir el número de llamadas al sistema por tipo
    exit(0);
}

int main()
{
    signal(SIGINT, sigint_handler);

    pid_t pid1, pid2;

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
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("Terminando proceso padre...\n");

    return 0;
}
