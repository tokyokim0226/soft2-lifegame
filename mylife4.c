#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // sleep()関数を使う

//initializing cells by reading in a file. The format of the file will be rendered within this function.
//if file is NULL, then the function should create a random initialization by itself
void my_init_cells(const int height, const int width, int cell[height][width], FILE* fp);

//drawing the grid out
void my_print_cells(FILE *fp, int gen, const int height, const int width, int cell[height][width]);

//counting for how many cells are alive around
int my_count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]);

//updating the cells according to the rules of lifegame
void my_update_cells(const int height, const int width, int cell[height][width] ,const int rule[2][9]);

//function to print the current rule
void print_rule(int arr[2][9]);


int main(int argc, char **argv){


    FILE *fp = stdout;
    const int height = 40;
    const int width = 70;

    int cell[height][width];
    for(int y = 0 ; y < height ; y++){
        for(int x = 0 ; x < width ; x++){
            cell[y][x] = 0;
        }
    }

    int rule[2][9] = {{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}};

    /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
    if ( argc > 3 ) {
        fprintf(stderr, "usage: %s [Input your Filename] [Input your Rule]\n", argv[0]);
        return EXIT_FAILURE;
    }

    //when there are 3 inputs given after output
    else if (argc == 3) {

        FILE *lgfile;

        if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
            my_init_cells(height,width,cell,lgfile); // ファイルによる初期化
            if (argv[2][0] == 'B' || argv[2][0] == 'b') {
                size_t len = strlen(argv[2]);
                int B_zero_S_one = 0;
                for (int i = 0; i<len; ++i) {
                    if (argv[2][i] == 'B' || argv[2][i] == 'b') {
                        B_zero_S_one = 0;
                    }
                    else if (argv[2][i] == 'S' || argv[2][i] == 's') {
                        B_zero_S_one = 1;
                    }
                    else if ('0' <= argv[2][i] && argv[2][i] <= '9') {
                        rule[B_zero_S_one][argv[2][i] - '0'] = 1;
                    }
                }
            }
            else {
                fprintf(stderr,"Your Rule is probably not in the right format:\nusage: %s [Input your Filename] [Input your Rule]\n", argv[0]);
                return EXIT_FAILURE;
            }
        }
        else{
            fprintf(stderr,"cannot open file %s\n usage: %s [Input your Filename] [Input your Rule]\n",argv[1], argv[0]);
            return EXIT_FAILURE;
        }
        fclose(lgfile);
    }

    //when there are 2 inputs given after output
    else if (argc == 2) {

        FILE *lgfile;

        if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
            my_init_cells(height,width,cell,lgfile); // ファイルによる初期化
            rule[0][3] = 1;
            rule[1][2] = 1;
            rule[1][3] = 1;
            fclose(lgfile);
        }
        

        else if (argv[1][0] == 'B' || argv[1][0] == 'b') {
            my_init_cells(height,width,cell,NULL);
            size_t len = strlen(argv[1]);
            int B_zero_S_one = 0;
            for (int i = 0; i<len; ++i) {
                if (argv[1][i] == 'B' || argv[1][i] == 'b') {
                    B_zero_S_one = 0;
                }
                else if (argv[1][i] == 'S' || argv[1][i] == 's') {
                    B_zero_S_one = 1;
                }
                else if ('0' <= argv[1][i] && argv[1][i] <= '9') {
                    rule[B_zero_S_one][argv[1][i] - '0'] = 1;
                }
            }
        }

        else{
            fprintf(stderr,"Your Rule is probably not in the right format:\nusage: %s [Input your Filename] [Input your Rule]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    //default mode when no file and no rule has been given
    else{
        my_init_cells(height, width, cell, NULL); // デフォルトの初期値を使う
        rule[0][3] = 1;
        rule[1][2] = 1;
        rule[1][3] = 1;
    }
 

    
    print_rule(rule);
    my_print_cells(fp, 0, height, width, cell); // 表示する
    sleep(1); // 1秒休止
    fprintf(fp,"\e[%dA",height+5);//height+3 の分、カーソルを上に戻す(壁2、表示部1)

    /* 世代を進める*/
    for (int gen = 1 ;; gen++) {
        my_update_cells(height, width, cell, rule); // セルを更新
        print_rule(rule);
        my_print_cells(fp, gen, height, width, cell);  // 表示する
        sleep(1); //1秒休止する
        fprintf(fp,"\e[%dA",height+5);//height+3 の分、カーソルを上に戻す(壁2、表示部1)

        //make a file that contains the lifegame cells status for every 100 generations

        if (gen % 100 == 0 && gen < 10000) {
            FILE *file;
            char filename[50];
            sprintf(filename, "gen%04d.lif", gen);
            file = fopen(filename, "w");
            fprintf(file, "#Life 1.06\n");
            for (int y = 0; y < height ; ++y) {
                for (int x = 0; x < width; ++x) {
                    if (cell[y][x]) {
                        fprintf(file, "%d %d\n", x, y);
                    }
                }
            }
            fclose(file);
        }

    }

    return EXIT_SUCCESS;
}

void my_init_cells(const int height, const int width, int (*cell)[width], FILE *fp) {
    srand(time(0));
    if (fp == NULL) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width ; ++x) {
                if (rand() % 10 == 0) {
                    cell[y][x] = 1;
                }
                else {
                    cell[y][x] = 0;
                }
            }
        }
        return;
    }

    const size_t bufsize = 500;
    char buf[bufsize];
    int end_of_file = 0;
    int x = 0;
    int y = 0;
    int len_buffer = 0;
    int is_RLE = 0;

    while (fgets(buf, bufsize, fp)!=NULL) {
        size_t len = strlen(buf) - 1;
        if (buf[0] == '#' && ((buf[1] == 'L' && buf[2] == 'i' && buf[3] == 'f' && buf[4] == 'e' && buf[5] == ' ' && buf[6] == '1' && buf[7] == '.' && buf[8] == '0' && buf[9] == '6') || buf[1] == 'C' || buf[1] == 'c' || buf[1] == 'N' || buf[1] == 'O' || buf[1] == 'P' || buf[1] == 'R' || buf[1] == 'r')) {
            continue;
        }
        else if (buf[0] == 'x' && buf[2] == '=') {
            is_RLE = 1;
            continue;
        }
        else {
            if (is_RLE == 0) {
                int x2, y2;
                sscanf(buf, "%d %d%*1[\n]", &x2, &y2);
                cell[y2][x2] = 1;
            }
            else if (is_RLE == 1) {
                for (int i = 0; i < len; ++i) {
                    if ('0' <= buf[i] && buf[i] <= '9') {
                        len_buffer = len_buffer * 10 + (buf[i] - '0');
                    }
                    else {
                        if (buf[i] == '!') {
                            end_of_file = 1;
                            break;
                        }
                        else if (buf[i] == '$') {
                            if (len_buffer == 0) {
                                len_buffer = 1;
                            }
                            x = 0;
                            y += len_buffer;
                            len_buffer = 0;
                        }
                        else if (buf[i] == 'b' || buf[i] == 'o') {
                            if (len_buffer == 0) {
                                len_buffer = 1;
                            }
                            if (buf[i] == 'o') {
                                for (int j = 0; j < len_buffer; ++j) {
                                    cell[y][x+j] = 1;
                                }
                            }
                            x += len_buffer;
                            len_buffer = 0;
                        }
                    }
                }
            }
            if (end_of_file) {
                break;
            }
        }
    }
}

