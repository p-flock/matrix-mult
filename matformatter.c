#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

int
main (void) {
    int MAX_ROWS = 100;
    int MAX_COLS = 100;
    int num_inputs = 1;
    int MAX_BUFFER = 10000;
    int MAX_LINE = 200;
    int array_A[MAX_ROWS][MAX_COLS];
    char *array_A_input;
    int current_row = 0;
    int current_col = 0;
    /*int num_cols = 0;*/
    int A_length = 0;
    int A_width = 0;
    int B_length = 0;
    int B_width = 0;
    int token_count = 0;
    int is_first_row = 1;

    // parese matrix input from STDIN
    while(num_inputs <= 1) {
        char line[MAX_BUFFER];
        char *token;

        /*
         *if (is_first_row == 1)
         *    printf("enter input array number %d:\n", num_inputs);
         */
        fgets(line, MAX_LINE, stdin);
        /*printf("%s\n", line);*/
        if (strcmp(line, "\n") == 0) {
                A_length = current_row;
                A_width = current_col;
                break;
            is_first_row = 1;
            token_count = 0;
            num_inputs++;
            current_row = 0;
            current_col = 0;
        }else{
            token = strtok(line, " ");
            current_col = 0;
            while(token != NULL) {
                // make sure to keep count of both rows and columns so you know the
                // array size
                if (num_inputs == 1) {
                    array_A[current_row][current_col] = atoi(token);
                }
                current_col++;
                /*printf("row / column = %d / %d = %d\n", current_row, current_col, atoi(token));*/
                token = strtok(NULL, " ");
            }
            current_row++;
            /*printf("token count = %d\n", token_count);*/
            /*printf("column_count = %d\n", current_col);*/
            if (is_first_row == 1) {
                token_count = current_col;
                is_first_row = 0;
            }
            else{
                if (token_count != current_col) {
                    printf("wrong number of columns in matrix row\n");
                    exit(1);
                }
                if (current_row > MAX_ROWS || current_col > MAX_COLS) {
                    printf("matrix bigger than Maximum allowed (100x100)\n");
                    exit(1);
                }
            }
        }
    }


    int x = 0;
    int y = 0;
    /*
     *for (x = 0; x < A_length; x++) {
     *    for (y = 0; y < A_width; y++) {
     *        printf("%d ",array_A[x][y] );
     *    }
     *    printf("\n");
     *}
     */
    // transpose this matrix
    int transpose[A_width][A_length];
    for (x = 0; x < A_length; x++) {
        for (y = 0; y < A_width; y++) {
            transpose[y][x] = array_A[x][y];
        }
    }
    for (x = 0; x < A_width; x++) {
        for (y = 0; y < A_length; y++) {
            printf("%d ", transpose[x][y] );
        }
        printf("\n");
    }

}
