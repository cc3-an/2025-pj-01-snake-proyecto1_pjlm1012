#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Tarea 1 */
game_state_t* create_default_state() {
  // TODO: Implementar esta funcion.
  const char *tablero[] = {
    "####################",
    "#                  #",
    "# d>D    *         #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "####################"
  };
  //creo variable de game state
  game_state_t tab;
  tab.num_rows = 18;
  tab.num_snakes = 1;
  
  //meter tablero en la memoria
  tab.board = malloc(tab.num_rows* sizeof(char*));
  for(int i = 0; i<tab.num_rows; i++){
    tab.board[i] = malloc(21* sizeof(char)); //21 columnas
    strcpy(tab.board[i], tablero[i]);
  }

  //hacer snake
  tab.snakes = malloc(sizeof(snake_t));
  tab.snakes[0].tail_row = 2;
  tab.snakes[0].tail_col = 2;
  tab.snakes[0].head_row = 2;
  tab.snakes[0].head_col = 4;
  tab.snakes[0].live = true;

  //copiar y devolver el estado
  game_state_t *state = malloc(sizeof(game_state_t));
  *state = tab;
  return state;

}


/* Tarea 2 */
void free_state(game_state_t* state) {
  // TODO: Implementar esta funcion.
  for(int i=0; i<state->num_rows; i++){//borra el tablero segun el num de rows
    free(state->board[i]);
  }
  free(state->board);//limpio board
  free(state->snakes);//limpio la snake
  free(state);//limpio la estructura

  return;
}


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  for(int i=0; i<state->num_rows;i++){
    fprintf(fp, "%s\n", state->board[i]);//imprime el string y newline
  }
}


/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Tarea 4.1 */


/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  // TODO: Implementar esta funcion.
  if(c=='w'||c=='a'||c=='s'|| c=='d'){
  return true;
  }else{
    return false;
  }
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  // TODO: Implementar esta funcion.
  if(c=='W'||c=='A'||c=='S'|| c=='D'|| c=='x'){
    return true;
    }else{
      return false;
    }
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implementar esta funcion.
  if(is_tail(c)==true || is_head(c)==true){
    return true;
  }else{
    return false;
  }
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implementar esta funcion.
  if(c=='^'){
    return 'w';
  }else if(c=='<'){
    return 'a';
  }else if(c=='v'){
    return 's';
  }else if(c=='>'){
    return 'd';
  }else{
    return '?';
  }
  
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implementar esta funcion.
  if(c=='W'){
    return '^';
  }else if(c=='A'){
    return '<';
  }else if(c=='S'){
    return 'v';
  }else if(c=='D'){
    return '>';
  }else{
    return '?';
  }
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implementar esta funcion.
  if (c == 'v' || c == 's' || c == 'S'){
    return cur_row+1;
  }else if(c == '^' || c == 'w' || c == 'W'){
    return cur_row-1;
  }else{
    return cur_row;
  }
}


