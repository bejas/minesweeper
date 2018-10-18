#include "structures_management.h"
#include "field_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_VALORI 3

/*
 * Metodo che controlla se l'utente ha vinto.
 * Un utente ha vinto se ha scoperto tutta la parte di campo scopribile.
 * Il metodo controlla cella per cella, e ritorna:
 * 0- ci sono ancora celle scopribili.
 * 1- tutte le celle scopribili sono state scoperte.
 */
int check_win(Field_manager my_field, int num_rows, int num_columns) {
    int win = 1, i = 0, j = 0;
    for (; i < num_rows; i++) {
        for (; j < num_columns; j++) {
            int code = get_code(my_field, i, j);
            int flag = get_flag(my_field, i, j);
            if (code >= 0 && flag != 1) {
                win = 0;
            }
        }

        j = 0;
    }

    return win;
}

/*
 * Metodo che inizializza la matrice di celle, dato numero di righe e numero di colonne.
 * Oltre che allocare la memoria, setta ogni code della cella a -2, e ogni flag a 0.
 * Per info su code e flag andare su structures_management.h
 */
void initialize_field(Field_manager *my_field, int num_rows, int num_columns) {
    int i = 0, j = 0;
    (*my_field).field = malloc(sizeof (*((*my_field).field)) * num_rows);

    for (; i < num_rows; i++) {
        (*my_field).field[i] = malloc(sizeof ((*my_field).field[i]) * num_columns);
    }

    i = 0;
    for (; i < num_rows; i++) {
        for (; j < num_columns; j++) {
            set_code(my_field, i, j, -2);
            set_flag(my_field, i, j, 0);
        }

        j = 0;
    }

    (*my_field).list_moves = NULL;
    (*my_field).cancellations_used = 0;
}

/*
 * Creazione di un nuovo file di campo, date numero di righe, numero di colonne 
 * e numero di mine.
 * 
 * Può ritornare:
 * 1 = riuscito
 * 2 = dimensioni non valide
 * 3 = numero di mine non valido
 * 4 = impossibile aprire il file input.txt
 */
int create_file_field(int num_rows, int num_columns, int tot_mines) {
    int codice = 1;

    if (num_rows > 0 && num_columns > 0) {
        if (tot_mines >= 0 && tot_mines < (num_rows * num_columns)) {
            FILE *fp;
            fp = fopen("input.txt", "w");
            if (fp) {
                int i = 0, random_row, random_column;
                List_mines list_mines = NULL;
                fprintf(fp, "%d, %d\n", num_rows, num_columns);
                srand(time(NULL));
                for (; i < tot_mines; i++) {
                    random_row = rand() % num_rows;
                    random_column = rand() % num_columns;
                    if (append_checking(&list_mines, random_row, random_column)) {
                        fprintf(fp, "%d, %d", random_row, random_column);
                        if (i != (tot_mines - 1)) {
                            fprintf(fp, "\n");
                        }
                    } else {
                        i--;
                    }
                }
                fclose(fp);
            } else {
                codice = 4;
            }
        } else {
            codice = 3;
        }
    } else {
        codice = 2;
    }
    return codice;
}

/*
 * Metodo che incremento la variabile puntata mines se 
 * e solo se la cella alla posizione indicata è una mina (code -1).
 */
void increment_mines_counter(Field_manager my_field, int pos_row, int pos_column, int *mines) {
    if ((get_code(my_field, pos_row, pos_column)) == -1) {
        (*mines)++;
    }
}

/*
 * Metodo che conta il numero di mine presenti nelle otto celle intorno 
 * alla cella data (identificata da pos_row e pos_column).
 */
int count_mines(Field_manager my_field, int pos_row, int pos_column) {
    int mines = 0;

    /* controlliamo se esistono le celle */
    int left = ((pos_column - 1) >= 0) ? 1 : 0;
    int right = ((pos_column + 1) < my_field.num_columns) ? 1 : 0;
    int up = ((pos_row - 1) >= 0) ? 1 : 0;
    int down = ((pos_row + 1) < my_field.num_rows) ? 1 : 0;

    if (up) {
        increment_mines_counter(my_field, pos_row - 1, pos_column, &mines);
    }

    if (down) {
        increment_mines_counter(my_field, pos_row + 1, pos_column, &mines);
    }

    if (left) {
        increment_mines_counter(my_field, pos_row, pos_column - 1, &mines);
    }

    if (right) {
        increment_mines_counter(my_field, pos_row, pos_column + 1, &mines);
    }

    if (up && right) {
        increment_mines_counter(my_field, pos_row - 1, pos_column + 1, &mines);
    }

    if (up && left) {
        increment_mines_counter(my_field, pos_row - 1, pos_column - 1, &mines);
    }

    if (down && right) {
        increment_mines_counter(my_field, pos_row + 1, pos_column + 1, &mines);
    }

    if (down && left) {
        increment_mines_counter(my_field, pos_row + 1, pos_column - 1, &mines);
    }

    return mines;
}

