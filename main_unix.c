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
#define easy_wave 10
#define regular_wave 8
#define difficult_wave 5

struct words{
char word[MAX_word_length];
int number_of_inserted_letters;

};

int maxx,maxy,last_word=3; //TODO: change 3 to MAX_word_loaded-1
struct words words[MAX_word_loaded];

void read_from_file(char lines[][MAX_word_length],const char file_path[30],int first_line, int last_line);
void borders(const int difficulity, const int score);
void game_play(struct words word[],double speed);
int menu();

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
  borders(0,0);
  menu();
   borders(3,0);
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
void borders(const int difficulity, const int score){
  start_color();
  init_pair(1,COLOR_GREEN,COLOR_BLACK);
  attron(COLOR_PAIR(1)|A_DIM);
   if(difficulity!=0){
  switch(difficulity){
      case 1:
  	 mvwprintw( stdscr, 0, 2, "difficulity: Easy" ); 
       break;
      case 2:
  	 mvwprintw( stdscr, 0, 2, "difficulity: Medium" ); 
       break;
       case 3:
  	 mvwprintw( stdscr, 0, 2, "difficulity: Hard" ); 
       break;
  }
  }
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
    borders(2,0);
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

int menu(){
init_pair(3,COLOR_YELLOW,COLOR_BLACK);
attron(A_BLINK | COLOR_PAIR(3) | A_BOLD);
mvprintw(maxy/2, maxx/2-22, "Hello and Welcome to WORD GAME! -By Aria Saki");
mvprintw(maxy/2+1, maxx/2-14, "Press any key to continue...");
refresh();
attroff(A_BLINK | COLOR_PAIR(3)| A_BOLD);
getch();
system("clear");
refresh();
WINDOW *w;
char list[3][7] = { "Easy", "Medium", "Hard"};
char item[7];
int ch, i = 0, width = 7;
w = newwin( 5, 12, maxy/2-3, maxx/2-5 ); // create a new window

box( w, 0, 0 ); // sets default borders for the window
// now print all the menu items and highlight the first one
mvwprintw( stdscr, maxy/2-4, maxx/2-10, "Choose the difficulity: " );
refresh( );
for( i=0; i<3; i++ ) {
if( i == 0 ) 
wattron( w, A_STANDOUT ); // highlights the first item.
else
wattroff( w, A_STANDOUT );
sprintf(item, "%-7s",  list[i]);
mvwprintw( w, i+1, 2, "%s", item );
}
wrefresh( w ); // update the terminal screen
i = 0;
noecho(); // disable echoing of characters on the screen
keypad( w, TRUE ); // enable keyboard input for the window.
curs_set( 0 ); // hide the default screen cursor.
// get the input
while(( ch = wgetch(w)) != '\n'){ 
// right pad with spaces to make the items appear with even width.
sprintf(item, "%-7s",  list[i]); 
mvwprintw( w, i+1, 2, "%s", item ); 
// use a variable to increment or decrement the value based on the input.
switch( ch ) {
case KEY_UP:
i--;
i = ( i<0 ) ? 2 : i;
break;
case KEY_DOWN:
i++;
i = ( i>2 ) ? 0 : i;
break;
}
// now highlight the next item in the list.
wattron( w, A_STANDOUT );
sprintf(item, "%-7s",  list[i]);
mvwprintw( w, i+1, 2, "%s", item);
wattroff( w, A_STANDOUT );
}
delwin( w );
system("clear");
refresh();
if(strcmp(item,"Easy"))
return 1;
else if(strcmp(item,"Medium"))
return 2;
else if(strcmp(item,"Hard"))
return 3;

}
