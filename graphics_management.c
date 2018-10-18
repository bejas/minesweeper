#include "graphics_management.h"
#include "field_management.h"
#include "classification_management.h"
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/* Metodo che viene usato per gestire le varie finestre del gioco. */
void campo_minato() {
    int scelta = 1;

    while (scelta != -1) {
        switch (scelta) {
            case 1:
                scelta = menu_iniziale();
                break;

            case 2:
                scelta = impostazioni();
                break;

            case 3:
                scelta = screen_err();
                break;

            case 4:
                scelta = game();
                break;
        }
    }
}

/* Metodo per stampare su schermo il campo.
 * Usato ogni volta che avviene un cambiamento sul campo.
 */
void print_field(Field_manager campo, WINDOW** gamewin) {
    int i, j, flag;
    int br = 1; /* BORDO RIGHE */
    int bc = 2; /* BORDO COLONNE */

    box(*gamewin, 0, 0);

    for (i = 0; i < campo.num_rows; i++) {
        for (j = 0; j < campo.num_columns; j++) {
            flag = get_flag(campo, i, j); /* CALCOLO IL FLAG DELLA CELLA CONSIDERATA */

            if (flag == 0) /* CELLA COPERTA */
                mvwprintw(*gamewin, i + br, j + bc, "#");

            else if (flag == 1) /* CELLA SCOPERTA */ {
                int code = get_code(campo, i, j);

                if (code == -1)
                    mvwprintw(*gamewin, i + br, j + bc, "*");
                else if (code == 0)
                    mvwprintw(*gamewin, i + br, j + bc, " ");
                else if (code > 0)
                    mvwprintw(*gamewin, i + br, j + bc, "%d", code);
            }
            else if (flag == 2) /* BANDIERA ! */
                mvwprintw(*gamewin, i + br, j + bc, "!");

            else if (flag == 3) /* BANDIERA ? */
                mvwprintw(*gamewin, i + br, j + bc, "?");
        }
    }
    wrefresh(*gamewin);
}

/* Metodo per stampare su schermo la stringa dei comandi e bandiere/ritorni rimastie/i.
 * Usato ogni volta che avviene un aggiornamento su bandiere e/o ritorni rimasti.
 */
void print_hud(Field_manager campo, int bandiera) {
    int srMax, scMax;
    int returns = get_cancellations_remaining(campo);
    
    getmaxyx(stdscr, srMax, scMax);
    /*scMax in realtà non ha utilità in questo contesta, ma tocca utilizzarla per via del warning durante la compilazione*/
    mvprintw(srMax - 1 + scMax - scMax, 0, "                                                           ");
    mvprintw(srMax - 1, 0, "Q: ESCI   F: BANDIERA(%d)   INVIO: SCOPRI   R: INDIETRO(%d)", bandiera, returns);
    refresh();
}

/* Questo metodo viene chiamato all'avvio del programma
 * e gestisce tutto il menu iniziale.
 */
