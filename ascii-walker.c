#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ncurses.h>

static int n_cols;
static int n_rows;
static int last_color;

void sig_handler(int signo){
    if (signo == SIGINT){
        char* white     = "\e[97m";
        printf("%sCleaning up ...\n", white);
        
        scr_dump("my_file");
        endwin();
        
        exit(1);
    }
}

int print_map(char **grid, int x, int y){
    //every pimp needs a top and bottom bitch
    int i;
    int j;


    char current_char = ' ';
    int  current_color = 1;


    for(i = 0; i < n_rows; ++i){

        for(j = 0; j < n_cols; ++j){

            current_char = grid[i][j];

            if(current_char == '.'){ 
                current_color = 3; 
            }
            else if(current_char == ','){ 
                current_color = 4; 
            }
            else if(current_char == '*'){ 
                current_color = 5; 
            }
            else if(current_char == '#'){ 
                current_color = 6; 
            }

            move(i,j);
            addch(current_char | COLOR_PAIR(current_color)  );

        }
    }
    
    move(y,x);
    addch('@' | COLOR_PAIR(2) );
    refresh();

    return 1;
}



int main(int argc, char **argv){
    int i;  //the all around iterating bitch
    int j;  //the bottom bitch
    int k;

    char tile = ' ';
    char player = '@';
    char path = '.';
    char second_visit = ',';
    char third_visit = '*';
    char fourth_visit = '#';

    srand(time(NULL));              //seed rng
    struct winsize w;               //get tty size
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    n_rows = w.ws_row-2;
    n_cols = w.ws_col;
    int x = rand()%n_cols;    //initial point
    int y = rand()%n_rows;
    int horizontal;
    int vertical;
    int direction;
    int total;
    int sleep_time = 40000;

    initscr();          /* Start curses mode          */
    noecho(); // Don't echo any keypresses
    curs_set(FALSE); // Don't display a cursor
    start_color();


    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);

    // LAZY ARGS
    if(argc > 1)
    {
        sleep_time = atoi(argv[1]);
    }


    // BUILD GRID
    char **grid = (char**)malloc(sizeof(char*)*n_rows);
    for(i = 0; i < n_rows; ++i){
        grid[i] = (char*)malloc(sizeof(char)*n_cols);
        for(j = 0; j < n_cols; ++j){
            grid[i][j] = tile;
        }
    }
    if (signal(SIGINT, sig_handler) == SIG_ERR){
        //pass
    }
    

    while(print_map(grid, x, y)){
        total = 96;
        direction = rand()%total;
        
        if(direction < 12){
            //cout<<"going down\n";
            horizontal = x;
            vertical = y + 1;
        }else if(direction < 24){
            //cout<<"going right\n";
            horizontal = x + 1;
            vertical = y;
        }else if(direction < 36){
            //cout<<"going up\n";
            horizontal = x;
            vertical = y - 1;
        }else if(direction < 48){
            //cout<<"going left\n";
            horizontal = x - 1;
            vertical = y;
        }else if(direction < 60){
            //cout<<"going up right\n";
            horizontal = x + 1;
            vertical = y - 1;
        }else if(direction < 72){
            //cout<<"going down left\n";
            horizontal = x - 1;
            vertical = y + 1;
        }else if(direction < 84){
            //cout<<"going up left\n";
            horizontal = x - 1;
            vertical = y - 1;
        }else if(direction < 96){
            //cout<<"going down right\n";
            horizontal = x + 1;
            vertical = y + 1;
        }
        //wrap around
        if(horizontal >= n_cols){ horizontal = 0; }
        else if(horizontal < 0){ horizontal = n_cols-1; }
        if(vertical >= n_rows ){ vertical = 0; }
        else if(vertical < 0){ vertical = n_rows-1; }

        if( grid[y][x] == path ){ grid[y][x] = second_visit; } 
        else if(   grid[y][x] == second_visit){ grid[y][x] = third_visit; }
        else if(   grid[y][x] == third_visit){  grid[y][x] = fourth_visit; }
        else if(   grid[y][x] == fourth_visit){ grid[y][x] = fourth_visit; }
        else{ grid[y][x] = path; }

        
        x = horizontal;
        y = vertical;

        usleep(sleep_time);

    }
    return 0;
}