void my_print_cells(FILE *fp, int gen, const int height, const int width, int (*cell)[width]) {
    
    //keep in track of the number of cells that are alive
    int alive = 0;
    for (int y = 0; y<height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (cell[y][x]) {
                alive ++;
            }
        }
    }

    fprintf(fp, "Generation: %d, Cells Alive: %d/%d\n", gen, alive, height*width);

    fprintf(fp, "+");
    for (int x = 0; x < width; ++x) {
        fprintf(fp, "-");
    }
    fprintf(fp, "+\n");

    for (int y = 0; y < height; ++y) {
        fprintf(fp, "|");
        for (int x = 0 ; x< width ; ++x) {
            if (cell[y][x]) {
                fprintf(fp, "\e[31m#\e[0m");
            }
            else {
                fprintf(fp, " ");
            }
        }
        fprintf(fp, "|\n");
    }

    fprintf(fp, "+");
    for (int x = 0; x < width; ++x) {
        fprintf(fp, "-");
    }
    fprintf(fp, "+\n");

    fflush(fp);
}

//counts how many of the 8 adjacent cells around a cell with coordinates (h,w) are a non-dead cell
int my_count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]) {
    int count = 0;

    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

    for (int i = 0; i<8; ++i){
        if ((0 <= w + dx[i]) && (w + dx[i] < width) && (0 <= h + dy[i]) && (h + dy[i] < height)) {
            if(cell[h+dy[i]][w+dx[i]]) {
                count ++;
            }
        }
    }

    return count;
}

void my_update_cells(const int height, const int width, int cell[height][width] ,const int rule[2][9]) {
    int temp[height][width];

    for (int y = 0; y<height; y++) {
        for (int x = 0; x < width; x++) {
            temp[y][x] = 0;
        }
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int count_adjacent = my_count_adjacent_cells(y, x, height, width, cell);
            if(cell[y][x]) {
                if (rule[1][count_adjacent] == 1) {
                    temp[y][x] = 1;
                }
            }

            else {
                if (rule[0][count_adjacent] == 1) {
                    temp[y][x] = 1;
                }
            }
        }
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cell[y][x] = temp[y][x];
        }
    }
}

void print_rule(int arr[2][9]) {
    printf("\nCurrent Rule is: [");
    for (int i = 0; i < 2; ++i) {
        if(i == 0) {
            printf("B");
        }
        else if (i == 1) {
            printf("S");
        }
        for (int j = 0; j < 9; ++j) {
            if (arr[i][j] == 1) {
                printf("%d",j);
            }
        }
        if(i == 0) {
            printf("/");
        }
    }
    printf("]\n");
}