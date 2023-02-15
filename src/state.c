#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state()
{
  // TODO: Implement this function.

  snake_t firstSnake;
  firstSnake.tail_row = 2;
  firstSnake.tail_col = 2;
  firstSnake.head_row = 2;
  firstSnake.head_col = 4;
  firstSnake.live = true;

  game_state_t *gameState = malloc(sizeof(game_state_t));
  gameState->num_rows = 18;
  gameState->num_snakes = 1;
  gameState->snakes = malloc(sizeof(snake_t) * (gameState->num_snakes));
  gameState->snakes[0] = firstSnake;

  // allocate memory of row and column of a board.
  unsigned int num_cols = 20;
  gameState->board = malloc(sizeof(char *) * ((gameState->num_rows))); // Statement 1
  for (int i = 0; i < gameState->num_rows; i++)
  {
    gameState->board[i] = malloc(sizeof(char) * (num_cols + 1));
  }
  // initialize a board
  for (int i = 0; i < gameState->num_rows; i++)
  {
    for (int j = 0; j < num_cols + 1; j++)
    {
      if (j == num_cols)
      {
        gameState->board[i][j] = '\0';
      }
      else if (i == 0 || i == (gameState->num_rows) - 1 || j == 0 || j == num_cols - 1)
      {
        strcpy(&(gameState->board[i][j]), "#");
      }
      else if (i == 2 && j == 2)
      {
        strcpy(&(gameState->board[i][j]), "d");
      }
      else if (i == 2 && j == 3)
      {
        strcpy(&(gameState->board[i][j]), ">");
      }
      else if (i == 2 && j == 4)
      {
        strcpy(&(gameState->board[i][j]), "D");
      }
      else if (i == 2 && j == 9)
      {
        strcpy(&(gameState->board[i][j]), "*");
      }
      else
      {
        strcpy(&(gameState->board[i][j]), " ");
      }
    }
  }

  return gameState;
}

