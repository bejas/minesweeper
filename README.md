# minesweeper
Minesweeper game written in C for a school project

Campo Minato

Autori: 
Bertazzon Nicholas e Jasari Besar

Per compilare:
gcc -std=gnu89 -Wall -pedantic -o campo_minato graphics_management.c structures_management.c field_management.c classification_management.c main.c -lcurses


Nota:
Durante il gioco verranno creati due file:
classification.txt -> contiene tutti i tempi di tutte le partite vinte.
input.txt -> contiene le impostazioni del campo attuale

Se il file input ancora non c'e', viene creato un campo preimpostato di 10x20 con 10 mine.

Ad ogni modifica delle impostazioni viene creato un nuovo campo, anche se viene modificata solo una delle tre opzioni.
E per la domanda "se voglio solo generare un nuovo campo con le impostazioni che ci sono gia'?" la risposta e': "basta premere N"
