#include <printf.h>
#include <pthread.h>
#include <ncurses.h>
#include "helper_unix.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#define MAX_word_length 40
#define MAX_word_loaded 10
struct words{
char word[MAX_word_length];
int number_of_inserted_letters;

};
int maxx,maxy,last_word=3; //TODO: change 3 to MAX_word_loaded-1
struct words words[MAX_word_loaded];
void read_from_file(char lines[][MAX_word_length],const char file_path[30],int first_line, int last_line);
void borders();
void game_play(struct words word[],double speed);
void my_callback_on_key_arrival(char c)
{

 if(c==words[last_word].word[words[last_word].number_of_inserted_letters]){
 words[last_word].number_of_inserted_letters++;
 }
 if(words[last_word].number_of_inserted_letters==strlen(words[last_word].word))
 last_word--;
 

}

int main()
{

  initscr();
  getmaxyx(stdscr,maxy,maxx);
  borders();
  char line[MAX_word_loaded][MAX_word_length];
  read_from_file(line,"file.txt",1,4);

  for(int i=0; i<MAX_word_loaded;i++){
  strcpy(words[i].word,line[i]);
  words[i].number_of_inserted_letters=0;
  }
  //words[2].number_of_inserted_letters=3;
 
  pthread_t thread_id = start_listening(my_callback_on_key_arrival);
  game_play(words,1);
 

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
  attroff(COLOR_PAIR(1)|COLOR_PAIR(2));
}
void game_play(struct words word[],double speed ){
  int words_count=last_word+1;
  int current_x[words_count],current_y=-words_count+2;
  srand(time(NULL));
  init_pair(2, COLOR_RED, COLOR_BLACK);
 
  start_color();	
  for(int i=0; i<words_count;i++)
    current_x[i]=rand()%(maxx-60) + 30;
  while(current_y+words_count!=maxy){
    borders();
    words_count=last_word+1;
    for(int i=0; i<words_count; i++){
      move(current_y+i,current_x[i]);
      if(current_y+i!=0 && current_y+i!=1){
      int letter_counter=0;
      for(letter_counter=0;letter_counter<word[i].number_of_inserted_letters;letter_counter++){
      attron(COLOR_PAIR(2));
      addch(word[i].word[letter_counter]);
      attroff(COLOR_PAIR(2));
      }
      for(int j=word[i].number_of_inserted_letters;words[i].word[j]!='\0';j++)
      addch(words[i].word[j]);
      refresh();
      }
    }
    sleep(speed);
    clear();
    current_y++;
    
    
    if(last_word<0){
 endwin();
 system("clear");
 printf("you win!");
 break;
 }
    

  }
}