/* Task 2 */
void free_state(game_state_t *state)
{
  // TODO: Implement this function.

  free(state->snakes);

  for (int i = 0; i < state->num_rows; i++)
  {
    free(state->board[i]);
  }

  free(state->board);

  free(state);

  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp)
{
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++)
  {
    fprintf(fp, "%s", state->board[i]);
    fprintf(fp, "\n");
  }

  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename)
{
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col)
{
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch)
{
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c)
{
  // TODO: Implement this function.
  // char tailString[5] = {'w', 'a', 's', 'd', '\0'};

  // for (int i = 0; tailString[i] != '\0'; i++)
  // {
  //   if (tailString[i] == c)
  //   {
  //     return true;
  //   }
  // }

  // return false;

  if (c != 'w' && c != 'a' && c != 's' && c != 'd')
  {
    return false;
  }

  return true;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c)
{
  // TODO: Implement this function.
  if (c != 'W' && c != 'A' && c != 'S' && c != 'D')
  {
    return false;
  }
  return true;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c)
{
  // TODO: Implement this function.
  if (c != 'W' && c != 'A' && c != 'S' && c != 'D' && c != 'w' && c != 'a' && c != 's' && c != 'd' && c != '^' && c != '<' && c != 'v' && c != '>' && c != 'x')
  {
    return false;
  }

  return true;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c)
{
  // TODO: Implement this function.
  if (c == '^')
  {
    return 'w';
  }
  if (c == '<')
  {
    return 'a';
  }
  if (c == 'v')
  {
    return 's';
  }
  if (c == '>')
  {
    return 'd';
  }

  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c)
{
  // TODO: Implement this function.
  if (c == 'W')
  {
    return '^';
  }
  if (c == 'A')
  {
    return '<';
  }
  if (c == 'S')
  {
    return 'v';
  }
  if (c == 'D')
  {
    return '>';
  }

  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c)
{
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S')
  {
    return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W')
  {
    return cur_row - 1;
  }

  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c)
{
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D')
  {
    return cur_col + 1;
  }
  else if (c == '<' || c == 'a' || c == 'A')
  {
    return cur_col - 1;
  }

  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  unsigned int head_row = state->snakes[snum].head_row;
  unsigned int head_col = state->snakes[snum].head_col;

  char head_char = get_board_at(state, head_row, head_col);

  if (head_char == 'W')
  {
    return get_board_at(state, head_row - 1, head_col);
  }
  if (head_char == 'S')
  {
    return get_board_at(state, head_row + 1, head_col);
  }
  if (head_char == 'A')
  {
    return get_board_at(state, head_row, head_col - 1);
  }
  if (head_char == 'D')
  {
    return get_board_at(state, head_row, head_col + 1);
  }

  return '?';
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  snake_t *snake = &(state->snakes[snum]);

  char head_char = get_board_at(state, snake->head_row, snake->head_col);

  if (head_char == 'W')
  {
    set_board_at(state, snake->head_row, snake->head_col, '^');
    set_board_at(state, snake->head_row - 1, snake->head_col, 'W');
    snake->head_row = snake->head_row - 1;
  }
  if (head_char == 'S')
  {
    set_board_at(state, snake->head_row, snake->head_col, 'v');
    set_board_at(state, snake->head_row + 1, snake->head_col, 'S');
    snake->head_row = snake->head_row + 1;
  }
  if (head_char == 'A')
  {
    set_board_at(state, snake->head_row, snake->head_col, '<');
    set_board_at(state, snake->head_row, snake->head_col - 1, 'A');
    snake->head_col = snake->head_col - 1;
  }
  if (head_char == 'D')
  {
    set_board_at(state, snake->head_row, snake->head_col, '>');
    set_board_at(state, snake->head_row, snake->head_col + 1, 'D');
    snake->head_col = snake->head_col + 1;
  }
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  snake_t *snake = &(state->snakes[snum]);

  char tail_char = get_board_at(state, snake->tail_row, snake->tail_col);
  unsigned int last_body_row = snake->tail_row;
  unsigned int last_body_col = snake->tail_col;

  if (tail_char == 'w')
  {
    last_body_row = last_body_row - 1;
  }
  if (tail_char == 's')
  {
    last_body_row = last_body_row + 1;
  }
  if (tail_char == 'a')
  {
    last_body_col = last_body_col - 1;
  }
  if (tail_char == 'd')
  {
    last_body_col = last_body_col + 1;
  }

  char last_body_char = get_board_at(state, last_body_row, last_body_col);

  if (last_body_char == '^')
  {
    set_board_at(state, last_body_row, last_body_col, 'w');
  }
  if (last_body_char == 'v')
  {
    set_board_at(state, last_body_row, last_body_col, 's');
  }
  if (last_body_char == '<')
  {
    set_board_at(state, last_body_row, last_body_col, 'a');
  }
  if (last_body_char == '>')
  {
    set_board_at(state, last_body_row, last_body_col, 'd');
  }

  set_board_at(state, snake->tail_row, snake->tail_col, ' ');
  snake->tail_row = last_body_row;
  snake->tail_col = last_body_col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state))
{
  // TODO: Implement this function.

  for (unsigned int i = 0; i < state->num_snakes; i++)
  {
    snake_t *snake = &(state->snakes[i]);

    char head_char = get_board_at(state, snake->head_row, snake->head_col);
    char next_head_char;
    unsigned int next_head_row = snake->head_row;
    unsigned int next_head_col = snake->head_col;

    if (head_char == 'W')
    {
      next_head_row = next_head_row - 1;
    }
    if (head_char == 'S')
    {
      next_head_row = next_head_row + 1;
    }
    if (head_char == 'A')
    {
      next_head_col = next_head_col - 1;
    }
    if (head_char == 'D')
    {
      next_head_col = next_head_col + 1;
    }

    next_head_char = get_board_at(state, next_head_row, next_head_col);

    if (next_head_char != '*' && next_head_char != ' ')
    {

      // update_head(state, i);
      // update_tail(state, i);
      set_board_at(state, snake->head_row, snake->head_col, 'x');
      snake->live = false;
    }
    else if (next_head_char == '*')
    {
      update_head(state, i);
      add_food(state);
    }
    else if (next_head_char == ' ')
    {
      update_head(state, i);
      update_tail(state, i);
    }
  }
  return;
}

/* Task 5 */
game_state_t *load_board(char *filename)
{
  // TODO: Implement this function.

  FILE *file = fopen(filename, "r");

  char ch;
  unsigned int num_rows = 0;
  while ((ch = fgetc(file)) != EOF)
  {
    printf("%c", ch);
    if (ch == '\n')
    {
      num_rows += 1;
    }
  }

  char **board = malloc(sizeof(char *) * num_rows);

  unsigned int row_index = 0;
  unsigned int col_index = 0;

  rewind(file);

  while ((ch = fgetc(file)) != EOF)
  {
    if (ch == '\n')
    {
      board[row_index] = realloc(board[row_index], sizeof(char) * (col_index + 1));
      board[row_index][col_index] = '\0';
      row_index += 1;
      col_index = 0;
    }
    else
    {
      if (row_index == 0)
      {
        board[0] = malloc(sizeof(char) * 2);
      }
      else
      {
        board[row_index] = realloc(board[row_index], sizeof(char) * (col_index + 2));
      }
      strcpy(&(board[row_index][col_index]), ch);
      // board[row_index][col_index] = ch;
      // board[row_index][col_index + 1] = '\0';
      col_index += 1;
    }
  }

  fclose(file);

  // game_state_t *gameState = create_default_state();
  // gameState->board = board;
  // gameState->num_rows = num_rows;
  // gameState->num_snakes = 0;
  // gameState->snakes = NULL;

  // return gameState;

  return;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum)
{
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state)
{
  // TODO: Implement this function.
  return NULL;
}