/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implementar esta funcion.
  if (c == '>' || c == 'd' || c == 'D'){
    return cur_col+1;
  }else if(c == '<' || c == 'a' || c == 'A'){
    return cur_col-1;
  }else{
    return cur_col;
  }
}


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t copia_snake = state->snakes[snum];
  //ubicacion de cabeza
  int cabeza_row = copia_snake.head_row;
  int cabeza_col = copia_snake.head_col;
  //sacar char de la cabeza
  char cabeza_char = get_board_at(state,cabeza_row,cabeza_col);
  //pasar de cabeza a cuerpo
  char cabeza_dir = head_to_body(cabeza_char);
  //actualizar
  int next_row = get_next_row(cabeza_row, cabeza_dir);
  int next_col = get_next_col(cabeza_col, cabeza_dir);

  return get_board_at(state, next_row, next_col);
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t *snake = &state->snakes[snum];//se usa puntero para poder modificar la serpiente en su dir de memoria
  unsigned int row = snake->head_row;
  unsigned int col = snake->head_col;

  char cabeza = get_board_at(state, row, col);
  char cuerpo = head_to_body(cabeza);
  //actualizar tablero
  set_board_at(state,row,col,cuerpo);

  unsigned int n_row = get_next_row(row, cuerpo);
  unsigned int n_col = get_next_col(col, cuerpo);
  //actualizar cabeza
  set_board_at(state, n_row, n_col, cabeza);

  snake->head_row = n_row;
  snake->head_col = n_col;
}


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t *snake = &state->snakes[snum];//se usa puntero para poder modificar la serpiente en su dir de memoria
  unsigned int row = snake->tail_row;
  unsigned int col = snake->tail_col;
  char cola = get_board_at(state, row, col);//obtengo el chasr de la cola
  //borro la cola actual
  set_board_at(state, row, col, ' ');

  unsigned int n_row = get_next_row(row, cola);
  unsigned int n_col = get_next_col(col, cola);

  //obntengo el cuerpo y luego lo cambio a cola
  char cuerpo = get_board_at(state, n_row, n_col);
  char cola_nueva = body_to_tail(cuerpo);

  set_board_at(state,n_row,n_col,cola_nueva);



  snake->tail_row = n_row;
  snake->tail_col = n_col;


}

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implementar esta funcion.
  for (unsigned int snum = 0; snum < state->num_snakes; snum++){//recorro las serpientes desde 0
    //obtengo la snake 
    snake_t *snake = &state->snakes[snum];


    //ver si esta muerta
    char muerta = get_board_at(state,snake->head_row, snake->head_col);
    if (muerta != 'x')
    {
      char cabeza_char = muerta;
      //esta viva
      unsigned int row = snake->head_row;
      unsigned int col = snake->head_col;
      int n_row = get_next_row(snake->head_row,cabeza_char);
      int n_col = get_next_col(snake->head_col,cabeza_char);


      char sig_mov = get_board_at(state,n_row,n_col);

      //gameplay 
      char Death = 'x';
      if(sig_mov == '*'){//come fruta
        update_head(state,snum);
        add_food(state);
      }else if(sig_mov != ' ' && sig_mov != '*'){//se muere si toca # o cualquier cosa que no sea espacio
        set_board_at(state, snake->head_row, snake->head_col, Death);
        snake->live = false;
      }else if(sig_mov == '#'){
        set_board_at(state, snake->head_row, snake->head_col, Death);
        snake->live = false;
      }
      else{//movimiento en mapa
        update_head(state,snum);
        update_tail(state,snum);
        
      }
    }else{
      //esta muerta
    }
  }  
}

/* Tarea 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implementar esta funcion.
  #define FILAS_MAX 100
  #define LARGO_MAX 1024

  FILE* file = fopen(filename, "r");
  if (file == NULL) {
      return NULL;
  }

  char buffer[LARGO_MAX];
  char** board = malloc(FILAS_MAX * sizeof(char*));//puntero de puntero al ser un arreglo de strings
  int filas = 0;

  while (fgets(buffer, sizeof(buffer), file) && filas < FILAS_MAX) {
      size_t len = strlen(buffer);

      // quitar los newline
      if (len > 0 && buffer[len - 1] == '\n') {
          buffer[len - 1] = '\0';
          len--;
      }

      board[filas] = malloc(len + 1);
      strcpy(board[filas], buffer);
      filas++;
  }

  fclose(file);

  game_state_t* state = malloc(sizeof(game_state_t));
  state->num_rows = filas;
  state->board = board; 
  state->num_snakes = 0;
  state->snakes = NULL;

  return state;
}



/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/

static void find_head(game_state_t* state, unsigned int snum) {
  snake_t* snake = &state->snakes[snum];  
  int row = snake->tail_row;  
  int col = snake->tail_col;

  char ch = get_board_at(state, row, col);  
  char dir;
//body a dir
  if (ch == 'w') {
    dir = '^';
  } else if (ch == 'a') {
    dir = '<';
  } else if (ch == 's') {
    dir = 'v';
  } else if (ch == 'd') {
    dir = '>';
  } else {
    return;  
  }

  // bucle para encontrar la cabeza
  while (1) {
    row = get_next_row(row, dir);
    col = get_next_col(col, dir);

    ch = get_board_at(state, row, col);

    if (is_head(ch) == true) {
      snake->head_row = row;
      snake->head_col = col;
      return;
    }
    if (ch == '^' || ch == '<' || ch == 'v' || ch == '>') {
      dir = ch;  
    } else {
      return; 
    }
  }
}




/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implementar esta funcion.
  return NULL;
}