int menu_iniziale() {
    /* NCURSES START */
    /* VARIABLES */
    int brMax, bcMax;
    int srMax, scMax;
    WINDOW* menuwin;
    char* choices[3] = {"INIZIA PARTITA", "IMPOSTAZIONI", "ESCI"};
    int choice, i;
    int highlight = 0;
    int num_rows, *config;
    FILE* fp;

    /* FILE SETTINGS */
    fp = fopen("input.txt", "r");
    if (!fp)
        create_file_field(10, 20, 10);
    else
        fclose(fp);
    config = get_current_settings();
    num_rows = config[0];
    free(config);

    /* SCREEN SETTINGS */
    initscr();
    clear();
    noecho();
    curs_set(0);
    getmaxyx(stdscr, srMax, scMax);

    /* WINDOW SETTINGS */
    menuwin = newwin(5, 18, srMax / 2 + 1, scMax / 2 - 9);
    box(menuwin, 0, 0);
    getmaxyx(menuwin, brMax, bcMax);
    keypad(menuwin, true);

    mvprintw(srMax / 2 - 8, scMax / 2 - 33, "   ___                                     _             _        ");
    mvprintw(srMax / 2 - 7, scMax / 2 - 33, "  / __\\__ _ _ __ ___  _ __   ___     /\\/\\ (_)_ __   __ _| |_ ___  ");
    mvprintw(srMax / 2 - 6, scMax / 2 - 33, " / /  / _` | '_ ` _ \\| '_ \\ / _ \\   /    \\| | '_ \\ / _` | __/ _ \\ ");
    mvprintw(srMax / 2 - 5, scMax / 2 - 33, "/ /__| (_| | | | | | | |_) | (_) | / /\\/\\ \\ | | | | (_| | || (_) |");
    mvprintw(srMax / 2 - 4, scMax / 2 - 33, "\\____/\\__,_|_| |_| |_| .__/ \\___/  \\/    \\/_|_| |_|\\__,_|\\__\\___/ ");
    mvprintw(srMax / 2 - 3, scMax / 2 - 33, "                     |_|                                          ");

    /* stessa storia dell'scMax di printhub, tocca utilizzare in qualche modo brMax per via dei warning "set but not used"*/
    mvprintw(srMax + brMax - brMax - 1, 0, "Created by Nicholas Bertazzon and Besar Jasari.");
    refresh();

    /* CHOICE WAITING */
    do {
        /* MENU PRINT */
        for (i = 0; i < 3; i++) {
            if (i == highlight)
                wattron(menuwin, A_REVERSE);
            mvwprintw(menuwin, i + 1, (bcMax / 2 - strlen(choices[i]) / 2), choices[i]);
            wattroff(menuwin, A_REVERSE);
        }

        choice = wgetch(menuwin);

        switch (choice) {
            case KEY_UP:
                highlight--;
                if (highlight == -1)
                    highlight = 2;
                break;

            case KEY_DOWN:
                highlight++;
                if (highlight == 3)
                    highlight = 0;
                break;
        }
    } while (choice != 10);

    if (highlight == 0) {
        getmaxyx(stdscr, srMax, scMax);
        if ((srMax - num_rows - 20 < 0) || (scMax < 55)) /* controllo se screen_err oppure no */
            return 3;
        else
            return 4;
    }

    if (highlight == 1)
        return 2;

    if (highlight == 2)
        endwin();
    return -1;
}

/* Metodo che viene chiamato qualora l'utente vuole cambiare le impostazioni
 * di righe, colonne oppure mine totali.
 */
int impostazioni() {
    /* VARIABLES */
    int srMax, scMax;
    int brMax, bcMax;
    WINDOW* impostazioniwin;
    char* choices[4] = {"Righe", "Colonne", "Mine", "INDIETRO"};
    int choice, i, highlight = 0, temp = 0, temp1 = 0;
    int * config = get_current_settings();

    /* SCREEN SETTINGS */
    initscr();
    clear();
    noecho();
    getmaxyx(stdscr, srMax, scMax);

    /* WINDOW SETTINGS */
    impostazioniwin = newwin(6, 30, srMax / 2 - 3, scMax / 2 - 15);
    box(impostazioniwin, 0, 0);
    getmaxyx(impostazioniwin, brMax, bcMax);
    keypad(impostazioniwin, true);

    mvprintw(srMax / 2 - brMax / 2 - 2, scMax / 2 - 6, "IMPOSTAZIONI");
    mvprintw(srMax - 1, 0, "N: GENERA NUOVO CAMPO");

    refresh();

    /* ALL SETTINGS PRINTERS */
    while (highlight != 3 || choice != 10) {
        /* SETTINGS PRINT */
        for (i = 0; i < 3; i++)
            mvwprintw(impostazioniwin, i + 1, bcMax - 3, "%d", config[i]);

        /* CHOICES PRINT */
        for (i = 0; i < 4; i++) {
            if (i == highlight)
                wattron(impostazioniwin, A_REVERSE);
            mvwprintw(impostazioniwin, i + 1, 2, choices[i]);
            wattroff(impostazioniwin, A_REVERSE);
        }

        choice = wgetch(impostazioniwin);

        switch (choice) {
            case KEY_UP:
                highlight--;
                if (highlight == -1)
                    highlight = 3;
                break;

            case KEY_DOWN:
                highlight++;
                if (highlight == 4)
                    highlight = 0;
                break;

            case 10: /* ENTER CASE */
            {
                if (highlight != 3) {
                    mvwprintw(impostazioniwin, highlight + 1, bcMax - 3, "  ");
                    curs_set(1);
                    wmove(impostazioniwin, highlight + 1, bcMax - 3);

                    /* FIRST NUMBER */
                    temp = wgetch(impostazioniwin);

                    if (temp >= 48 && temp <= 57) {
                        temp -= 48;
                        wprintw(impostazioniwin, "%d", temp);

                        /* SECOND NUMBER */
                        temp1 = wgetch(impostazioniwin);

                        if (temp1 >= 48 && temp1 <= 57) {
                            temp *= 10;
                            wprintw(impostazioniwin, "%d", temp1 - 48);
                            temp += temp1 - 48;
                            wmove(impostazioniwin, highlight + 1, bcMax - 2);
                        }
                        config[highlight] = temp;
                        create_file_field(config[0], config[1], config[2]);
                    }
                    curs_set(0);
                } else{/* INDIETRO */
                    free(config);
                    return 1;
                } 
                    
                break;
            }

            case 'N': case 'n':
                mvprintw(srMax / 2 + 4, scMax / 2 - 10, "Nuovo campo generato!");
                refresh();
                create_file_field(config[0], config[1], config[2]);
                break;
        }
    }
    free(config);
    return 1;
}

