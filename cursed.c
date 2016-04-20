#include <ncurses.h>

void checkitout(char* thing)
{
	printw(thing);
}

int main(int argc, char **argv){

	initscr();

	int i = 0;

	while(true){

		mvprintw(5,5,'p');
		
		refresh();
	}

	return 0;
}