#include "structures_management.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * INTRODUZIONE:
 * structures_management.c e structures_management.h
 * Definizione delle strutture dati e operazioni basilari su di esse.
 */

struct cell * get_cell(Field_manager *my_field, int pos1, int pos2) {
    return &((*my_field).field[pos1][pos2]);
}

void set_code(Field_manager *my_field, int pos1, int pos2, int type) {
    (*my_field).field[pos1][pos2].code = type;
}

int get_code(Field_manager my_field, int pos1, int pos2) {
    return my_field.field[pos1][pos2].code;
}

void set_flag(Field_manager *my_field, int pos1, int pos2, int type) {
    (*my_field).field[pos1][pos2].flag = type;
}

int get_flag(Field_manager my_field, int pos1, int pos2) {
    return my_field.field[pos1][pos2].flag;
}

void set_cancellations_remaining(Field_manager *my_field, int amount) {
    (*my_field).cancellations_remaining = amount;
}

void remove_one_cancellation(Field_manager *my_field) {
    (*my_field).cancellations_remaining--;
    (*my_field).cancellations_used++;
}

int get_cancellations_remaining(Field_manager my_field) {
    return my_field.cancellations_remaining;
}

int get_cancellations_used(Field_manager my_field) {
    return my_field.cancellations_used;
}


/*
 * SEZIONE: Lista delle mine. 
 */

/*
 * Aggiunta in coda alla lista di mine.
 */
void listac_append(List_mines *list, int pos_row, int pos_column) {
    if (*list == NULL) {
        *list = (List_mines) malloc(sizeof (struct mine));
        if (*list) {
            (*list)->next = NULL;
            (*list)->pos_row = pos_row;
            (*list)->pos_column = pos_column;
        }
    } else {
        listac_append(&((*list)->next), pos_row, pos_column);
    }
}

/*
 * Piccolo check prima dell'aggiunta in coda alla lista, per evitare la presenza di più mine in uno stesso punto.
 */
int append_checking(List_mines *list, int pos_row, int pos_column) {
    List_mines temp = *list;
    int found = 0;
    while (temp != NULL && !found) {
        if (temp->pos_row == pos_row && temp->pos_column == pos_column) {
            found = 1;
        } else {
            temp = temp->next;
        }
    }

    if (!found) {
        listac_append(list, pos_row, pos_column);
        return 1;
    } else {
        return 0;
    }
}


/*
 * SEZIONE: Lista delle mosse.
 */

/*
 * Aggiunta di una nuova mossa alla lista(in generale).
 * All'inizio è NULL, ma gli append_changed_cell aggiungeranno man mano nodi alla lista.
 */
void append_move(List_moves *list) {
    if ((*list) == NULL) {
        (*list) = (List_moves) malloc(sizeof (struct move));
        (*list)->list_changed_cells = NULL;
        (*list)->next = NULL;
    } else {
        append_move(&((*list)->next));
    }
}

/*
 * Aggiunta di una nuova referenzazione ad una cella nella lista changed_cells di un mossa(in generale).
 */
void append_changed_cell(List_changed_cells *list, Cell *cell) {
    if ((*list) == NULL) {
        (*list) = (List_changed_cells) malloc(sizeof (struct changed_cell));
        (*list)->cell = cell;
        (*list)->next = NULL;
    } else {
        append_changed_cell(&((*list)->next), cell);
    }
}

/*
 * Ritorna un puntatore all'ultima mossa della lista.
 */
List_moves * get_latest_move(List_moves *list) {
    if ((*list)->next != NULL) {
        return get_latest_move(&((*list)->next));
    } else {
        return list;
    }
}

/*
 * Aggiunta di una referenzazione ad una cella nella lista changed_cells dell'ultima mossa disponibile.
 * l = latest (per distinguerlo da quello in generale)
 */
void append_changed_cell_l(Field_manager *my_field, Cell *cell) {
    List_moves *current_moves = get_latest_move(&((*my_field).list_moves));
    append_changed_cell(&((*current_moves)->list_changed_cells), cell);
}

/*
 * Distrugge la lista di changed_cells.
 */
void destroy_list_changed_cells(List_changed_cells list) {
    List_changed_cells temp = list;
    while (temp) {
        list = temp->next;
        free(temp);
        temp = list;
    }
}

void destroy_list_moves(List_moves list) {
    if (list) {
        if (list->next != NULL) {
            destroy_list_moves(list->next);
        }

        destroy_list_changed_cells(list->list_changed_cells);
        free(list);
    }
}

void destroy_matrix(Field_manager my_field) {
    int i = 0;
    for (; i < my_field.num_rows; i++) {
        free(my_field.field[i]);
    }
    free(my_field.field);
    my_field.field = NULL;
}
