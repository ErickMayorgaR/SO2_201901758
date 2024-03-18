# Manual Técnico del Proyecto

## Descripción General

Este proyecto está diseñado para gestionar la carga de usuarios y operaciones bancarias desde archivos CSV, registrar cualquier error durante el proceso y generar un estado de cuentas de todos los usuarios cargados. Utiliza la programación multihilo para mejorar la eficiencia de la carga de datos.

## Estructuras de Datos Principales

### Usuario

Representa un usuario bancario con los siguientes campos:

- `int cuenta`: Número de cuenta del usuario.
- `char nombre[100]`: Nombre del usuario.
- `double saldo`: Saldo de la cuenta del usuario.

### Operacion

Representa una operación bancaria, incluyendo transferencias entre cuentas, con los siguientes campos:

- `int tipoOperacion`: Tipo de operación realizada.
- `int cuentaFrom`: Número de cuenta de origen en una transferencia.
- `int cuentaTo`: Número de cuenta de destino en una transferencia.
- `double monto`: Monto de la operación.

### ThreadArg

Utilizado para pasar argumentos a los hilos responsables de la carga de usuarios, con los siguientes campos:

- `const char* fileName`: Nombre del archivo a procesar.
- `int startLine`: Línea inicial del archivo a procesar por el hilo.
- `int endLine`: Línea final del archivo a procesar por el hilo.
- `int hilo`: Identificador del hilo.

## Funciones Principales

### cargarUsuarios()

Responsable de la carga de usuarios desde un archivo CSV. Divide el trabajo entre varios hilos para mejorar la eficiencia.

### registrarError()

Registra errores encontrados durante la carga de usuarios y operaciones en archivos de log específicos para su revisión.

### generarReporteEstadoCuentas()

Genera un reporte en formato CSV del estado de cuenta de todos los usuarios cargados en el sistema, incluyendo el número de cuenta, nombre y saldo.

### validarRegistro()

Valida la integridad de cada registro de usuario leído del archivo CSV, asegurando que los datos sean correctos antes de su carga.

## Multithreading

El proyecto utiliza programación multihilo para mejorar la eficiencia en la carga de datos y el procesamiento de operaciones. Se divide el archivo de entrada en segmentos que se procesan en paralelo por diferentes hilos, reduciendo el tiempo total de procesamiento.

## Gestión de Errores

Todos los errores detectados durante la carga de datos y el procesamiento de operaciones se registran en archivos de log para su posterior análisis y corrección. Esto incluye registros inválidos y operaciones fallidas.

## Conclusión

Este manual técnico proporciona una visión general del proyecto, describiendo sus principales componentes, funcionalidades y el enfoque utilizado para la gestión de datos y errores. El proyecto demuestra el uso efectivo de la programación multihilo y las prácticas de registro de errores para gestionar la carga y el procesamiento de datos en un contexto bancario.
