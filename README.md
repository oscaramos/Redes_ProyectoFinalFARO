# Redes Proyecto Final Faro 2019-2
FARO: FabricioAlonsoRubenOscar
# Integrantes
* Oscar Daniel Ramos Ramirez
* Alonso Valdivia Quispe
* Fabricio Flores Pari
* Ruben Cuba Lajo


# Descripción
Una **base de datos distribuida** con redundancia con el modelo Master-Slaves basada en grafos.
Este sistema posee las siguientes funcionalidades:
*	Crear nodo.
*	Linkear nodos.
*	Borrar nodos.
*	Deslinkear nodos.
*	Actualizar el identificador del nodo.
*	Explorar nodos a partir de un nodo inicial.
*	Explorar nodos a partir de un nodo inicial mostrando sus atributos.

# Herramientas
* g++
* make
* terminator

# Manual de Usuario
## Master
1. Ejecutar el **Master** con "./master.exe <PORT_SERVER>"

## Slaves
2. Ejecutar cada **Slave** con "./slave.exe <PORT_SRVMASTER> <PORT_SRVSLAVES> [<SLAVE_ID>]"

<PORT_SRVMASTER>: Puerto que conecta master y slaves.

<PORT_SRVSLAVES>: Puerto que conecta slaves y slaves.

[<SLAVE_ID>]: es opcional, es para volver a reutilizar <PORT_SRVSLAVES> sobre la misma máquina.

## Client
3. Ejecutar "./client.exe <IP_SERVER> <PORT_SERVER>"

El server que apunta es el **Master**

# Ejemplo
* Este ejemplo es cómo se ejecuta en una máquina, no en varias.
* Seguir este orden: Master, Cliente, Slave 1 y 2, Cliente.
* '$' es la terminal de ubuntu(usamos la 18.04)
Terminal **Master**
```
$./master.exe 1111
Servidor Abierto!
```

Terminal **Cliente**
```
$./client.exe 127.0.0.1 1111
Cliente: conectado a servidor 127.0.0.1 | 1111
>_
```

Terminal **Slave 1**
```
$./slave.exe 8888 5555 0
Servidor Abierto!
Servidor Abierto!
```

Terminal **Slave 2**
```
$./slave.exe 9999 5555 1
Servidor Abierto!
Servidor Abierto!
```

Terminal **Cliente**
```
>CONNECT 127.0.0.1 8888
[THE SLAVE WAS CONNECTED]
>CONNECT 127.0.0.1 9999
[THE SLAVE WAS CONNECTED]
>START
[THE SYSTEM IS UP]
...
```

# Historia
Oscar hizo todo

# Preguntas frecuentes
**Pregunta**: Y los comandos?

**Respuesta**: Lee la doc

**Pregunta**: Y la info?

**Respuesta**: Lee la doc

**Pregunta**: Y las pruebas?

**Respuesta**: Lee la doc

**Pregunta**: Y l...?

**Respuesta**: Lee la doc

doc = documentación
