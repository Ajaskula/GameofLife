#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#ifndef WIERSZE
#define WIERSZE 22
#endif

#ifndef KOLUMNY
#define KOLUMNY 80
#endif

// function returns absolute value of an int number
int abs(int a) {

  if (a > 0) {

    return a;

  } else {

    return -a;
  }
}

// struct which keeps inforamtion about cell coordinates (row, column)
struct cell {

  int row;
  int column;
};

struct hash_set {
  struct cell *hash_table;
  int table_size;
  int element_count;
};

// function takes row and column as coordinates of a cell
// and convert them into value due to which this cell is stored in a hash_table
uint64_t hash_function(const struct cell *cell);

// function reads instructions from standard input and due to them performs
// accurate operations
bool read_instructions_and_and_perform_operations(
    int *row_number, int *column_number, struct hash_set *set);

// function takes all live cells from a hash table and store them into an arrey
struct cell *clone_alive_cells(const struct hash_set *set, int *result_size);

// function firstly compares cells according to their rows values
// and than compares their columns values
int compare_cells(const void *p1, const void *p2);

// function reads population of live cells from standard input
// and stores it in a hash table
void read_population(struct hash_set *set);

// displays part of cell's population in a window of size(WIERSZE, KOLUMNY)
// displays "0" if cell is alive and "." if it's not
void display_windows(struct hash_set *set, int start_column, int start_row);

// read one line of instractions from a standard input
void readline(struct hash_set *set);

// creates new generation of cells according to rules specified in a task
void create_new_generation(struct hash_set *set);

// check number of neighbours of cells which are in a surrounding of a given
// cell
void check_surroundings(int cell_row, int cell_column, const struct hash_set *set, struct hash_set *new_set);

// counts number of alive cells stored in a hash table
int count_alive_cells(const struct hash_set *set);

// function checks how many alive neighbours a given cell has
int count_neighbours(int cell_row, int cell_column, const struct hash_set *set);

// function writes out a description of accurate population according to task
// specification
void print_population(const struct hash_set *set);

// function initialize empty hash table
struct hash_set *hash_set_init(int table_size);

// function insert cell with given row and column into a hash table
void hash_set_insert(struct hash_set *set, struct cell cell);

// function resizes hash table and makes it two times bigger
void hash_set_resize(struct hash_set *set, int new_size);

// function check if cell with given coordinates(row, collum) is stored in a
// hash table
bool hash_set_lookup(const struct hash_set *haystack, const struct cell *needle);

void hash_set_free(struct hash_set *set);

void hash_set_move(struct hash_set *dst, struct hash_set *src);

bool population_has_cell_at_pos(const struct hash_set *set, int row, int column);

float hash_set_load_factor(const struct hash_set *set);

bool hash_set_entry_present(const struct hash_set *set, int idx);

int main() {

  // orginal size of a hash table
  int table_size = 10;

  struct hash_set *set = hash_set_init(table_size);
  read_population(set);

  // orginal coordinates of left, top corner of displaying window
  int row_number = 1;
  int column_number = 1;
  bool should_continue = true;

  do {
    display_windows(set, column_number, row_number);
    should_continue = read_instructions_and_and_perform_operations(&row_number, &column_number, set);

  } while (should_continue);
  
  hash_set_free(set);

  return 0;
}

bool read_instructions_and_and_perform_operations(
    int *row_number, int *column_number, struct hash_set *set) {

  // depending on what is found in a instruction line
  // specific operation is perform
  char a = (char)getchar();

  if (a == '\n') {
    create_new_generation(set);
    return true;
  }

  // finish work of the program
  if (a == '.') {
    return false;
  }
  char b = (char)getchar();

  // writes out description of population
  if (a == '0' && b == '\n') {

    print_population(set);
    return true;

    // change coordinates of left top corner of displaying window or calculate
    // n-th generation
  } else {

    ungetc(b, stdin);
    ungetc(a, stdin);

    int number_from_input;

    if (!scanf("%d", &number_from_input)) {

      printf("Cannot read number_from_input");
    }
    a = (char)getchar();

    // change coordinates of left top corner of displaying window
    if (a == ' ') {
      int first_coordinate = number_from_input;
      int second_coordinate;
      if (!scanf("%d", &second_coordinate)) {

        printf("Cannot read second_coordinate");
      }
      *row_number = first_coordinate;
      *column_number = second_coordinate;
      getchar();
      return true;

      // calculate n-th generation
    } else {

      int number_of_generation = number_from_input;
      while (number_of_generation > 0) {

        create_new_generation(set);
        number_of_generation--;
      }
      return true;
    }
  }
}

