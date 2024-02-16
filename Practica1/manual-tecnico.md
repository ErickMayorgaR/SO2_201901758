# Manual Tecnico Practica 1

El proyecto consiste en desarrollar un programa en lenguaje C que monitorea las llamadas al sistema realizadas por procesos hijos creados por un proceso padre. El programa captura estas llamadas y las registra en un archivo de registro. Además, se cuenta el número total de llamadas de cada tipo (read, write, seek) y el número total de llamadas al sistema.


## Funcionalidades

        -Monitoreo de llamadas al sistema

        -Conteo de llamadas de lectura.

        -Conteo de llamadas de escritura.

        -Conteo de llamadas de buqueda


## Funciones y Comandos


        execl("./child.bin", "child.bin", NULL);


execl() es una función en C que se utiliza para ejecutar un programa en un proceso hijo. 



        signal();
Se utiliza para crear un nuevo proceso llamado proceso hijo. El proceso hijo es una copia exacta del proceso padre, pero con su propio espacio de memoria.



        fork();
Se utiliza para manejar señales en Unix/Linux. Las señales son eventos asincrónicos que pueden ser generados por el sistema operativo o por otros procesos.


        wait()
Se utiliza para que el proceso padre espere a que uno de sus procesos hijos termine de ejecutarse. 


        perror()
se utiliza para imprimir un mensaje de error que describe la última función de llamada que ha fallado, junto con una cadena que especifica la causa del error.


        exit()
se utiliza para terminar la ejecución de un proceso. Puede tomar un argumento opcional que especifica el código de salida del proceso