/*
 * Metodo principale per creare il campo "ingame" (usato poi dalle altre funzioni).
 * (1) - Dapprima inizializza la matrice con il metodo initialize_field.
 * (2) - Dopodichè, per ogni mina che trova, setta il code a -1 nella cella appropriata.
 * (3) - Infine, setta il code corrispondente di tutte le altre celle.
 *  
 * Ritorna: 
 * 0 -> impossibile aprire il file input.txt
 * 1-> riuscito
 */
int create_ingame_field(Field_manager *my_field, int num_rows, int num_columns) {
    FILE *fp;
    int result = 1;
    size_t len = 0;
    int read;
    int i = 0;
    int pos1 = 0, pos2 = 0;
    char *line = NULL;
    (*my_field).num_rows = num_rows;
    (*my_field).num_columns = num_columns;
    /* (1) */
    initialize_field(my_field, num_rows, num_columns);
    fp = fopen("input.txt", "r");
    if (fp) {
        /* (2) */
        while ((read = (getline(&line, &len, fp))) != -1) {
            if (read != 0) {
                if (line[read - 1] == '\n') {
                    line[read - 1] = '\0';
                }
                if (i != 0) {
                    sscanf(line, "%d, %d", &pos1, &pos2);
                    set_code(my_field, pos1, pos2, -1);
                } else {
                    i++;
                }
            }
        }
        pos1 = 0;
        pos2 = 0;
        /* (3) */
        for (; pos1 < num_rows; pos1++) {
            for (; pos2 < num_columns; pos2++) {
                if ((get_code(*my_field, pos1, pos2)) == -2) {
                    set_code(my_field, pos1, pos2, count_mines(*my_field, pos1, pos2));
                }
            }
            pos2 = 0;
        }

    } else {
        result = 0;
    }
    return result;
}

/*
 * Metodo che scopre massivamente tutte le celle del campo.
 * Non fa distinzioni fra mine o non mine.
 */
void massive_uncover_cells(Field_manager *my_field) {
    int i = 0, j = 0;
    for (; i < (*my_field).num_rows; i++) {
        for (; j < (*my_field).num_columns; j++) {
            set_flag(my_field, i, j, 1);
        }

        j = 0;
    }
}

/*
 * Metodo che scopre una cella.
 * L'opzione mina viene trattata su "uncover_cells", che è il metodo principale 
 * per lo scoprimento delle mine, e l'unico che viene richiamato nel graphics_management;
 * Leggere la descrizione di do_uncover_cells per ulteriori chiarimenti.
 * 
 * Ritorna:
 * 0 -> successo nulla, cella già scoperta/bandiera
 * 1 -> scoperto un numero
 * 2 -> scoperta una cella vuota
 */
int uncover_cell(Field_manager *my_field, int pos1, int pos2) {
    int result = 0;
    int code = get_code(*my_field, pos1, pos2);
    int flag = get_flag(*my_field, pos1, pos2);
    if (flag == 0) {
        if (code == 0) {
            result = 2;
        } else {
            result = 1;
        }

        set_flag(my_field, pos1, pos2, 1);
    }
    return result;
}

/*
 * Metodo che scopre tutte le celle scopribili adiacente alla cella identificata
 * dalle posizioni riga e colonna. 
 * Questo metodo viene richiamato solo se la cella non è una mina.
 * (1) - Dapprima scopre la cella, e, se possibile, l'aggiunge alla lista di
 * celle cambiate (all'interno dell'ultima mossa disponibile nella lista di mosse).
 * (2) - Dopodichè, se la cella è vuota (e non un numero), procede a scoprire
 * anche le altre adiacenti.
 */
void do_uncover_cells(Field_manager *my_field, int pos_row, int pos_column) {
    if (pos_row >= 0 && pos_row < (*my_field).num_rows && pos_column >= 0 && pos_column < (*my_field).num_columns) {
        /* (1) */
        int cell_uncovered = uncover_cell(my_field, pos_row, pos_column);
        if (cell_uncovered == 2 || cell_uncovered == 1) {
            append_changed_cell_l(my_field, get_cell(my_field, pos_row, pos_column));
        }
        /* (2) */
        if (cell_uncovered == 2) {
            do_uncover_cells(my_field, pos_row - 1, pos_column);
            do_uncover_cells(my_field, pos_row + 1, pos_column);
            do_uncover_cells(my_field, pos_row, pos_column - 1);
            do_uncover_cells(my_field, pos_row, pos_column + 1);
            do_uncover_cells(my_field, pos_row - 1, pos_column + 1);
            do_uncover_cells(my_field, pos_row - 1, pos_column - 1);
            do_uncover_cells(my_field, pos_row + 1, pos_column + 1);
            do_uncover_cells(my_field, pos_row + 1, pos_column - 1);
        }
    }
}

