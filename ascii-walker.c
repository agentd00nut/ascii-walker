#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>      /* printf */
#include <time.h>       /* time_t, time (for timestamp in second) */
#include <sys/timeb.h>  /* ftime, timeb (for timestamp in millisecond) */
#include <sys/time.h>   /* gettimeofday, timeval (for timestamp in microsecond) */

#include <ncurses.h>

static int n_cols;
static int n_rows;
static int last_color;

static int decay_time =  15000;//15000; good for non cascading decay
static int decay_i = 0;

static char char_map[8];
static int color_map[8];
static int color_counts[8];
static int max_characters;
static int should_decay = 0;


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
    char_map[0] = '@';
    char_map[1] = ' ';
    char_map[2] = '8';//'.';
    char_map[3] = '8';//',';
    char_map[4] = '8';//'*';
    char_map[5] = '8';//'#';
    char_map[6] = '8';//'^';
    char_map[7] = '8';//'%';

    /*init_pair(1, COLOR_BLACK, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);*/

    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLUE);
    init_pair(3, COLOR_CYAN, COLOR_CYAN);
    init_pair(4, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(5, COLOR_GREEN, COLOR_GREEN);
    init_pair(6, COLOR_RED, COLOR_RED);
    init_pair(7, COLOR_MAGENTA, COLOR_MAGENTA);

    init_pair(8, COLOR_WHITE, COLOR_BLACK);

    max_characters = sizeof(char_map) / sizeof(char_map[0]);

}

long long int get_ms_stamp()
{
struct timeval timer_usec; 
  long long int timestamp_usec; /* timestamp in microsecond */
  if (!gettimeofday(&timer_usec, NULL)) {
    timestamp_usec = ((long long int) timer_usec.tv_sec) * 1000000ll + 
                        (long long int) timer_usec.tv_usec;
  }
  else {
    timestamp_usec = -1;
  }
  //printf("%lld microseconds since epoch\n", timestamp_usec);
  return timestamp_usec;
}

int sum_surrounding_tiles(int **grid, int y, int x)
{
    int sum = 0;
    int i;
    int j;
    int off_x;
    int off_y;
     //wrap around

    for( i = -1; i<2; ++i){

        off_y = y+i;
        if(off_y >= n_rows) { off_y = 0; } else if(off_y < 0)  { off_y = n_rows-1; }

        for(j = -1; j<2; ++j){

            off_x = x+i;
            if(off_x >= n_cols ){ off_x = 0; } else if(off_x < 0)  { off_x = n_cols-1; }

            sum += grid[off_y][off_x];
        }
    }

    return sum - grid[y][x]; // Don't add myself into the sum.
}


int wrap_horizontal(int horizontal)
{
    if(horizontal >= n_cols){ horizontal = horizontal-n_cols; }
    else if(horizontal < 0){ horizontal = n_cols+horizontal; }
    return horizontal;
}

int wrap_vertical(int vertical)
{
    if(vertical >= n_rows ){ vertical = vertical-n_rows; }
    else if(vertical < 0){ vertical = n_rows+vertical; }

    return vertical;
}