/* Metodo principale dove si svolge la partita in corso.
 * Gestisce ogni mossa del giocatore.
 */
int game() {
    /* VARIABLES */
    int srMax, scMax;
    int brMax, bcMax;
    time_t start;
    time_t end;
    int record;
    int bd_r = 1, bd_c = 2;
    int r, c;
    int choice;
    int returns;
    int unc_cell;
    int new_game = 0;
    WINDOW* gamewin;
    /* config[0] == Righe   config[1] == Colonne    config[2] == Mine */
    int* config = get_current_settings();
    int bandiera = config[2];
    Field_manager campo;

    /* SET RETURNS TIMES */
    returns = screen_return();
    set_cancellations_remaining(&campo, returns);

    /* SCREEN SETTINGS */
    initscr();
    clear();
    refresh();
    getmaxyx(stdscr, srMax, scMax);

    /* HUD */
    print_hud(campo, bandiera);

    /* WINDOW SETTINGS */
    gamewin = newwin(config[0] + 2, config[1] + 4, srMax / 2 - (config[0] + 2) / 2, scMax / 2 - (config[1] + 4) / 2);
    box(gamewin, 0, 0);
    getmaxyx(gamewin, brMax, bcMax);
    keypad(gamewin, true);

    /* CREAZIONE DEL CAMPO */
    create_ingame_field(&campo, config[0], config[1]);
    print_field(campo, &gamewin);
    start = time(NULL);

    curs_set(1);
    wmove(gamewin, bd_r, bd_c);

    /* CHOICE CELL */
    do {
        choice = wgetch(gamewin);
        getyx(gamewin, r, c);

        switch (choice) {
            case KEY_DOWN:
                r += 1;
                if (r == brMax - 1)
                    r = 1;
                wmove(gamewin, r, c);
                break;

            case KEY_UP:
                r -= 1;
                if (r == 0)
                    r = brMax - 2;
                wmove(gamewin, r, c);
                break;

            case KEY_LEFT:
                c -= 1;
                if (c == 1)
                    c = bcMax - 3;
                wmove(gamewin, r, c);
                break;

            case KEY_RIGHT:
                c += 1;
                if (c == bcMax - 2)
                    c = 2;
                wmove(gamewin, r, c);
                break;

            case 'f': case 'F': /* Quando si preme "f" "F" */
            {
                r -= bd_r;
                c -= bd_c;

                switch (get_flag(campo, r, c)) {
                    case 0: /* COPERTO */
                        if (bandiera > 0) {
                            bandiera--;
                            set_flag(&campo, r, c, 2);
                        } else
                            set_flag(&campo, r, c, 3);
                        break;

                    case 1: /* SCOPERTO */
                        break;

                    case 2: /* BANDIERA ! */
                        bandiera++;
                        set_flag(&campo, r, c, 3);
                        break;

                    case 3: /* BANDIERA ? */
                        set_flag(&campo, r, c, 0);
                        break;
                }

                getyx(gamewin, r, c);
                print_field(campo, &gamewin);
                wmove(gamewin, r, c);
                print_hud(campo, bandiera);
                break;
            }

            case 10: /* Quando si preme INVIO */
            {
                r -= bd_r;
                c -= bd_c;

                /* scopriamo una cella */
                unc_cell = uncover_cells(&campo, r, c);

                if (unc_cell == 2) /* VITTORIA */ {
                    end = time(NULL);
                    end = end - start;
                    append_score(end / 60, end % 60, config[0], config[1], config[2]);
                    record = get_record(config[0], config[1], config[2]);


                    mvprintw(srMax / 2 - (brMax + 2) / 2 - 6, scMax / 2 - 17, "                  __    __ _       ");
                    mvprintw(srMax / 2 - (brMax + 2) / 2 - 5, scMax / 2 - 17, "/\\_/\\___  _   _  / / /\\ \\ (_)_ __  ");
                    mvprintw(srMax / 2 - (brMax + 2) / 2 - 4, scMax / 2 - 17, "\\_ _/ _ \\| | | | \\ \\/  \\/ / | '_ \\ ");
                    mvprintw(srMax / 2 - (brMax + 2) / 2 - 3, scMax / 2 - 17, " / \\ (_) | |_| |  \\  /\\  /| | | | |");
                    mvprintw(srMax / 2 - (brMax + 2) / 2 - 2, scMax / 2 - 17, " \\_/\\___/ \\__,_|   \\/  \\/ |_|_| |_|");
                    mvprintw(srMax / 2 - (brMax + 2) / 2 - 1, scMax / 2 - 17, "                                   ");

                    mvprintw(srMax / 2 + (brMax + 2) / 2 + (brMax % 2) + 1, scMax / 2 - 10, "TEMPO MIGLIORE: %d:%02d", record / 60, record % 60);
                    mvprintw(srMax / 2 + (brMax + 2) / 2 + (brMax % 2) + 2, scMax / 2 - 10, "TEMPO ATTUALE: %d:%02d", end / 60, end % 60);
                    mvprintw(srMax / 2 + (brMax + 2) / 2 + (brMax % 2) + 5, scMax / 2 - 16, "Premi N per una nuova partita ..");


                    print_field(campo, &gamewin);
                    refresh();
                    curs_set(0);

                    do
                        choice = getch(); while (choice != 'q' && choice != 'Q' && choice != 'n' && choice != 'N');

                    if (choice == 'n' || choice == 'N') {
                        new_game = 1;
                        choice = 'q';
                    }

                } else if (unc_cell == 1) /* MINA NON TROVATA */ {
                    getyx(gamewin, r, c);
                    print_field(campo, &gamewin);
                    wmove(gamewin, r, c);

                } else if (unc_cell == 0) /* MINA TROVATA */ {
                    if (get_cancellations_remaining(campo) > 0) /* MINA TROVATA CON POSSIBILITà DI TORNARE INDIETRO*/ {
                        getyx(gamewin, r, c);

                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 7, scMax / 2 - 4, "     ,--.!,");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 6, scMax / 2 - 4, "  __/   -*-");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 5, scMax / 2 - 4, ",d08b.  '|`");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 4, scMax / 2 - 4, "0088MM     ");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 3, scMax / 2 - 4, "`9MMP'     ");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 1, scMax / 2 - 8, "TROVATO UNA MINA!");
                        mvprintw(srMax / 2 + (brMax + 2) / 2 + (brMax % 2) + 1, scMax / 2 - 15, "Premi R per tornare indietro ..");

                        mvwprintw(gamewin, r, c, "*");
                        wrefresh(gamewin);
                        refresh();
                        curs_set(0);

                        do
                            choice = getch(); while (choice != 'r' && choice != 'R' && choice != 'q' && choice != 'Q');

                        if (choice == 'r' || choice == 'R') {
                            go_back(&campo);

                            clear();
                            print_hud(campo, bandiera);
                            print_field(campo, &gamewin);
                            curs_set(1);
                            wmove(gamewin, r, c);
                        }
                    } else /* GAME OVER */ {
                        record = get_record(config[0], config[1], config[2]);
                        
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 6, scMax / 2 - 25, "   ___                          ___                 ");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 5, scMax / 2 - 25, "  / _ \\__ _ _ __ ___   ___     /___\\__   _____ _ __ ");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 4, scMax / 2 - 25, " / /_\\/ _` | '_ ` _ \\ / _ \\   //  //\\ \\ / / _ \\ '__|");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 3, scMax / 2 - 25, "/ /_\\\\ (_| | | | | | |  __/  / \\_//  \\ V /  __/ |   ");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 2, scMax / 2 - 25, "\\____/\\__,_|_| |_| |_|\\___|  \\___/    \\_/ \\___|_|   ");
                        mvprintw(srMax / 2 - (brMax + 2) / 2 - 1, scMax / 2 - 25, "                                                   ");
                        
                        if(record >= 0)
                        {
                            mvprintw(srMax / 2 + (brMax + 2) / 2 + (brMax % 2) + 1, scMax / 2 - 10, "TEMPO MIGLIORE: %d:%02d", record / 60, record % 60);
                        }
                        else
                        {
                            mvprintw(srMax / 2 + (brMax + 2) / 2 + (brMax % 2) + 1, scMax / 2 - 12, "TEMPO MIGLIORE: NESSUNO");
                        }

                        mvprintw(srMax / 2 + (brMax + 2) / 2 + (brMax % 2) + 5, scMax / 2 - 16, "Premi N per una nuova partita ..");

                        curs_set(0);
                        massive_uncover_cells(&campo);
                        print_field(campo, &gamewin);
                        refresh();

                        do
                            choice = getch(); while (choice != 'q' && choice != 'Q' && choice != 'n' && choice != 'N');

                        if (choice == 'n' || choice == 'N') {
                            new_game = 1;
                            choice = 'q';
                        }
                    }
                }
            }
        }

    } while (choice != 'q' && choice != 'Q');

    purge(campo);
    create_file_field(config[0], config[1], config[2]);
    free(config);

    if (new_game)
        return 4;
    else
        return 1;
}

