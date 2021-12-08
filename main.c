#include <stdio.h>
#include <ncurses.h>

#define DIM_Y 3
#define DIM_X 3
#define AI_ENABLED 1

#define gtermy(gridy) ((gridy) * (DIM_Y - 1) + 1)
#define gtermx(gridx) ((gridx) * (DIM_X - 1) + 1)

// 0 is not played
// 1 is player 1
// 2 is player 2 (ai is always player 2)
int grid[DIM_Y][DIM_X] = {0};
int px = 0, py = 0;
int player = 1;
int game_ended = 0;

int check_arrow();
void handle_arrow(int);
// returns 0 if it was set successfully, 1 if it failed to set.
int handle_player_set();

void draw_blank_grid();
void fill_grid();
void draw_game_status();
void draw();

// returns 0 if nobody has won
// 1 if player 1 has won
// 2 if player 2 has won
int get_winner();

int check_arrow()
{
  int key = getchar();
  if (key == 0x5B)
  {
    key = getchar();
    switch (key)
    {
    case 0x41:
      // up
      return 1;
    case 0x42:
      // down
      return 2;
    case 0x43:
      // right
      return 3;
    case 0x44:
      // left
      return 4;
    }
  }
  return 0;
}

void handle_arrow(int arrow)
{
  switch (arrow)
  {
  case 1:
    // up
    if (py != 0)
      py--;
    break;
  case 2:
    // down
    if (py != DIM_Y - 1)
      py++;
    break;
  case 3:
    // right
    if (px != DIM_X - 1)
      px++;
    break;
  case 4:
    // left
    if (px != 0)
      px--;
    break;
  }
}

void draw_blank_grid()
{
  clear();

  // Vertical
  move(0, 0);
  vline(ACS_VLINE, 7);
  move(0, 2);
  vline(ACS_VLINE, 7);
  move(0, 4);
  vline(ACS_VLINE, 7);
  move(0, 6);
  vline(ACS_VLINE, 7);

  // Horizontal
  move(0, 1);
  hline(ACS_HLINE, 5);
  move(2, 1);
  hline(ACS_HLINE, 5);
  move(4, 1);
  hline(ACS_HLINE, 5);
  move(6, 1);
  hline(ACS_HLINE, 5);

  // Corners and middle points
  // Left wall
  move(0, 0);
  addch(ACS_ULCORNER);
  move(2, 0);
  addch(ACS_LTEE);
  move(4, 0);
  addch(ACS_LTEE);
  move(6, 0);
  addch(ACS_LLCORNER);
  // Middle wall 1
  move(0, 2);
  addch(ACS_TTEE);
  move(2, 2);
  addch(ACS_PLUS);
  move(4, 2);
  addch(ACS_PLUS);
  move(6, 2);
  addch(ACS_BTEE);
  // Middle wall 2
  move(0, 4);
  addch(ACS_TTEE);
  move(2, 4);
  addch(ACS_PLUS);
  move(4, 4);
  addch(ACS_PLUS);
  move(6, 4);
  addch(ACS_BTEE);
  // Right wall
  move(0, 6);
  addch(ACS_URCORNER);
  move(2, 6);
  addch(ACS_RTEE);
  move(4, 6);
  addch(ACS_RTEE);
  move(6, 6);
  addch(ACS_LRCORNER);

  // refresh();
}
void fill_grid()
{
  for (int y = 0; y < DIM_Y; y++)
  {
    for (int x = 0; x < DIM_X; x++)
    {
      // 0, 0 -> 1, 1
      // 0, 1 -> 1, 3
      // 0, 2 -> 1, 5
      // 1, 0 -> 3, 1
      // 1, 1 -> 3, 3
      // 1, 2 -> 3, 5
      move(gtermy(y), gtermx(x));
      switch (grid[y][x])
      {
      case 0:
        // addch('*');
        break;
      case 1:
        addch('o');
        break;
      case 2:
        addch('x');
        break;
      }
    }
  }
  // refresh();
}

void draw_game_status()
{
  if (game_ended)
  {
    move(1, 9);
    addstr("WINNER: ");
    switch (get_winner())
    {
    case 1:
      addch('o');
      break;
    case 2:
      addch('x');
      break;
    }
  }
  else
  {
    move(1, 9);
    addstr("Now playing: ");
    switch (player)
    {
    case 1:
      addch('o');
      break;
    case 2:
      addch('x');
      break;
    }
  }
}

void draw()
{
  draw_blank_grid();
  fill_grid();
  draw_game_status();

  // move mouse to correct position
  move(gtermy(py), gtermx(px));
  refresh();
}

int handle_player_set()
{
  if (grid[py][px] != 0)
    return 1;

  // Set the selected grid item to the current player's number.
  grid[py][px] = player;

  game_ended = get_winner() != 0;

  return 0;
}

int get_winner()
{
  for (int y = 0; y < DIM_Y; y++)
  {
    int first_player_found = grid[y][0];
    for (int x = 1; x < DIM_X; x++)
    {
      if (grid[y][x] != first_player_found)
        first_player_found = -1;
    }

    if (first_player_found != -1)
      return first_player_found;
  }
  for (int x = 0; x < DIM_X; x++)
  {
    int first_player_found = grid[0][x];
    for (int y = 1; y < DIM_Y; y++)
    {
      if (grid[y][x] != first_player_found)
        first_player_found = -1;
    }
    if (first_player_found != -1)
      return first_player_found;
  }

  if (grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2])
    return grid[0][0];
  if (grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0])
    return grid[0][2];

  return 0;
}

void ai_place()
{
  for (int x = 0; x < DIM_X; x++)
  {
    for (int y = 1; y < DIM_Y; y++)
    {
      if (grid[y][x] == 0)
      {
        grid[y][x] = 2;
        return;
      }
    }
  }
}

int main(void)
{
  initscr();

  addstr("--------------------\n| tictactoe        |\n| github.com/pblop |\n--------------------\n\n");
  refresh();
  addstr("\npress any key to continue...");
  refresh();
  getch();

  while (1)
  {
    if (AI_ENABLED && player == 2)
    {
      ai_place();

      player = 1;
      game_ended = get_winner() != 0;
    }
    else
    {
      // Game loop
      int key = getchar();
      switch (key)
      {
      case 0x1B:
        endwin();
        int arrow = check_arrow();
        if (arrow != 0)
          handle_arrow(arrow);
        break;
      case ' ':
        if (game_ended == 0 && handle_player_set() == 0)
        {
          // Switch players.
          if (player == 1)
            player = 2;
          else
            player = 1;
        }
        break;
      case 'q':
        endwin();
        printf("Exited successfully.\n");
        return 0;
      }
    }

    draw();
  }

  return 0;
}