#include "structures_management.h"
#ifndef FIELD_MANAGEMENT_H
#define FIELD_MANAGEMENT_H

int create_file_field(int num_rows, int num_columns, int tot_mine);
int create_ingame_field(Field_manager *my_field, int num_rows, int num_columns);
int * get_current_settings();
int uncover_cells(Field_manager *my_field, int pos_row, int pos_column);
void go_back(Field_manager *my_field);
void massive_uncover_cells(Field_manager *my_field);
void purge(Field_manager my_field);

#endif

