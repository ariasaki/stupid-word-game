#include <printf.h>
#include <pthread.h>
#include <ncurses.h>
#include "helper_unix.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#define MAX_word_length 40
int maxx,maxy;
void read_from_file(char lines[][MAX_word_length],const char file_path[30],int first_line, int last_line);
void borders();
void game_play(const char words[][MAX_word_length],double speed, int words_count,char c);
void my_callback_on_key_arrival(char c)
{


  char a[2];
  a[0]=c;
  a[1]='\0';
  move(10,10);
  printw(a);
  refresh();
}

int main()
{

  initscr();
  getmaxyx(stdscr,maxy,maxx);
  borders();
  char words[10][MAX_word_length];
  read_from_file(words,"file.txt",1,4);
  char c;
  game_play(words,1,4,c);

  pthread_t thread_id = start_listening(my_callback_on_key_arrival);

 

  pthread_join(thread_id, NULL);
  endwin();
  return 0;
}
void read_from_file(char lines[][MAX_word_length],const char file_path[30],int first_line, int last_line)
{
  FILE *fptr = NULL; 
  int i = 0;
  fptr = fopen(file_path, "r");
  for (int i = 0; i < first_line-1; i++)
  fgets(lines[0], MAX_word_length, fptr);
  while(fgets(lines[i], MAX_word_length, fptr)){
    lines[i][strlen(lines[i]) - 1] = '\0';
    i++;
    if(i==last_line-first_line+1)
      break;
  }
}
void borders(){
  start_color();
  init_pair(1,COLOR_GREEN,COLOR_BLACK);
  attron(COLOR_PAIR(1)|A_DIM);

  for(int i=0;i<maxx;i++){
    move(1,i);
    printw("#");
  }
  for(int i=1;i<maxy;i++){
  move(i,0);
  printw("#");
  move(i,maxx-1);
  printw("#");
  }
  for(int i=0;i<maxx;i++){
    move(maxy-1,i);
    printw("#");
  }
  refresh();
}
void game_play(const char words[][MAX_word_length],double speed, int words_count,char c){
  int current_x[words_count],current_y=-words_count+2;
  srand(time(NULL));
  for(int i=0; i<words_count;i++)
    current_x[i]=rand()%(maxx-60) + 30;
  while(current_y+words_count!=maxy){
    borders();
    for(int i=0; i<words_count; i++){
      move(current_y+i,current_x[i]);
      if(current_y+i!=0 && current_y+i!=1){
      printw(words[i]);
      refresh();}
    }
    sleep(speed);
    clear();
    current_y++;
  }
}