#### UNIVERSIDAD DE LOS ANDES

#### Facultad de Ingeniería 
#### y Ciencias Aplicadas

## Sistemas Operativos y Redes 2018

## Tarea 1 - Parte 1

#### Hugo de la Fuente - María Pía Sánchez

___________________________________________________________________________________________________________________________
### 1. Problemas encontrados en la implementación.
Los principales problemas que tuvimos en la tarea se pueden apreciar en los archivos prueba_quicksort.c y pqsort.c. Uno de ellos fue la sincronización de los threads. Cuando tratábamos de leer las posiciones del arreglo correspondientes a cada thread, se podía observar que éstos no siempre eran los esperados. Es decir, si el thread P_i tenía que leer los primeros k elementos del arreglo, a veces leía números distintos a los que tenía el subarreglo correspondiente, o simplemente no los leía. Esto implica que las secciones críticas no habían sido identificadas. Para solucionarlo se debía implementar un mutex, sin embargo no nos funcionó.

Otro problema que tuvimos, fue a la hora de hacer la partición. Si bien, debido al problema anterior la partición se nos dificultaba, en el momento de que empezaba la paritción, el código nos retornaba un error.


### 2. Funcionalidad pendiente sin implementar.
Debido a que nuestro parallel quicksort no es exactamente como el que nos recomiendan, no utilizamos mutex ni variables de condición. Sin embargo sigue siendo paralelo, debido a que ocupa thrads a medida que el arreglo se va particionando.
