#include <printf.h>
#include <pthread.h>
#include <ncurses.h>
#include "helper_unix.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MAX_word_length 40
#define MAX_word_loaded 10
#define easy_wave 25
#define medium_wave 8
#define hard_wave 5

struct words
{
  char word[MAX_word_length];
  int number_of_inserted_letters;
  int is_ambiguous;
  int y_position;
};

int maxx, maxy,thread_exit, last_word = MAX_word_loaded - 1, difficulity = 0, wave, score = 0, tempsleep = 0, special_mode = 0, current_x[MAX_word_loaded], current_y;
long int speed;
char line[MAX_word_loaded][MAX_word_length];
struct words words[MAX_word_loaded];

void losing_screen();
void add_score();
void read_from_file(char lines[][MAX_word_length], const char file_path[30], int first_line, int last_line);
void borders(const int difficulity, const int score);
void game_play(int difficulity);
int menu();

void my_callback_on_key_arrival(char c)
{

  if (c == words[last_word].word[words[last_word].number_of_inserted_letters])
  {
    words[last_word].number_of_inserted_letters++;
  }
  if (words[last_word].number_of_inserted_letters == strlen(words[last_word].word) || (special_mode == 1 && words[last_word].number_of_inserted_letters == 3))
  {
    add_score();
    last_word--;
    system("ffplay -loglevel quiet -autoexit -nodisp menu.mp3  &");
  }
}


int main()
{

  initscr();
  getmaxyx(stdscr, maxy, maxx);
  borders(difficulity, 0);
    system("ffplay -loop 0 -loglevel quiet -autoexit -nodisp bgmusic.mp3 & ");
  difficulity = menu();
  borders(difficulity, 0);

  pthread_t thread_id = start_listening(my_callback_on_key_arrival);

  

  game_play(difficulity);

  pthread_join(thread_id, NULL);
  endwin();
  return 0;
}
void read_from_file(char lines[][MAX_word_length], const char file_path[30], int first_line, int last_line)
{
  FILE *fptr = NULL;
  int i = 0;
  fptr = fopen(file_path, "r");
  for (int i = 0; i < first_line - 1; i++)
    fgets(lines[0], MAX_word_length, fptr);
  while (fgets(lines[i], MAX_word_length, fptr))
  {
    lines[i][strlen(lines[i]) - 1] = '\0';
    i++;
    if (i == last_line - first_line + 1)
      break;
  }
}
void borders(const int difficulity, const int score)
{
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  attron(COLOR_PAIR(1) | A_DIM);
  if (difficulity != 0)
  {
    switch (difficulity)
    {
    case 1:
      mvwprintw(stdscr, 0, 2, "difficulity: Easy");
      break;
    case 2:
      mvwprintw(stdscr, 0, 2, "difficulity: Medium");
      break;
    case 3:
      mvwprintw(stdscr, 0, 2, "difficulity: Hard");
      break;
    }
    mvwprintw(stdscr, 0, maxx - 10, "score:%d", score);
    mvwprintw(stdscr, 0, maxx - 20, "wave:%d", wave);
  }
  for (int i = 0; i < maxx; i++)
  {
    move(1, i);
    printw("#");
  }
  for (int i = 1; i < maxy; i++)
  {
    move(i, 0);
    printw("#");
    move(i, maxx - 1);
    printw("#");
  }
  for (int i = 0; i < maxx; i++)
  {
    move(maxy - 1, i);
    printw("#");
  }
  refresh();
  attroff(COLOR_PAIR(1) | COLOR_PAIR(2));
}
void game_play(int difficulity)
{

  int words_count = last_word + 1;
  switch (difficulity)
  {
  case 1:
    speed = easy_wave * 1000000 / (maxy - 3);
    break;
  case 2:
    speed = medium_wave * 1000000 / (maxy - 3);
    break;
  case 3:
    speed = hard_wave * 1000000 / (maxy - 3);
    break;
  }

  current_y = -MAX_word_loaded + 2;
  srand(time(NULL));
  init_pair(2, COLOR_RED, COLOR_BLACK);

  start_color();
  while (speed * (maxy - 3) > 1000000)
  {

    read_from_file(line, "file.txt", wave * MAX_word_loaded + 1, wave * MAX_word_loaded + 10);

    for (int i = 0; i < MAX_word_loaded; i++)
    {
      strcpy(words[i].word, line[i]);
      words[i].number_of_inserted_letters = 0;
      words[i].is_ambiguous = 0;
    }
    words[rand() % ((MAX_word_loaded ) - 1) + 1].is_ambiguous = 1;
    words[rand() % ((MAX_word_loaded ) - 1) + 1].is_ambiguous = 1;
    words[rand() % ((MAX_word_loaded ) - 1) + 1].is_ambiguous = 1;
    for (int i = 0; i < MAX_word_loaded; i++)
      current_x[i] = rand() % (maxx - 60) + 20;
    last_word = MAX_word_loaded - 1;
    current_y = -MAX_word_loaded + 2;
    words_count = last_word + 1;
    while (current_y + words_count != maxy && last_word > -1)
    {
      borders(difficulity, score);
      words_count = last_word + 1;
      for (int word_iterator = 0; word_iterator < words_count; word_iterator++)
      {
        move(current_y + word_iterator, current_x[word_iterator]);
        words[word_iterator].y_position = current_y + word_iterator;
        if (current_y + word_iterator != 0 && current_y + word_iterator != 1)
        {
          int letter_counter = 0;
          for (letter_counter = 0; letter_counter < words[word_iterator].number_of_inserted_letters; letter_counter++)
          {
            attron(COLOR_PAIR(2));
            addch(words[word_iterator].word[letter_counter]);
            attroff(COLOR_PAIR(2));
          }
          for (int j = words[word_iterator].number_of_inserted_letters; special_mode ? j < 3 : words[word_iterator].word[j] != '\0'; j++)
            addch(words[word_iterator].word[j]);
          if (words[word_iterator].is_ambiguous == 1 && last_word != word_iterator)
          {
            move(current_y + word_iterator, current_x[word_iterator]);
            printw("********************");
          }
          refresh();
        }
      }
      if (tempsleep)
      {
        usleep(speed * 3);
        tempsleep = 0;
      }
      if (special_mode)
      {
        usleep(speed  * 2);
      }
      else
        usleep(speed);
      clear();
      current_y++;

      if (!(current_y + words_count != maxy && last_word > -1))
        losing_screen();
    }

    switch (difficulity)
    {
    case 1:
      speed = 0.8 * speed;
      break;
    case 2:
      speed = 0.7 * speed;
      break;
    case 3:
      speed = 0.6 * speed;
      break;
    }
    wave++;
    special_mode = 0;
  }
}

