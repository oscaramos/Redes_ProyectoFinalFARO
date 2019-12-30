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

# Historia
Oscar hizo todo.
Haha, noo. A ver... lo que realmente paso fue:

## Tareas
Al inicio, yo estaba encargado de Master. Ruben y Alonso encargados de los Slaves. Fabricio de las pruebas y la documentación.

En la fase de análisis e ideas todos colaboramos para hacer un protocolo robusto.

## Problema
Pocos dias antes de la presentación ya estaba hecho el Master pero seguía sin terminar los Slaves. Sabía que su código se había convertido en un *Bloater*(https://sourcemaking.com/refactoring/smells/bloaters), se había vuelto un codigo demasiado grande, poco estructurado(solo funciones), titánico lo diría. Hicieron un buen trabajo, eso sí, pero tenían este problema muy marcado.

## Solución
Así que tomé su trabajo, y lo reestructure completamente con técnicas de refactoring. El código... ya estaba totalmente funcional, solamente le faltaba reestructurarlo para mejorar legibilidad y además agregarle el protocolo interno de los slaves.

Sin embargo, hacer todo esto toma muchisimo tiempo y esfuezo para lograrlo, en circunstancias normales hubiera fallado... 

Para lograr terminarlo mi equipo conseguía tiempo valioso mediante la postergación de la presentación final por unos dias(pasamos de presentar el Viernes al Lunes). Sin este tiempillo extra nuestro trabajo hubiera sido un total desastre. 

Tambien me cubrian en otros cursos mientras codeaba en clase(cof cof Compiladores) y me ayudaban a estudiar examenes de los cursos que llevabamos. Les debo muchas gracias por todo esto.

## Presentación
En la presentación la metodología fue la siguiente: 
### Fase 1 (documento)
El profe pedía código de x funcionalidad dentro del documento, señalaba su ubicación dentro del diagrama de clases, buscabamos la clase y la función y la línea de código exacta que pedía, lo encontrabamos y les explicabamos al profe el cómo funciona ese segmento de código.

### Fase 2 (pruebas)
La siguiente fase consiste en hacer la prueba del código: encender nuestras máquinas(mientras más... mejor), hacerle el comando ping entre ellos para verificar que se conecten, correr los programas, insertar los comandos del ejemplo del profe, y mostrarle las salidas al profe. No tenía la parte de que slave muere y sigue corriendo el sistema, sin embargo como arte de mágia funcionó... jajaja.

# Resultados
* En el código conseguímos A++, debído al buen algoritmo de búsqueda mostrada durante la presentación. 
* En el documento conseguímos A+, debído al contenido del documento al tener diagramas de todo tipo: casos de uso, protocolo, secuencia, clases y pruebas.
* En las pruebas conseguimos A+, no fue A++ debído a la carencia de la funcionalidad del "slave muere y sistema sigue funcionando como si nunca murio". El resto de funcionalidades sí estaban presentes, como el KeepAlive y todas las queries.  

# Conclusiones
Sin duda fué un trabajo de software enoorme. Hubo buenos resultados pero me demoré demasiado al tomar la decisión de encargarme del código. También debí enseñarles mi framework para que pudiesen implementar sin el problema del Bloater. 

# Trabajos futuros
Tú!!! has de deber continuar este proyecto, pls. Sólo le falta la función de la muerte de slaves.

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