/*
 * Principale metodo per lo scoprimento delle celle.
 * 
 * Da notare:
 * (1) : se non è una mina, aggiungiamo una nuova mossa (vuota ma che poi si riempirà).
 * 
 * Ritorna:
 * 0 -> esplosa una mina
 * 1 -> si può continuare
 * 2 -> vinto
 */
int uncover_cells(Field_manager *my_field, int pos_row, int pos_column) {
    int result = 1;
    int flag = get_flag(*my_field, pos_row, pos_column);
    if (flag == 0) {
        int code = get_code(*my_field, pos_row, pos_column);
        if (code == -1) {
            result = 0;
        } else {
            /* (1) */
            append_move(&((*my_field).list_moves));
            do_uncover_cells(my_field, pos_row, pos_column);
            if (check_win(*my_field, (*my_field).num_rows, (*my_field).num_columns)) {
                result = 2;
            }
        }
    }

    return result;
}

/*
 * Metodo usato per annullare le ultime x mosse (dove x è il numero di
 * annullamenti usati dall'utente).
 * 
 * (1) - Malus, torniamo indietro tante volte quante sono le volte annullate. Da notare
 * che la scoperta di una mina non viene considerata una "mossa" da aggiungere 
 * alla lista del field_manager. L'annullamento della scoperta della mina, infatti,
 * viene fatto direttamente nel graphics_management (se l'utente sceglie di tornare
 * indietro). Proprio per questo la variabile i parte da 1 invece che da 0.
 * 
 * (2) - Se il primi nodo non è NULL, iteriamo finchè non troviamo l'ultimo nodo
 * (ovvero l'ultima mossa).
 * 
 * (3) - Puntiamo alla prima cella della lista di "celle cambiate", e iteriamo
 * risettando il flag a 0 di tutti i nodi (ricopriamo così tutte le celle della lista).
 * 
 * (4) - Infine liberiamo le memoria.
 * 
 * (5) - Da notare che, se prev_move è NULL, vuol dire che il current_move usato
 * era l'unico nodo nella lista di mosse.
 */
void go_back(Field_manager *my_field) {
    int i = 1;
    remove_one_cancellation(my_field);
    /* (1) */
    for (; i < get_cancellations_used(*my_field); i++) {
        List_moves current_move = (*my_field).list_moves;
        List_moves prev_move = NULL;
        List_changed_cells current_c_cell = NULL;

        /* (2) */
        if (current_move != NULL) {
            int j = 0;
            while (current_move->next != NULL) {
                if (j >= 0) {
                    prev_move = current_move;
                }
                current_move = current_move->next;
                j++;
            }
            
            /* (3) */
            current_c_cell = current_move->list_changed_cells;

            if (current_c_cell != NULL) {
                while (current_c_cell != NULL) {
                    current_c_cell->cell->flag = 0;
                    current_c_cell = current_c_cell->next;
                }

                /* (4) */
                destroy_list_changed_cells(current_move->list_changed_cells);
                free(current_move);
            }

            /* (5) */
            if (prev_move == NULL) {
                (*my_field).list_moves = NULL;
            } else {
                prev_move->next = NULL;
            }
        }
    }
}

/*
 * Metodo che legge da "input.txt" e ritorna l'indirizzo di un vettore con
 * numero di righe, numero di colonne e numero di mine nel campo.
 */
int * get_current_settings() {
    FILE *fp;
    size_t len = 0;
    int read;
    int i = 0;
    char *line = NULL;
    int *values = malloc(3 * sizeof (int));
    fp = fopen("input.txt", "r");
    for (; i < 3; i++) {
        values[i] = 0;
    }

    i = 0;

    if (fp) {
        while ((read = getline(&line, &len, fp)) != -1) {
            if (read != 0) {
                if (line[read - 1] == '\n') {
                    line[read - 1] = '\0';
                }
                if (i != 0) {
                    values[2]++;
                } else {
                    sscanf(line, "%d, %d", &values[0], &values[1]);
                    i++;
                }
            }
        }
    }
    return values;
}

void purge(Field_manager my_field){
    destroy_matrix(my_field);
    destroy_list_moves(my_field.list_moves);
}
