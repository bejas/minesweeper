#include <ncurses.h>
#include "field_management.h"
#ifndef GRAPHICS_MANAGEMENT_H
#define GRAPHICS_MANAGEMENT_H

void campo_minato();
int menu_iniziale();
int impostazioni();
void print_field(Field_manager campo, WINDOW** gamewin);
int screen_return();
void print_hud(Field_manager campo, int bandiera);
int game();
int screen_return();
int screen_err();


#endif
