# EDAversi

## Integrantes del grupo y contribución al trabajo de cada integrante

* MICAELA DINSEN: Desarrollo del motor de reglas y la mecánica central del juego Reversi, asegurando que el motor del juego se comporte de acuerdo al reglamento oficial.

* AGUSTIN MONTOTO: Desarrollo del motor de reglas y mecanica central del juego Reversi, asegurando que el motor del juego se comporte de acuerdo al reglamento oficial. Implementacion del modelo de ia alpha-beta pruning. Implementacion de los openings y mejores jugadas del inicio. 

* LEILA CASAIS:

* MARIA SOL VIGILANTE:

## Parte 1: Generación de movimientos válidos y algoritmo de jugada

[Enumera aquí las pruebas que hiciste para validar que tu generador de movimientos válidos y algoritmo de jugada funcionan correctamente.]
* Cambiamos el color de las posibles movidas del jugador humano para poder corroborar que son validas

* Para la validacion de movimientos validos, empezamos a recorrer el tablero y salteamos las casillas que esten ocupadas. 

* Cuando una casilla esta libre, recorre en las 8 direcciones (Horizontal, Vertical y Diagonal)

* Si se encuentran fichas enemigas en el medio y al final no hay ni una ficha nuentra ni se sale del tablero, guardamos el movimiento como valido

## Parte 2: Implementación del motor de IA
Para la parte de la implementacion del motor de IA, dividimos el "trabajo" en 3 partes. Opening, Mid-Game & Ending. 

Para la parte del Opening, Buscamos bases de datos de los mejores opening y jugadas que se suelen repetir, ya que al principio los movimientos suelen ser muy parecidos. Con esto, logramos hacer que no recorrer tantos nodos ya que al principio es donde mas hijos hay.

Para la parte de Mid-Game 

Parte la parte Ending, una vez quedan 8 casillas libres restantes (o menos), utilizamos fuerza bruta para predecir todos los posibles finales ya que altura del arbol se reducio de gran manera con respecto a las otras etapas. Por lo tanto, siempre evaluamos todos los posibles resultados con respecto a los movimientos propios y del rival, y en base a eso ses juega el proximo "mejor" movimiento



## Parte 3: Poda del árbol
En la parte 2 implementamos minimax sin limitar la profundidad de búsqueda. Como consecuencia, la función init_tree intenta construir un árbol de estados completo y minimax recorre todos sus nodos.
Al ejecutar el juego, cuando es el turno de la IA, aparece el mensaje “EDA Versi is not responding” y hay que forzar el cierre. El bloqueo se debe a que el árbol de juego crece exponencialmente: en Reversi el factor de ramificación es significativo y la cantidad de estados posibles es astronómica, por lo que no es computable generar y evaluar todos los nodos en tiempo razonable. En una partida tipica de REVERSI tendriamos 10e58 posibles movidas por lo que tendria que generar 10e58 nodos. 

Conclusión: construir el árbol completo no es viable. Es necesario acotar la búsqueda y generar nodos bajo demanda durante la recursión (no preconstruir todo con init_tree).
[Justifica por qué el algoritmo minimax de la parte anterior no se completa. Consejo: determina la complejidad computacional.]

## Documentación adicional

Base de datos para la apertura: https://samsoft.org.uk/reversi/openings.htm

## Bonus points

Comparamos las diferentes ias con diferentes algoritmos y estrategias de juego, haciendolas juagar entre si. Esta estrategia nos ayudo para decidir con cual estrategia quedarnos.

