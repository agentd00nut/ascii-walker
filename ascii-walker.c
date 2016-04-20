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

static int decay_time = 30000;

static char player = '@';
static char tile = ' ';
static char path = '.';
static char second_visit = ',';
static char third_visit = '*';
static char fourth_visit = '#';
static char fifth_visit = '^';
static char sixth_visit = '+';


static char char_map[8];
static int color_map[8];
static int max_characters;


void sig_handler(int signo){
    if (signo == SIGINT){
        char* white     = "\e[97m";
        printf("%sCleaning up ...\n", white);
        
        scr_dump("my_file");
        endwin();
        
        exit(1);
    }
}

int randomize_decay_time(int decay_time)
{
    return random()%decay_time;
}

void init_char_map()
{
    char_map[0] = player;
    char_map[1] = ' ';
    char_map[2] = '.';
    char_map[3] = ',';
    char_map[4] = '*';
    char_map[5] = '#';
    char_map[6] = '^';
    char_map[7] = '%';

    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);

    init_pair(8, COLOR_WHITE, COLOR_BLACK);

    max_characters = sizeof(char_map) / sizeof(char_map[0]);

}

int print_map(int **grid, int **decay, int x, int y){
    //every pimp needs a top and bottom bitch
    int i;
    int j;

    int current_char = 0;
    int print_char = 0;
    int decay_amount = 0;


    int poop = 0;

    for(i = 0; i < n_rows; ++i){
        for(j = 0; j < n_cols; ++j){

            current_char = grid[i][j];

            if(current_char == 1)
            {
                continue;
            }

            decay[i][j]--;

            if(decay[i][j] == 0)
            {
                decay[i][j] = randomize_decay_time(decay_time);
                
                if(current_char > 1)
                {
                    current_char -= (random()%2);
                }
                if(current_char <= 0)
                {
                    current_char = 1;
                }

                grid[i][j] = current_char;
            }

            move(i,j);
            addch( char_map[current_char]  | COLOR_PAIR(current_char) );

        }
    }
    
    move(y,x);
    addch('@' | COLOR_PAIR(8) );
    refresh();

    return 1;
}






int main(int argc, char **argv){
    int i;  //the all around iterating bitch
    int j;  //the bottom bitch
    int k;

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


    // NCURSES INIT

    initscr();          /* Start curses mode          */
    noecho();           // Don't echo any keypresses
    curs_set(FALSE);    // Don't display a cursor
    start_color();

 
    init_char_map();        // This probably has to be after start_color(); ? 

    // LAZY ARGS

    if(argc > 1)
    {
        sleep_time = atoi(argv[1]);
    }


    // BUILD GRID
    int **grid = (int**)malloc(sizeof(int*)*n_rows);
    int **decay = (int**)malloc(sizeof(int*)*n_rows);

    for(i = 0; i < n_rows; ++i){
        grid[i] = (int*)malloc(sizeof(int)*n_cols);
        decay[i] = (int*)malloc(sizeof(int)*n_cols);

        for(j = 0; j < n_cols; ++j){
            grid[i][j]  = 1;
            decay[i][j] = randomize_decay_time(decay_time);
        }
    }
    if (signal(SIGINT, sig_handler) == SIG_ERR){
        //pass
    }
    

    while(print_map(grid, decay, x, y)){
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

        if( grid[y][x] < max_characters-1)
        {
            grid[y][x]++;
        }
        
        x = horizontal;
        y = vertical;

        usleep(sleep_time);

    }
    return 0;
}