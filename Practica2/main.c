#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#define MAX_USERS 1000
#define MAX_OPERATIONS 1000
#define MAX_ERROR_MESSAGES 500
#define TIPO_ERROR_USUARIO 1
#define TIPO_ERROR_OPERACION 2

typedef struct
{
    int cuenta;
    char nombre[100];
    double saldo;
} Usuario;

typedef struct
{
    int tipoOperacion;
    int cuentaFrom; // se retira dinero para transferencia
    int cuentaTo;   // se deposita monto transferido
    double monto;
} Operacion;


typedef struct {
    const char* fileName;
    int startLine;
    int endLine;
    int hilo;
} ThreadArg;

Usuario usuarios[MAX_USERS];
char error_messages_operation[MAX_ERROR_MESSAGES][256];
char error_messages_users[MAX_ERROR_MESSAGES][256];
int contador_usuarios_cargados;

pthread_mutex_t mutex_usuarios = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_operaciones = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_error = PTHREAD_MUTEX_INITIALIZER;

// contadores
int usuariosPorHilo[3];
int usuariosTotal = 0;
int erroresUsuario = 0;
int erroresOperacion = 0;

void generarReporteEstadoCuentas() {
    FILE* estadoCuentaFile = fopen("EstadoCuentas.csv", "w");
    if (estadoCuentaFile != NULL) {
        fprintf(estadoCuentaFile, "no_cuenta,nombre,saldo\n"); // Encabezados del archivo CSV
        for (int i = 0; i < MAX_USERS; i++) {
            // Asegúrate de verificar si la cuenta es diferente de 0, lo que indica un usuario cargado
            if (usuarios[i].cuenta != 0) {
                fprintf(estadoCuentaFile, "%d,%s,%.2f\n", usuarios[i].cuenta, usuarios[i].nombre, usuarios[i].saldo);
            }
        }
        fclose(estadoCuentaFile);
        printf("Reporte de estado de cuentas generado correctamente en 'EstadoCuentas.csv'.\n");
    } else {
        printf("Error al abrir el archivo 'Usuarios_cargados.csv' para escribir el reporte.\n");
    }
}

void cargarOperaciones()
{
    pthread_t operation_threads[4];
}

void registrarError(const char *message, int line_number, int tipoError)
{
    pthread_mutex_lock(&mutex_error);

    // Determina el tipo de error y actúa en consecuencia
    if (tipoError == TIPO_ERROR_USUARIO && erroresUsuario < MAX_ERROR_MESSAGES)
    {
        sprintf(error_messages_users[erroresUsuario], "Linea #%d: %s", line_number, message);
        erroresUsuario++;
        // Nombre del archivo quemado para errores de usuario
        FILE *error_file = fopen("ErroresUsuario.csv", "a");
        if (error_file != NULL)
        {
            fprintf(error_file, "- Linea #%d: %s\n", line_number, message);
            fclose(error_file);
        }
    }
    else if (tipoError == TIPO_ERROR_OPERACION && erroresOperacion < MAX_ERROR_MESSAGES)
    {
        sprintf(error_messages_operation[erroresOperacion], "Linea #%d: %s", line_number, message);
        erroresOperacion++;
        // Nombre del archivo quemado para errores de operación
        FILE *error_file = fopen("ErroresOperacion.csv", "a");
        if (error_file != NULL)
        {
            fprintf(error_file, "- Linea #%d: %s\n", line_number, message);
            fclose(error_file);
        }
    }
    pthread_mutex_unlock(&mutex_error);
}

bool validate_account_number(const char *token)
{
    for (int i = 0; token[i] != '\0'; i++)
    {
        if (!isdigit(token[i]))
            return false;
    }
    return atoi(token) > 0;
}

bool validate_name(const char *token)
{
    if (token == NULL || strlen(token) == 0)
        return false;
    return true;
}

bool validate_balance(const char *token)
{
    char *end;
    double balance = strtod(token, &end);
    if (end == token || *end != '\0' || balance < 0)
        return false;
    return true;
}

bool validarRegistro(const char *line)
{
    char buffer[256];
    strcpy(buffer, line);

    // Eliminar posibles caracteres de fin de línea
    buffer[strcspn(buffer, "\r\n")] = '\0';
    //printf("Validando registro: %s\n", buffer);

    // Validar número de cuenta
    char *token = strtok(buffer, ",");
    if (!validate_account_number(token))
        return false;

    // Validar nombre
    token = strtok(NULL, ",");

    if (!validate_name(token))
        return false;

    // Validar saldo
    token = strtok(NULL, ",");

    if (!validate_balance(token))
        return false;

    // // Verificar si hay más datos después del saldo

    // if (strtok(NULL, ",") != NULL) return false;

    return true;
}

Usuario *encontrarUsuario(int cuenta)
{
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (usuarios[i].cuenta == cuenta)
        {
            return &usuarios[i];
        }
    }
    return NULL;
}