int * random_direction(int x, int y)
{
    static int directions[2];
    int direction;
    int horizontal;
    int vertical;
    int total = 96;

     direction = rand()%total;
        
    if(direction < 6){
        //cout<<"going down\n";
        horizontal = x;
        vertical = y + 1;
    }else if(direction < 24){
        //cout<<"going right\n";
        horizontal = x + 1;
        vertical = y;
    }else if(direction < 30){
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
    directions[0]  = wrap_horizontal(horizontal);
    directions[1]  = wrap_vertical(vertical);

    return directions;
}



int faster_decay(int **grid, int **decay, int *decay_x, int *decay_y)
{
    int i;
    int x;
    int y;

    int current_char    = 0;

    for(i = 0; i < decay_i; ++i){
        x = decay_x[i];
        y = decay_y[i];

        decay[y][x]--;

        if(decay[y][x] <= 0)
        {
            current_char = grid[y][x];
             
            /*if(current_char*8 == sum_surrounding_tiles(grid, i, j)){
                decay[i][j] = 1;
                continue;
            }*/

            decay[y][x] = randomize_decay_time(decay_time);


            if(current_char > 1){
                current_char -= (random()%1)+1;
            }
            if(current_char <= 0){
                current_char = 1;
            }


            grid[y][x] = current_char;
            move(y,x);
            addch( char_map[current_char]  | COLOR_PAIR(current_char) );
        }
    }
    return 1;
}



void draw_statistics(int fps, int tiles, int steps)
{
    int i;  
    attron(COLOR_PAIR(8));
    mvprintw(n_rows, 5, "FPS: %d", fps);
    mvprintw(n_rows, 18, "Tiles: %d", tiles);
    mvprintw(n_rows, 32, "Steps: %d", steps);

    for(i=2; i<8; ++i) // 8 because thats the # of colors we have
    {
        move(n_rows+1,38+i*6);
        mvprintw(n_rows+1, 38+i*6, "%d", color_counts[i]);
    }
    attroff(COLOR_PAIR(1));
}

int grid_decay(int **grid, int **decay)
{
    int i;
    int j;

    int current_char = 0;


     for(i = 0; i < n_rows; ++i){
        for(j = 0; j < n_cols; ++j){

            current_char = grid[i][j];

            if(current_char == 1){ continue; }

            decay[i][j]--;  

            if(decay[i][j] <= 0)
            {
                 
                /*if(current_char*8 == sum_surrounding_tiles(grid, i, j)){
                    decay[i][j] = 1;
                    continue;
                }*/

                decay[i][j] = randomize_decay_time(decay_time);


                if(current_char > 1){
                    current_char -= (random()%1)+1;
                }
                if(current_char <= 0){
                    current_char = 1;
                }

                color_counts[ grid[i][j] ] --;
                grid[i][j] = current_char;
                color_counts[ grid[i][j] ] ++;
                move(i,j);
                addch( char_map[current_char]  | COLOR_PAIR(current_char) );
            }
        }
    }

    return 1;
}

static int players = 12;


int main(int argc, char **argv){
  
    int i;  //the all around iterating bitch
    int j;  //the bottom bitch
    int k;

    srand(time(NULL));              //seed rng
    struct winsize w;               //get tty size
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    n_rows = w.ws_row-2;
    n_cols = w.ws_col;
    //int x = rand()%n_cols;    //initial point
    //int y = rand()%n_rows;
    int x;
    int y;
    long long int sleep_time = 40000;
    long long int loop_time = get_ms_stamp();
    int second = 0;
    int start_time = 0;
    int fps = 0;

    int horizontal;
    int vertical;

    // NCURSES INIT

    initscr();          /* Start curses mode          */
    noecho();           // Don't echo any keypresses
    curs_set(FALSE);    // Don't display a cursor
    start_color();
 
    init_char_map();        // This probably has to be after start_color(); ? 

    // LAZY ARGS

    int show_players = 1;
    int should_cycle = 1;

    if(argc > 1)
    {
        show_players    = atoi(argv[1]);
        should_decay    = atoi(argv[2]);
        should_cycle    = atoi(argv[3]);
        sleep_time      = atoi(argv[4]);
        players         = atoi(argv[5]);
    }

    int players_x[players];
    int players_y[players];

    // BUILD GRID
    int **grid = (int**)malloc(sizeof(int*)*n_rows);
    int **decay = (int**)malloc(sizeof(int*)*n_rows);

    for(i = 0; i < n_rows; ++i){
        grid[i] = (int*)malloc(sizeof(int)*n_cols);
        decay[i] = (int*)malloc(sizeof(int)*n_cols);

        for(j = 0; j < n_cols; ++j){
            grid[i][j]  = 1;
            //decay[i][j] = randomize_decay_time(decay_time);
        }
    }
    if (signal(SIGINT, sig_handler) == SIG_ERR){
        //pass
    }
    
    int tiles =0;
    int steps =0;
    start_time = time(NULL);

    players_x[0] = rand()%n_cols;
    players_y[0] = rand()%n_rows;
    

    int up_or_down = 0;
    for(i=1; i<players; ++i)
    {
        players_x[i] = wrap_horizontal( (rand()%n_cols)+players_x[i-1] );
        players_y[i] = wrap_vertical(   (rand()%n_rows)+players_y[i-1] );
    }


    for(i=2; i<8; ++i) // 8 because thats the # of colors we have
    {
        move(n_rows+1,37+i*6);
        addch( char_map[ i ]  | COLOR_PAIR( i ) );
    }

    for(;;){

        for(i=0; i<players; ++i)
        {
            x = players_x[i];
            y = players_y[i];

            steps ++;
           
           if( grid[y][x] == 1)
            {
                tiles++;
            }

            if( grid[y][x] < max_characters-1)
            {
                color_counts[grid[y][x]] --;
                grid[y][x]++;
                color_counts[grid[y][x]] ++;

                decay[y][x]         = randomize_decay_time(decay_time);
                //decay_x[decay_i]    = x;
                //decay_y[decay_i]    = y;
                //decay_i++;
            }else if(grid[y][x] == max_characters-1 && should_cycle)
            {
                color_counts[grid[y][x]] --;
                grid[y][x] = 1;
                decay[y][x]         = randomize_decay_time(decay_time);
            }


  
            
            move(y,x);
            addch( char_map[ grid[y][x] ]  | COLOR_PAIR( grid[y][x] ) );

            int *directions = random_direction(x,y);
            horizontal      = directions[0];
            vertical        = directions[1];
            players_y[i]    = vertical;
            players_x[i]    = horizontal;

            if(show_players)
            {
                move(vertical,horizontal);
                addch('@' | COLOR_PAIR(8) );
            }

        }

        if(should_decay) 
        { 
            grid_decay(grid, decay); 
        }

        if(time(NULL) - start_time >= 1)
        {
            draw_statistics(fps, tiles, steps);

            fps=0;
            start_time = time(NULL);
        }
        

        refresh();
        fps ++;

        usleep( sleep_time );

    }
    return 0;
}