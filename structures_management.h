#ifndef STRUCTURES_MANAGEMENT_H
#define STRUCTURES_MANAGEMENT_H

/*
 * INTRODUZIONE:
 * structures_management.c e structures_management.h
 * Definizione delle strutture dati e operazioni basilari su di esse.
 */

/*
 * Struttura di una mina.
 */
struct mine {
    int pos_row;
    int pos_column;
    struct mine *next;
};
/*
 * Lista di mine (uso la lista come appoggio per evitare duplicazioni di mine nel file).
 */
typedef struct mine* List_mines;

/*
 * Struttura di una cella del campo.
 * 
 * Codice può essere:
 * 0 -> non c'è nulla
 * 1-8 -> 8 mine vicino
 * -1 -> mina
 * -2 -> ancora da scriverci
 * 
 * Flag può essere:
 * 0 -> coperta
 * 1 -> scoperta
 * 2 -> bandiera
 * 3 -> mina o non mina?
 */
struct cell {
    int code;
    int flag;
};
typedef struct cell Cell;
typedef struct cell **Matrix_field;

/*
 * Ogni mossa può provocare la scoperta di x celle. Con la struttura changed_cell teniamo conto
 * della scoperta di una cella.
 */
struct changed_cell {
    Cell *cell;
    struct changed_cell *next;
};
typedef struct changed_cell *List_changed_cells;

/*
 * Struttura di una mossa.
 * La lista di changed_cell tiene conto di tutte le celle che sono cambiate con quella mossa.
 */
struct move {
    List_changed_cells list_changed_cells;
    struct move *next;
};
typedef struct move *List_moves;

/*
 * La Struttura principale.
 * Al suo interno abbiamo:
 * -matrix_field : la matrice che costituisce il campo.
 * -list_moves : la lista di mosse
 * -num_rows : il numero delle righe del campo.
 * -num_columns : il numero delle colonne del campo.
 * -cancellations_remaining : gli annullamenti rimasti 
 * -cancellations_used : gli annullamenti usati(per tenere conto del malus).
 */
typedef struct field_manager {
    Matrix_field field;
    List_moves list_moves;
    int num_rows;
    int num_columns;
    int cancellations_remaining;
    int cancellations_used;
} Field_manager;

/* 
 * Metodi 
 */
void set_code(Field_manager *my_field, int pos1, int pos2, int type);
int get_code(Field_manager my_field, int pos1, int pos2);
void set_flag(Field_manager *my_field, int pos1, int pos2, int type);
int get_flag(Field_manager my_field, int pos1, int pos2);
void set_cancellations_remaining(Field_manager *my_field, int amount);
void remove_one_cancellation(Field_manager *my_field);
int get_cancellations_remaining(Field_manager my_field);
int get_cancellations_used(Field_manager my_field);
Cell * get_cell(Field_manager *my_field, int pos1, int pos2);

int append_checking(List_mines *list, int pos_row, int pos_column);
void append_changed_cell_l(Field_manager *my_field, Cell *cell);
void append_move(List_moves *list);
void destroy_list_changed_cells(List_changed_cells list);
void destroy_list_moves(List_moves list);
void destroy_matrix(Field_manager my_field);

#endif

