#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "classification_management.h"

/*
 * Metodo che appende un nuovo punteggio(tempo) nel file "classification.txt"
 * I tempi sono divisi per numero di righe, colonne e mine nel campo.
 */
int append_score(int minutes, int seconds, int rows, int columns, int mines) {
    FILE *fp;
    int result = 1;
    fp = fopen("classification.txt", "a");

    if (fp) {
        fprintf(fp, "%dX%d - %d:%d - %d mines\n", rows, columns, minutes, seconds, mines);
		fclose(fp);
    } else {
        result = 0;
    }
    
    return result;
}

/*
 * Metodo che ritorna (in secondi) il migliore dei tempi registrati con quel 
 * totale di righe, quel totale di colonne e quel totale di mine.
 */
int get_record(int rows, int columns, int mines) {
    int result = -1;
    char *line = NULL;
    size_t len = 0;
    int read;
    int num_mines, num_rows, num_columns, minutes, seconds, record_minutes = 0, record_seconds = -1;
    FILE *fp = fopen("classification.txt", "r");
    if (fp) {
        while ((read = (getline(&line, &len, fp))) != -1) {
            sscanf(line, "%dX%d - %d:%d - %d", &num_rows, &num_columns, &minutes, &seconds, &num_mines);
            if (num_rows == rows && num_columns == columns && num_mines == mines) {
                if (record_seconds == -1) {
                    record_minutes = minutes;
                    record_seconds = seconds;
                } else {
                    if (record_minutes > minutes) {
                        record_minutes = minutes;
                        record_seconds = seconds;
                    } else if (record_minutes == minutes) {
                        if (seconds < record_seconds) {
                            record_seconds = seconds;
                        }
                    }
                }
                result = (record_seconds + (record_minutes * 60));
            }
        }
		fclose(fp);
    }
    

    return result;
}