void *cargaUsuario(void *arg)
{
    ThreadArg* threadArg = (ThreadArg*)arg;
    const char* fileName = threadArg->fileName;
    int startLine = threadArg->startLine;
    int endLine = threadArg->endLine;
    int hilo = threadArg->hilo;

    printf("Nombre del archivo: %s\n", fileName);
    char rutaArchivo[256];
    sprintf(rutaArchivo, "Datos/%s", (char *)fileName);

    FILE *archivoUsuarios = fopen(rutaArchivo, "r");
    if (!archivoUsuarios)
    {
        registrarError("Imposible abrir archivo de usuarios", 0, 1);
        return NULL;
    }

    char buffer[256];
    int numeroLinea = 0, cargadosCorrectamente = 0;
    while (fgets(buffer, sizeof(buffer), archivoUsuarios) != NULL)
    {
        numeroLinea++;

        if (numeroLinea < startLine || numeroLinea > endLine) {
            continue;
        }
        char *registroDuplicado = strdup(buffer); // Duplicar línea para usar en caso de error
        if (!validarRegistro(buffer))
        {
            char mensajeError[512]; 
            snprintf(mensajeError, sizeof(mensajeError), "Registro inválido encontrado: %s", buffer);
            registrarError(mensajeError, numeroLinea, TIPO_ERROR_USUARIO);
            continue;
        }

        int cuenta;
        sscanf(strtok(buffer, ","), "%d", &cuenta); // Extraer número de cuenta

        pthread_mutex_lock(&mutex_usuarios);
        Usuario *usuario = encontrarUsuario(cuenta);

        if (usuario == NULL)
        {
            // Insertar el nuevo usuario si no hay duplicado
            for (int i = 0; i < MAX_USERS; i++)
            {
                if (usuarios[i].cuenta == 0)
                {
                    sscanf(registroDuplicado, "%d,%99[^,],%lf", &usuarios[i].cuenta, usuarios[i].nombre, &usuarios[i].saldo);
                    cargadosCorrectamente++;
                    printf("Usuario cargado: %s", registroDuplicado);
                    break;
                }
            }
        }
        else
        {
            // Manejar el caso de usuario duplicado
            registrarError("Usuario duplicado", numeroLinea, 1);
            printf("Error: usuario duplicado - %s", registroDuplicado);
        }
        pthread_mutex_unlock(&mutex_usuarios);
        free(registroDuplicado); // Asegurar liberación de memoria de la línea duplicada
    }

    fclose(archivoUsuarios);

    // Actualizar contadores globales y por hilo.
    pthread_mutex_lock(&mutex_usuarios);
    contador_usuarios_cargados += cargadosCorrectamente;
    // Asumiendo que 'parametro' puede convertirse o mapearse a un índice de hilo adecuado.
    usuariosPorHilo[hilo] += cargadosCorrectamente;
    pthread_mutex_unlock(&mutex_usuarios);

    printf("Proceso de carga finalizado. Usuarios cargados en este hilo: %d\n", cargadosCorrectamente);
    return NULL;
}

void cargarUsuarios()
{

    char infoInicio[256];
    time_t inicio = time(NULL);
    struct tm *tiempoInicio = localtime(&inicio);
    strftime(infoInicio, sizeof(infoInicio), "Inicio de la carga: %Y-%m-%d %H:%M:%S", tiempoInicio);
    registrarError(infoInicio, 0, TIPO_ERROR_USUARIO); 


    const char* fileName = "Datos/usuarios.csv";
    FILE *archivoUsuarios = fopen(fileName, "r");
    if (!archivoUsuarios) {
        printf("Imposible abrir archivo de usuarios, total lineas\n");
        return;
    }

    int totalLineas = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), archivoUsuarios) != NULL) {
        totalLineas++;
    }
    fclose(archivoUsuarios);

    pthread_t hilo_user[3];
    ThreadArg threadArgs[3];
    int lineasPorHilo = totalLineas / 3;
    int startLine = 1;
    for (int i = 0; i < 3; i++) {
        threadArgs[i].fileName = "usuarios.csv";
        threadArgs[i].startLine = startLine;
        threadArgs[i].endLine = (i == 2) ? totalLineas : startLine + lineasPorHilo - 1; // Ajuste para el último hilo
        threadArgs[i].hilo = i;
        pthread_create(&hilo_user[i], NULL, cargaUsuario, &threadArgs[i]);
        startLine += lineasPorHilo;
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(hilo_user[i], NULL);
    }

      // Registrar hora de finalización
    char infoFin[256];
    time_t fin = time(NULL);
    struct tm *tiempoFin = localtime(&fin);
    strftime(infoFin, sizeof(infoFin), "Fin de la carga: %Y-%m-%d %H:%M:%S", tiempoFin);
    registrarError(infoFin, 0, TIPO_ERROR_USUARIO); // Asumiendo el uso del log de usuario para información

    // Imprimir y registrar el total de usuarios cargados por cada hilo
    for (int i = 0; i < 3; i++) {
        char infoUsuariosPorHilo[256];
        snprintf(infoUsuariosPorHilo, sizeof(infoUsuariosPorHilo), "Total de usuarios cargados por el hilo %d: %d", i, usuariosPorHilo[i]);
        printf("%s\n", infoUsuariosPorHilo);
        registrarError(infoUsuariosPorHilo, 0, TIPO_ERROR_USUARIO); // Asumiendo el uso del log de usuario para información
    }

    generarReporteEstadoCuentas();
    
}

void mostrarMenuPrincipal()
{
    printf("Sistema Bancario\n");
    printf("1. Cargar Masiva Usuarios\n");
    printf("2. Carga Masiva Operaciones\n");
    printf("3. Salir\n");
    printf("Seleccione una opción: ");
}

int main()
{
    int opcion;
    do
    {
        mostrarMenuPrincipal();
        scanf("%d", &opcion);
        switch (opcion)
        {
        case 1:
            cargarUsuarios();
            break;
        case 2:
            cargarOperaciones();
            break;
        case 3:
            printf("Saliendo...\n");
            break;
        default:
            printf("Opción inválida. Por favor, intente nuevamente.\n");
        }
    } while (opcion != 4);

    return 0;
}