int menu()
{
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  attron(A_BLINK | COLOR_PAIR(3) | A_BOLD);
  mvprintw(maxy / 2, maxx / 2 - 22, "Hello and Welcome to WORD GAME! -By Aria Saki");
  mvprintw(maxy / 2 + 1, maxx / 2 - 14, "Press any key to continue...");
  refresh();
  attroff(A_BLINK | COLOR_PAIR(3) | A_BOLD);
  getch();
  system("clear");
  refresh();
  WINDOW *w;
  char list[3][7] = {"Easy", "Medium", "Hard"};
  char item[7];
  int ch, i = 0, width = 7;
  w = newwin(5, 12, maxy / 2 - 3, maxx / 2 - 5);

  box(w, 0, 0);
  mvwprintw(stdscr, maxy / 2 - 4, maxx / 2 - 10, "Choose the difficulity: ");
  refresh();
  for (i = 0; i < 3; i++)
  {
    if (i == 0)
      wattron(w, A_STANDOUT);
    else
      wattroff(w, A_STANDOUT);
    strcpy(item, list[i]);
    mvwprintw(w, i + 1, 2, "%s", item);
  }
  wrefresh(w);
  i = 0;
  noecho();
  keypad(w, TRUE);
  curs_set(0);
  while ((ch = wgetch(w)) != '\n')
  {
    strcpy(item, list[i]);
    mvwprintw(w, i + 1, 2, "%s", item);
    
    switch (ch)
    {
    case KEY_UP:
      i--;
      i = (i < 0) ? 2 : i;
      
      break;
    case KEY_DOWN:
      i++;
      i = (i > 2) ? 0 : i;
      break;
    }
    wattron(w, A_STANDOUT);

    strcpy(item, list[i]);
    mvwprintw(w, i + 1, 2, "%s", item);
    wattroff(w, A_STANDOUT);
  }
  delwin(w);
  system("clear");
  refresh();
  if (strcmp(item, "Easy") == 0)
    return 1;
  else if (strcmp(item, "Medium") == 0)
    return 2;
  else if (strcmp(item, "Hard") == 0)
    return 3;
}

void add_score()
{

  if (strlen(words[last_word].word) <= 10)
    score += 1;
  else if (strchr(words[last_word].word, '$') != NULL ||
           strchr(words[last_word].word, '@') != NULL ||
           strchr(words[last_word].word, '!') != NULL ||
           strchr(words[last_word].word, '^') != NULL ||
           strchr(words[last_word].word, '&') != NULL ||
           strchr(words[last_word].word, '%') != NULL)
  {
    score += 3;
    if (words[last_word].y_position > maxy - 4)
    {
      special_mode = 1;
    }
  }
  if (strchr(words[last_word].word, '#') != NULL)
    tempsleep = 1;
  if (words[last_word].is_ambiguous == 1)
    score += 1;
}
void losing_screen()
{
  system("clear");
  borders(0, 0);
  mvwprintw(stdscr, maxy / 2, maxx / 2, "YOU LOSE!");
  switch (difficulity)
  {
  case 1:
    mvwprintw(stdscr, maxy / 2 + 1, maxx / 2, "difficulity: Easy");
    break;
  case 2:
    mvwprintw(stdscr, maxy / 2 + 1, maxx / 2, "difficulity: Medium");
    break;
  case 3:
    mvwprintw(stdscr, maxy / 2 + 1, maxx / 2, "difficulity: Hard");
    break;
  }
  mvwprintw(stdscr, maxy / 2 + 2, maxx / 2 - 10, "You were in wave %d and your score was %d.", wave, score);
  refresh();
  sleep(5);
  endwin();
  exit(0);
}