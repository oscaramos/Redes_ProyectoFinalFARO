# Redes Proyecto Final Faro 2019-2
Sigla FARO: FabricioAlonsoRubenOscar
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
* g++ (c++17)
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

# Presentación
En la presentación la metodología fue la siguiente: 

## Fase 1 (documento)
El profe pedía código de x funcionalidad dentro del documento, señalaba su ubicación dentro del diagrama de clases, buscabamos la clase y la función y la línea de código exacta que pedía, lo encontrabamos y les explicabamos al profe el cómo funciona ese segmento de código.

## Fase 2 (pruebas)
La siguiente fase consiste en hacer la prueba del código: encender nuestras máquinas, hacerle el comando ping entre ellos para verificar que se conecten, correr los programas, insertar los comandos del ejemplo del profe, y mostrarle las salidas al profe. No teníamos la parte de que slave muere y sigue corriendo el sistema, sin embargo como arte de mágia funcionó... jajaja.

# Resultados
* En el código conseguímos A++, debído a la buena organización de equipo al presentar el código. 
* En el documento conseguímos A+, debído al contenido del documento al tener diagramas de todo tipo: casos de uso, protocolo, secuencia, clases y pruebas, no sé porque no sacamos A++.
* En las pruebas conseguimos A+, no fue A++ debído a la carencia de la funcionalidad del "slave muere y sistema sigue funcionando como si nunca murio". El resto de funcionalidades sí estaban presentes, como el KeepAlive y todas las queries.

# Conclusiones
Sin duda fué un trabajo de software enoorme. Hubo buenos resultados pero me demoré demasiado al tomar la decisión de encargarme del código. También debí enseñarles mi framework para que pudiesen implementar sin el problema del Bloater. 

# Trabajos futuros
Tú!!! has de continuar este proyecto. Sólo le falta la función de la muerte de slaves.
