# EDAversi

## Integrantes del grupo y contribución al trabajo de cada integrante

* [Nombre]: [contribución]

[completar]

## Parte 1: Generación de movimientos válidos y algoritmo de jugada

[Enumera aquí las pruebas que hiciste para validar que tu generador de movimientos válidos y algoritmo de jugada funcionan correctamente.]
* Cambiamos el color de las posibles movidas del jugador humano para poder corroborar que son validas
*

## Parte 2: Implementación del motor de IA


## Parte 3: Poda del árbol
En la parte 2 implementamos minimax sin limitar la profundidad de búsqueda. Como consecuencia, la función init_tree intenta construir un árbol de estados completo y minimax recorre todos sus nodos.
Al ejecutar el juego, cuando es el turno de la IA, aparece el mensaje “EDA Versi is not responding” y hay que forzar el cierre. El bloqueo se debe a que el árbol de juego crece exponencialmente: en Reversi el factor de ramificación es significativo y la cantidad de estados posibles es astronómica, por lo que no es computable generar y evaluar todos los nodos en tiempo razonable. En una partida tipica de REVERSI tendriamos 10e58 posibles movidas por lo que tendria que generar 10e58 nodos. 

Conclusión: construir el árbol completo no es viable. Es necesario acotar la búsqueda y generar nodos bajo demanda durante la recursión (no preconstruir todo con init_tree).
[Justifica por qué el algoritmo minimax de la parte anterior no se completa. Consejo: determina la complejidad computacional.]

## Documentación adicional

[Aquí.]

## Bonus points

[Aquí.]