/* Schermata che viene visualizzata dopo aver premuto su inizia partita 
 * e serve a chiedere all'utente quante possibilità di ritorno vuole.
 * 
 * Alla fine la funzione ritorna il numero scelto dall'utente che va da 0 a 99.
 */
int screen_return() {
    /* VARIABLES */
    int srMax, scMax;
    int temp = 0, temp1 = 0;
    char s1[] = "Quante volte vuoi tornare indietro?";
    char s2[] = "Premi INVIO per iniziare ..";

    /* SCREEN SETTINGS */
    initscr();
    clear();
    curs_set(1);
    getmaxyx(stdscr, srMax, scMax);

    mvprintw(srMax / 2 - 2, scMax / 2 - strlen(s1) / 2, s1);
    mvprintw(srMax - 1, 0, "INVIO: NESSUNA");
    refresh();

    wmove(stdscr, srMax / 2 - 1, scMax / 2 - 1);

    /* FIRST NUMBER */
    do {
        temp = getch();
        if (temp == 10)
            return 0;
    } while (temp < 49 || temp > 57);

    mvprintw(srMax - 1, 0, "              ");
    wmove(stdscr, srMax / 2 - 1, scMax / 2 - 1);
    temp -= 48;
    printw("%d", temp);

    /* SECOND NUMBER */
    do {
        temp1 = getch();
        if (temp1 >= 48 && temp1 <= 57) {
            temp *= 10;
            printw("%d", temp1 - 48);
            temp += temp1 - 48;
        }
    } while ((temp1 < 48 || temp1 > 57) && (temp1 != 10));

    curs_set(0);
    mvprintw(srMax / 2 + 1, scMax / 2 - strlen(s2) / 2, s2);
    refresh();

    /* ENTER WAITING */
    while (temp1 != 10)
        temp1 = getch();

    return temp;
}

/* Questo metodo viene invocato soltanto quando le righe e/o colonne
 * del campo scelto sono troppo grandi per essere stampati su video.
 * 
 * Metodo anti-buggamento :)
 */
int screen_err() {
    /* VARIABLES */
    int srMax, scMax;
    char s1[] = "ERRORE DIMENSIONE FINESTRA";
    char s2[] = "Aumentare la dimensione della finestra.";
    char s3[] = "Premi un tasto per tornare indietro ..";

    /* SCREEN SETTINGS */
    initscr();
    clear();
    noecho();
    curs_set(0);
    getmaxyx(stdscr, srMax, scMax);

    /* PRINTERS */
    mvprintw(srMax / 2 - 1, scMax / 2 - strlen(s1) / 2, s1);
    mvprintw(srMax / 2, scMax / 2 - strlen(s2) / 2, s2);
    mvprintw(srMax - 1, 0, s3);

    getch();

    /* NEXT WINDOW */
    return 1;
}