void read_population(struct hash_set *set) {

  char a = (char)getchar();
  char b = (char)getchar();

  // reads all alive cells from a line and puts it into hash_table
  while (a != '/' || b != '\n') {

    ungetc(b, stdin);

    readline(set);

    a = (char)getchar();
    b = (char)getchar();
  }
}

void display_windows(struct hash_set *set, int start_column, int start_row) {

  for (int row = start_row; row < WIERSZE + start_row; row++) {

    for (int column = start_column; column < KOLUMNY + start_column;
         column++) {

      if (population_has_cell_at_pos(set, row, column)) {

        printf("0");
      } else {
        printf(".");
      }
    }
    printf("\n");
  }
  // writes separation line after writing w window
  for (int i = 0; i < KOLUMNY; i++) {

    printf("=");
  }
  printf("\n");
}

void readline(struct hash_set *set) {

  int row;

  if (!scanf("%d", &row)) {

    printf("Cannot read row");
  }

  // inserts all alive cells from a line into a hash_table
  while (getchar() != '\n') {

    int column;
    if (scanf("%d", &column)) {
			struct cell cell = {
				.row = row,
				.column = column
			};
      hash_set_insert(set, cell);

    } else {

      printf("Cannot read column");
    }
  }
}
int population = 0;

void create_new_generation(struct hash_set *set) {
  // printf("Generating new population: %d, table_size: %d, elemnents: %d\n", population++, set->table_size, set->element_count);
  int num_elements = set->table_size;

  struct hash_set *new_set = hash_set_init(set->table_size);

  // in this loop program visits all alive cells in a hash table, and check
  // their surroundings in order to callculate which cells from it will be alive
  // in next generation
  for (int i = 0; i < num_elements; i++) {
    if (hash_set_entry_present(set, i)) {
    	check_surroundings(set->hash_table[i].row, set->hash_table[i].column, set, new_set);
    }
  }
  
  hash_set_move(set, new_set);
  new_set = NULL;
}
void check_surroundings(int cell_row, int cell_column, const struct hash_set *set, struct hash_set *new_set) {

  // every time we check if number of neighbours for a given cell == 3 and the
  // cell is dead or if cell has two neighbours and is currently alive if
  // conditions are met we insert this cell into new hash tbale
  for (int row = cell_row - 1; row <= cell_row + 1; row++) {
    for (int column = cell_column - 1; column <= cell_column + 1; column++) {
    	if (population_has_cell_at_pos(new_set, row, column)) {
    		// Cell is already present in the new table - we can skip it as it was definitely already processed
      	continue;
    	}
    	int neighbours = count_neighbours(row, column, set);
    	int is_currently_alive = population_has_cell_at_pos(set, row, column);
    	
    	struct cell new_cell = {
    		.row = row,
    		.column = column
    	};
    	if (is_currently_alive && (neighbours == 2 || neighbours == 3)) {
				hash_set_insert(new_set, new_cell);
    	} else if (!is_currently_alive && neighbours == 3) {
		 		hash_set_insert(new_set, new_cell);
    	}
    }
  }
}

int count_neighbours(int cell_row, int cell_column, const struct hash_set *set) {

  int counter = 0;
  // check if specfied neighbour of a given cell is alive (stored in a hash
  // table)
  
  for (int row = cell_row - 1; row <= cell_row + 1; row++) {
  	for (int column = cell_column - 1; column <= cell_column + 1; column++) {
  		if (row == cell_row && column == cell_column) {
  			continue;
  		}
  		if (population_has_cell_at_pos(set, row, column)) {
  			counter++;
  		}
  	}
  }

  return counter;
}

void print_population(const struct hash_set *set) {

  int num_values = 0;
  // store only alive cells in a table values
  struct cell *values =
      clone_alive_cells(set, &num_values);
  qsort(values, (long unsigned int)num_values, sizeof(struct cell),
        compare_cells);

  int i = 0;
  // display description of given population
  while (i < num_values) {
    int remember = values[i].row;
    printf("/%d", remember);
    while (i < num_values && values[i].row == remember) {

      printf(" %d", values[i].column);
      i++;
    }
    printf("\n");
  }
  printf("/\n");

  free(values);
  values = NULL;
}
int compare_cells(const void *p1, const void *p2) {

  struct cell cell1 = *(struct cell *)p1;
  struct cell cell2 = *(struct cell *)p2;

  if (cell1.row < cell2.row)
    return -1;
  if (cell1.row > cell2.row)
    return 1;

  if (cell1.column < cell2.column)
    return -1;

  if (cell1.column > cell2.column)
    return 1;

  return 0;
}

