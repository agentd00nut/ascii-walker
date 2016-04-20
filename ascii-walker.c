#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
static int n_cols;
static int n_rows;
void sig_handler(int signo){
    if (signo == SIGINT){
        char* white     = "\e[97m";
        printf("%sCleaning up ...\n", white);
        exit(1);
    }
}
int print_map(char **grid, int x, int y){
    //every pimp needs a top and bottom bitch
    int i;
    int j;
    char* black     = "\e[30m";
    char* white     = "\e[97m";
    char* green     = "\e[32m";
    char* magenta   = "\e[35m";
    char* cyan      = "\e[96m";
    char* red      = "\e[91m";
    char *buf = (char*)malloc(sizeof(char)*(n_cols*n_rows*13));
    char temp_char = ' ';


    int iter = 0;

    for(i = 0; i < n_rows; ++i){

        for(j = 0; j < n_cols; ++j){

            if( i == y && j == x)
            {
                temp_char = grid[i][j];
                grid[i][j] = '@';
            }

            if(grid[i][j] == '@'){
                iter += sprintf(buf+iter, "%s%c%s", white, grid[i][j], black );
            }else if(grid[i][j] == '#'){
                iter += sprintf(buf+iter, "%s%c%s", green, grid[i][j], black );
            }else if(grid[i][j] == '*'){
                iter += sprintf(buf+iter, "%s%c%s", magenta, grid[i][j], black );
            }else if(grid[i][j] == '%'){
                iter += sprintf(buf+iter, "%s%c%s", cyan, grid[i][j], black );
            }else if(grid[i][j] == '&'){
                iter += sprintf(buf+iter, "%s%c%s", red, grid[i][j], black );
            }else{
                iter += sprintf(buf+iter, "%c", grid[i][j]);
            }
        }

        iter += sprintf(buf+iter, "\n");
    }
    
    printf("%s", buf); //%s\n
    free(buf);

    grid[y][x] = temp_char;

    //printf("\033[%i;%iH%s%c%s", y, x, white, '@', green); Sorta works but like the guy doesn't show up untill dah end
    //printf("\033[%i;%iH%s%c%s\033[0;0H", y, x, white, '@', black);  


    return 1;
}
int main(int argc, char **argv){
    int i;  //the all around iterating bitch
    int j;  //the bottom bitch
    int k;
    int chances[8];
    char current_pos = ' ';
    char tile = ' ';
    char player = '@';
    char path = '#';
    char second_visit = '*';
    char third_visit = '%';
    char fourth_visit = '&';
    srand(time(NULL));              //seed rng
    struct winsize w;               //get tty size
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    n_rows = w.ws_row;
    n_cols = w.ws_col;
    int x = rand()%n_cols;    //initial point
    int y = rand()%n_rows;
    int horizontal;
    int vertical;
    int direction;
    int total;
    int temp;
    //my grid
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
    
    //grid[y][x] = player;

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
        if(horizontal < 0){ horizontal = n_cols-1; }
        if(vertical >= n_rows ){ vertical = 0; }
        if(vertical < 0){ vertical = n_rows-1; }

        if( grid[y][x] == path ){ grid[y][x] = second_visit; } 
        else if(   grid[y][x] == second_visit){ grid[y][x] = third_visit; }
        else if(   grid[y][x] == third_visit){  grid[y][x] = fourth_visit; }
        else if(   grid[y][x] == fourth_visit){ grid[y][x] = fourth_visit; }
        else{ grid[y][x] = path; }

        
        x = horizontal;
        y = vertical;
        //usleep(65000); NICE
        //usleep(50000);
        usleep(40000);
        //usleep(100000000);
        //sleep(1);
    }
    return 0;
}