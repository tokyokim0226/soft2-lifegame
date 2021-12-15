#include <bits/pthreadtypes-arch.h>
#include<stdio.h>
#include <stdlib.h>

int getnumber(); 

void print_selection_screen();

int main() {

    print_selection_screen();

    int input = 0;
    //call a function to input number from key board
    input = getnumber();
    
    //when input is not in the range of 1 to 9,print error message
    while (!((input <= 9) && (input >= 1))) {
        input = 0;
        input = getnumber();
    }
    
    //this function is repeated until a valid input is given by user.
    printf("The number you entered is %d", input);
    return 0;
}      

//this function returns the number given by user
int getnumber() {    
    int number;
    //asks user for a input in given range printf(" Enter a number between 1 to 9 ");
    scanf("%d", &number);
    return (number);
}

void print_selection_screen() {
    printf("\r\n\n********************************************\r\n");
    printf("**                                        **\r\n");
    printf("**   Select Lifegame Mode:                **\r\n");
    printf("**                                        **\r\n");
    printf("**   1. Standard Lifegame                 **\r\n");
    printf("**   2. Antilife (B0123478/S01234678)     **\r\n");
    printf("**   3. Seeds (B2/S)                      **\r\n");
    printf("**                                        **\r\n");
    printf("**                                        **\r\n");
    printf("********************************************\r\n\n\n");
    
}