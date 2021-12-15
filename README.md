# Software II Week I 課題　 - [Lifegame] 

- Kadai 1-3 are all included within `mylife3.c`. However, I will still explain the processes I used to navigate through the project step by step.

- Kadai 4 (`mylife4.c`) is an extension of Kadai 3 that accepts any lifegame rule in the format Bxxxxx/Sxxxxx. The different types of rules are referenced [here](https://www.conwaylife.com/wiki/List_of_Life-like_cellular_automata)

    (発展課題では任意のルールを`argv[2]`として入力するとそのルールの通りでゲームを実行することができるようにしました)


# Introduction

- [課題１&コードの全体的説明](#課題１) - コードの全体的の説明も含みますので長いかも知らないです。
- [課題２](#課題２) - 100世代ごとの盤面の状態をLife1.06形式でファイルに出力するようにプログラムを修正した部分のみを説明しました
- [課題３](#課題３) - 初期配置パターンをRLEフォーマットのファイルから読み込むことができるように修正した部分のみを説明しました

All the programs will be compiled in the same way:

To compile this program, input the following:
```
gcc -o mylife3 mylife3.c 
```

Depending on which file it is you want to compile, change it accordingly in the following manner:

- 課題１ - `gcc -o mylife1 mylife1.c `
- 課題２ - `gcc -o mylife2 mylife2.c` 
- 課題３ - `gcc -o mylife3 mylife3.c `

To execute the program in 課題1~3, you must pass in:
1. The output file (ex) ./mylife)
2. A file in `.lif` (or `.rle` for 課題３) format to initialize the lifegame map (if there isn't, a randomized map will be formulated)

Example codes to execute:
```
./mylife1 gosperglidergun.lif
```
```
./mylife3 gosper.rle
```


In the optional assignment (`mylife4.c`), I have made it so that the user can input a lifegame rule of their choice (in the format Bxxxxxx/Sxxxxx) and made the program be able to produce a lifegame that follows that rule. Any rule thht is in the format Bxxxx/Sxxxx (exmaples: [this page](https://www.conwaylife.com/wiki/List_of_Life-like_cellular_automata)) can be rendered.

- [発展課題](#発展課題) 

To compile this program, input the following:
```
gcc -o mylife4 mylife4.c 
```
To execute the program in 課題４, you must pass in:
1. The output code (ex) ./mylife)
2. A file in `.lif` or `.rle` format to initialize the lifegame map (if there isn't, a randomized map will be implemented)
3. The rule you want to use in `Bxxxxx/Sxxxx` format (if no rule is given, then the default Conway's lifegame rule will be implemented)

Example code to execute:

```
./mylife4 gosperglider.lif B3/S023 
```

# 課題１

Here, I have focused on defining several functions in order for the lifegame to function properly

### `void my_init_cells(const int height, const int width, int cell[height][width], FILE* fp);`
    
This function initializes the lifegame map, both when there is a given file for its last parameter.

When there is no proper file given through as the last parameter:

```
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
```

As seen here, a given cell for the lifegame grid will only be equal to 1 (which means that it is alive) when the random number it has produced is divisible by 10, which has a 10% chance. Hence, this code makes it so that there is only a 10% chance that a cell is alive. 

Moreover, `srand(time(0))` makes sure that the randomization is actually random for every trial of running the program.
    
If the function has indeed received a file that it can iterate through, the following will occur: 

```
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
```

To read a .lif format, the first line will always be in the format of #Life 1.06. We make it so that we ignore this line and continue with the code, and for each line that is in the format `x y` within the file, we will make sure that the cell corresponding to those coordinates will be set as `1`, which again, means that the cell is alive at that point

### `void my_print_cells(FILE *fp, int gen, const int height, const int width, int (*cell)[width])`

This function helps print out the lifegame grid in a visually pleasing format. The specifics can be referred to in the raw code.However, the rather logical portion in this code is to make sure the print out only the cells that are alive in the following manner:

```
for (int y = 0; y < height; ++y) {
        fprintf(fp, "|");
        for (int x = 0 ; x< width ; ++x) {
            if (cell[y][x]) {
                fprintf(fp, "\e[31m#\e[0m");
            }
            ...
```

I simply iterate through the grid for the lifegame and check for which cells are marked as `1`. If the cells are marked as `1`, they will be printed out as a red hashtag. If that is not the case, then that spot will be filled in with a space.

### `my_count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width])`

This function checks for how many of the 8 adjacent cells of a cell are currently alive. 

```
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
```

We keep in track of a count, and iterate through the 8 spaces according to their coordinates one by one, and add a count if the cell in that space is alive.

### `my_update_cells(const int height, const int width, int cell[height][width]) `

```
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
```

This is the function that keeps updating the cells for each generation. In Kadai1, it is set to follow the basic rule of the Conway Lifegame, where if 2 or 3 cells are alive around an alive cell, the cell will continue to live, and if 3 cells ar alive around a dead cell, the cell will come to life. Once we have saved the new updated status of the cell for every single cell in the lifegame grid into a temporary matrix, we will now write in our lifegame grid with the values in our temporary matrix that has stored the updated status for all the cells.

Within the main function, we initialize the formatting for our lifegame:

```
FILE *fp = stdout;
    const int height = 40;
    const int width = 70;

    int cell[height][width];
    for(int y = 0 ; y < height ; y++){
        for(int x = 0 ; x < width ; x++){
            cell[y][x] = 0;
        }
    }
```

and then we make sure to return an error if `argc >2` for the input. 
```
if ( argc > 2 ) {
        fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
        return EXIT_FAILURE;
    }
```

If `argc == 2`, then we check if the file is readable and if it is, continue to initalize the grid. If not, an error is returned.
```
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
```

If that is also not the case (when `argc == 1`), then we will simply initialize the cells using a NULL input for where the file is supposed to be, and the initialization will be random.

```
else{
    my_init_cells(height, width, cell, NULL); // デフォルトの初期値を使う
}
```

Once this has been done, we print out the grid, sleep for aecond, and then return the cursor to print properly for the next round. Then a for loop is used to continue this for however many generations until `Ctrl+C` is pressed on the keyboard:

```
my_print_cells(fp, 0, height, width, cell); // 表示する
sleep(1); // 1秒休止
fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)

/* 世代を進める*/
for (int gen = 1 ;; gen++) {
    my_update_cells(height, width, cell); // セルを更新
    my_print_cells(fp, gen, height, width, cell);  // 表示する
    sleep(1); //1秒休止する
    fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)
}

return EXIT_SUCCESS;
```
    
----------------------------------
---------------------------------
# 課題２

Within the main function where the screen is updated and printed for each new generation, I have added the following code:

```
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
```

This makes it so that whenever the generation hits a multiple of 100 (but is below 10000), a file is created with the name of `gen%04d.lif` with the numbers being filled in with the generation. The first line will be printed as `#Life 1.06` and the following lines will all be in the format `x y`, storing all the coordinates of the cells alive in that particular generation.

# 課題３

To enable reading in files in RLE format, I only had to make a slight change in the initialization function `void my_init_cells`. The changed portion is as follows:
```
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
```

An extra variable `int is_RLE` is used to check for whether the file is in rle format or not. We similarly skip all lines that start with a hashtag, as in the Life 1.06 format it is used for the first line and in the RLE format, it is simply used for comments.

The additional line of `.rle` that isn't present in `.lif` is the `x = , y = ` part. This is used to denote the dimensions of the pattern in a `.rle` format. We will render this and if this is the case, we will chagne `is_RLE` to 1. If not, it remains 0.

If `is_RLE` does happen to be 0, then we simply follow the rules of reading an RLE format. [This link](https://www.conwaylife.com/wiki/Run_Length_Encoded) explains it in detail.

Within the code, we use a for loop for the length of the buffer string that we loaded (will be the part of the `.rle` file that has the information of dead/alive cells.) We check for whether the cell is a number, $, b, o, or a !. When it is a b or a o, we simply scan across however many of those are stored within the len_buffer and repeat it for that many times.

-------------------------------
-------------------------------

# 発展課題

Here, I have extended the program to also receive a user input for the lifegame rule format that they want. For instance:

Example code to execute:

```
./mylife4 gosperglider.lif B3/S12345
```

This code will execute a lifegame on the initial map of the gosperglider, but instead of following the conventional conway gamelife rules of B3/S23, it will instead follow the rules for [Maze](https://www.conwaylife.com/wiki/OCA:Maze).

However, you can choose to either additionally input only the initial file or only the rule, or neither. 

For instance:

```
./mylife4 gosperglider.lif
```

This will render the initial grid as the file gosperglider.lif says, and will use the default rule (B3/S23)

```
./mylife4 B3/S12345
```

This will create a random map for the initial lifegame grid and use the rules for [Maze](https://www.conwaylife.com/wiki/OCA:Maze).

```
./mylife4
```

This will use a random initial grid and the default rule (B3/S23)

## **Explanation of Code**

The function: 

`void my_update_cells(const int height, const int width, int cell[height][width] ,const int rule[2][9]);` 

was updated to also include a rule that is to be considered. 

The matrix `rule[2][9]` is basically a matrix with two rows each with 9 elements. Each element indicates whether the number that element is indexed in is included in Bxxxx/Sxxxx format of indicating the rule. If there is a 1, it means that the number corresponding to that index exists. If it is a 0, it means it doesn't exist.

For instance, for our default rule B3/S23, the rule matrix will look like this:
 ```
 {0,0,0,1,0,0,0,0,0}
 {0,0,1,1,0,0,0,0,0}
 ```

 Basically, the numbers after B indicates the condition(s) for when an already alive cell should be alive, and the numbers after S indicates the condition(s) for when a dead cell should come to life.
 
 For instance, `B3` means that for any alive cell, it will continue to stay alive when there are exactly 3 alive cells around it. `S23` means that for any dead cell, if there are 2 or 3 alive cells around it, then it will also come to life.

 So once this rule is passed on into the function `void my_update_cells`, we change the code up slightly. First, we initialize a grid that is comprised of all 0s"

 ```
for (int y = 0; y<height; y++) {
    for (int x = 0; x < width; x++) {
        temp[y][x] = 0;
    }
}
```
Then, we basically iterate through the rules for `B` and `S` respectively, and make the cells that match their conditions alive.

The cells that are alive (`1`) will follow the conditions of `B`, and the cells that are dead (`0`) will follow the conditions of `S`

```
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
```

With this changed update function, we now only need to change up the main function a little bit to make use of this function.

First, we need to initialize a `rule[2][9]` within the main function that is comprised of all 0s to begin with.
```
int rule[2][9] = {{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}};
```    
Moreover, we check to see if there are more than 3 inputs this time (compared to 2 inputs in kadai1~3) as we are additionally putting in a rule.

To account for some flexibility in the input, we scan for the user's input, and we check to see if there is a rule in either the 2nd or 3rd input.

If the rule comes as the 2nd input straight after `./lifegame`, then we will use a randomized map and use the rules that were passed. If the rule comes as the 3rd input, given that the 2nd input is a correctly formatted file for the intial grid for the lifegame, it will implement the set of rules on that initial grid and for the future generations.

If there is only the input `./lifegame`, then a random map and default rules will be used.

The code to execute all this is as follows: 

```
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
```