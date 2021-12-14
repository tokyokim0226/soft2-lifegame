#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // sleep()関数を使う

//initializing cells by reading in a file. The format of the file will be rendered within this function.
//if file is NULL, then the function should create a random initialization by itself
void my_init_cells(const int height, const int width, int cell[height][width], FILE* fp);

//drawing the grid out
void my_print_cells(FILE *fp, int gen, const int height, const int width, int cell[height][width]);

//counting for how many cells are alive around
int my_count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]);

//updating the cells according to the rules of lifegame
void my_update_cells(const int height, const int width, int cell[height][width]);


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

    /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
    if ( argc > 2 ) {
        fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
        return EXIT_FAILURE;
    }

    else if (argc == 2) {

        FILE *lgfile;

        if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
            my_init_cells(height,width,cell,lgfile); // ファイルによる初期化
        }
        else{
            fprintf(stderr,"cannot open file %s\n",argv[1]);
            return EXIT_FAILURE;
        }
        fclose(lgfile);
    }

    else{
        my_init_cells(height, width, cell, NULL); // デフォルトの初期値を使う
    }

    my_print_cells(fp, 0, height, width, cell); // 表示する
    sleep(1); // 1秒休止
    fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)

    /* 世代を進める*/
    for (int gen = 1 ;; gen++) {
        my_update_cells(height, width, cell); // セルを更新
        my_print_cells(fp, gen, height, width, cell);  // 表示する
        sleep(1); //1秒休止する
        fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)

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

    if (fp == NULL) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width ; ++x) {
                if (rand() % 10 == 0) {
                    cell[y][x] = 1;
                }
            }
        }
        return;
    }

    const size_t bufsize = 100;
    char buf[bufsize];

    while (fgets(buf, bufsize, fp)!=NULL) {
        if (buf[0] == '#') {
            continue;
        }
        else {
            int x, y;
            sscanf(buf, "%d %d%*1[\n]", &x, &y);
            cell[y][x] = 1;
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

void my_update_cells(const int height, const int width, int cell[height][width]) {
    int temp[height][width];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int count_adjacent = my_count_adjacent_cells(y, x, height, width, cell);
            if(cell[y][x]) {
                if (count_adjacent == 2 || count_adjacent == 3) {
                    temp[y][x] = 1;
                }
                else {
                    temp[y][x] = 0;
                }
            }

            else {
                if (count_adjacent == 3) {
                    temp[y][x] = 1;
                }
                else {
                    temp[y][x] = 0;
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