struct hash_set *hash_set_init(int table_size) {

  struct cell *hash_table = (struct cell *)malloc(
      sizeof(struct cell) * (long unsigned int)table_size);

  for (int i = 0; i < table_size; i++) {

    // if no cell is stored in a specific cell of an arrey
    // stored values are as below
    hash_table[i].row = INT_MAX;
    hash_table[i].column = INT_MAX;
  }
  
  struct hash_set *set = (struct hash_set *)malloc(sizeof(struct hash_set));
  set->hash_table = hash_table;
  set->table_size = table_size;
  set->element_count = 0;

  return set;
}

void hash_set_free(struct hash_set *set) {
	free(set->hash_table);
	free(set);
}

void hash_set_move(struct hash_set *dst, struct hash_set *src) {
	free(dst->hash_table);
	*dst = *src;
	free(src);
}

void hash_set_insert(struct hash_set *set, struct cell cell) {

  // calculates index under which we want to store given cell
  int index = (int)(hash_function(&cell) % (uint64_t) set->table_size);
  // we find first free place to store our cell
  for (int i = 0; i < set->table_size; i++) {
    // index under wich we try to store given cell
    int try = ((i + index) % (set->table_size));
    if (compare_cells(&set->hash_table[try], &cell) == 0) {
    	// entry already present in set
    	break;
    }
    if (!hash_set_entry_present(set, try)) {
    	set->hash_table[try] = cell;
    	set->element_count++;
    	break;
    }
  }
  // resizes hash table 2 times
  if (hash_set_load_factor(set) > 0.75) {
		int new_size = set->table_size * 3;
		hash_set_resize(set, new_size);
  }
}

void hash_set_resize(struct hash_set *set, int new_size) {
  if (set->element_count > new_size) {
  	// Not enough space for all elements - this indicates a bug
  	printf("Tried to resize set with %d elements to %d\n", set->element_count, new_size);
  	assert(false);
  }
  
  struct hash_set *new_set = hash_set_init(new_size);

  // inserts cells from old set into new one
  for (int i = 0; i < set->table_size; i++) {
	  if (hash_set_entry_present(set, i)) {
	  	hash_set_insert(new_set, set->hash_table[i]);
	  }
  }
  
  hash_set_move(set, new_set);
  new_set = NULL;
}

bool hash_set_lookup(const struct hash_set *haystack, const struct cell *needle) {

  // index under which cell should be stored
  int index = (int)(hash_function(needle) % (uint64_t) haystack->table_size);

  // checks if cell is stored under following indices
  for (int i = 0; i < haystack->table_size; i++) {

    int try = (index + i) % haystack->table_size;
    if (haystack->hash_table[try].row == needle->row 
     && haystack->hash_table[try].column == needle->column) {
      return true;
    }
    // gives information, that cell is not stored in a hash table
    if (!hash_set_entry_present(haystack, try)) {
      return false;
    }
  }

  return false;
}

struct cell *clone_alive_cells(const struct hash_set *set, int *result_size) {
	*result_size = count_alive_cells(set);
  struct cell *values = (struct cell *)malloc(sizeof(struct cell) * ((size_t)*result_size));
  
  int result_idx = 0;
  for (int i = 0; i < set->table_size; i++) {
    // if this place of hash_table stores a cell, we store it into values arrey
    if (hash_set_entry_present(set, i)) {
    	values[result_idx] = set->hash_table[i];
    	result_idx++;
    }
  }

  return values;
}

uint64_t hash_function(const struct cell *cell) {
  return (((uint64_t) abs(cell->row)) << 32) + (uint64_t) abs(cell->column);
}

int count_alive_cells(const struct hash_set *set) {
	return set->element_count;
}

bool hash_set_entry_present(const struct hash_set *set, int idx) {
	return set->hash_table[idx].row != INT_MAX && set->hash_table[idx].column != INT_MAX;
}

float hash_set_load_factor(const struct hash_set *set) {
	return (float)set->element_count / (float)set->table_size;
}

bool population_has_cell_at_pos(const struct hash_set *set, int row, int column) {
	struct cell cell = {
		.row = row,
		.column = column
	};
	
	return hash_set_lookup(set, &cell);
}

