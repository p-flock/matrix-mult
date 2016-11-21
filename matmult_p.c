#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

#define GROUP_READ 0040
#define GROUP_WRITE 0020
#define FTOK_ID 200

int
main (void) {
    int MAX_ROWS = 100;
    int MAX_COLS = 100;
    int num_inputs = 1;
    int MAX_BUFFER = 10000;
    int MAX_LINE = 200;
    int array_A[MAX_ROWS][MAX_COLS];
    int array_B[MAX_ROWS][MAX_COLS];
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

    // parse matrix input from STDIN
    while(num_inputs <= 2) {
        char line[MAX_BUFFER];
        char *token;

        if (is_first_row == 1)
            printf("enter input array number %d:\n", num_inputs);
        fgets(line, MAX_LINE, stdin);
        /*printf("%s\n", line);*/
        if (strcmp(line, "\n") == 0) {
            if (num_inputs == 1){
                A_length = current_row;
                A_width = current_col;
            }else {
                B_length = current_row;
                B_width = current_col;
                break;
            }
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
                }else {
                    array_B[current_row][current_col] = atoi(token);
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

    int i, k;
    int **matrixA = (int**)malloc(A_length * sizeof(int *));
    for (i = 0; i<A_length; i++)
        matrixA[i] = (int*)malloc(A_width * sizeof(int));

    int **matrixB = (int**)malloc(B_length * sizeof(int *));
    for (i = 0; i<B_length; i++)
        matrixB[i] = (int*)malloc(B_width * sizeof(int));

    /*int matrixA[100][100];*/
    /*int matrixB[100][100];*/
    /*printf("about to set matrices\n");*/
    for (i = 0; i < A_length; i++) {
        for (k = 0; k < A_width; k++) {
            matrixA[i][k] = array_A[i][k];
        }
    }
    for (i = 0; i < B_length; i++) {
        for (k = 0; k < B_width; k++) {
            matrixB[i][k] = array_B[i][k];
        }
    }

    int SPACE_BETWEEN_MATRICES = 0;

    /*printf("finding length / width\n");*/
    int length_of_A = A_length;
    int width_of_A = A_width;
    int length_of_B = B_length;
    int width_of_B = B_width;

    // check matrices for errors
    if (A_width != B_length) {
        printf("invalid matrices\n");
        exit(0);
    }

/*
 *
 *    printf("A\n");
 *    for (i = 0; i < length_of_A; i++) {
 *        for (k=0; k< width_of_A; k++) {
 *            [>matrixA[i][k] = (i+k);<]
 *            printf("%d ", matrixA[i][k]);
 *        }
 *        printf("\n");
 *    }
 *
 *    printf("*B\n");
 *    for (i = 0; i < length_of_B; i++) {
 *        for (k=0; k < width_of_B; k++) {
 *            printf("%d ", matrixB[i][k]);
 *        }
 *        printf("\n");
 *    }
 *    printf("=\n");
 *
 */
    /*printf("A[%d][%d] B[%d][%d]\n", length_of_A, width_of_A, length_of_B, width_of_B);*/
    fflush(stdout);
    int num_processes = length_of_A * width_of_B;
    /*printf("%d\n", num_processes);*/



    int INPUT_ARRAYS_SIZE = sizeof(matrixA) + sizeof(matrixB);
    // allocate shared memory for different processes
    //  Size of both input matrices plus output matrix plus 50 bytes in between each
    int SHARED_MEM_SIZE =
        ((length_of_A * width_of_A * sizeof(int)) + SPACE_BETWEEN_MATRICES
         + (length_of_B * width_of_B * sizeof(int)) + SPACE_BETWEEN_MATRICES
         + (length_of_A * width_of_B* sizeof(int)));

    int *shmptr;
    int shmid;
    key_t shm_key;

    shm_key = ftok("/home/ugrad/pflock/410", 'R');
    /*printf("ftok generated key in parent = %d\n", shm_key);*/
    if ((shmid = shmget(shm_key, sizeof(matrixA), IPC_CREAT | 0666)) < 0) {
        printf("shmget error%d\n", shmid);
        exit(1);
    }
    /*int (*shmptr)[10] = shmat(shmid, NULL, 0))*/
    if ((shmptr = shmat(shmid, NULL, 0)) == (void *)-1){
        printf("shmat error");
        exit(1);
    }
    /*
     *printf("shared memory attached from %p to %p\n shmid = %d in parent\n", (void *)shmptr,
     *        (void *)shmptr+SHARED_MEM_SIZE, shmid);
     */

    //
    // put the two arrays in the memory segment of the right size at the shared memory address

    for (i = 0; i < length_of_A; i++) {
        for (k = 0; k < width_of_A; k++) {
            shmptr[k + (width_of_A*i)] = matrixA[i][k];
            /*printf("A[%d][%d] = %d should be = %d ", i, k, shmptr[k + length_of_A*i], matrixA[i][k]);*/
            /*printf("at iteration %d/%d set address %p to %d\n", i, k, (void*)shmptr+(k + width_of_A*i), matrixA[i][k]);*/
        }
        /*printf("\n");*/
    }
    for (i = 0; i < length_of_B; i++) {
        for (k = 0; k < width_of_B; k++) {
            shmptr[(length_of_A * width_of_A * sizeof(int)) + SPACE_BETWEEN_MATRICES
                + (k + width_of_B*i)] = matrixB[i][k];

        }
    }


    //based on matrix size allocate n jobs to n processes
    pid_t pid;
    int x, y;
    for ( x = 0; x < length_of_A; x++) {
        for ( y = 0; y < width_of_B; y++){
            if (( pid = fork() ) < 0) {
                printf("fork error");
                exit(1);
            } else if (pid == 0) { // child

                // command line arguments must be strings
                char shmid_buffer[20], x_buffer[20], y_buffer[20],
                     size_buffer[20], input_A_rows_buffer[20],
                     input_A_columns_buffer[20],
                     input_B_rows_buffer[20],
                     input_B_columns_buffer[20];

                // print integers into char buffers to pass as arguments
                sprintf(shmid_buffer,  "%d", shmid);
                sprintf(x_buffer, "%d", x);
                sprintf(y_buffer, "%d", y);
                sprintf(size_buffer, "%d", SHARED_MEM_SIZE);
                sprintf(input_A_columns_buffer, "%d", width_of_A);
                sprintf(input_A_rows_buffer, "%d", length_of_A);
                sprintf(input_B_columns_buffer, "%d", width_of_B);
                sprintf(input_B_rows_buffer, "%d", length_of_B);


                if (execl("/home/ugrad/pflock/410/a2/matrix-mult/multiply",
                            "/home/ugrad/pflock/410/a2/matrix-mult/multiply",
                            shmid_buffer, x_buffer, y_buffer, size_buffer,
                            input_A_columns_buffer, input_A_rows_buffer,
                            input_B_columns_buffer, input_B_rows_buffer, (char *)0) < 0) {
                    printf("execl() error, returned to calling process\n");
                    exit(1);
                }
            } else { // in parent, wait for pid
                // do nothing
            }
        }
    }

    // wait for all children
    while (pid = waitpid(-1, NULL, 0)) {
        if (errno == ECHILD) {
            // if there are no children left, exit loop
            break;
        }
    }

    // retrieve final matrix from shared memory
    int matrixC[length_of_A][width_of_B];
    int val;
    for (x = 0; x < length_of_A; x++) {
        for (y = 0; y < width_of_B; y++) {
            val = shmptr[ (length_of_A * width_of_A * sizeof(int)) +
                SPACE_BETWEEN_MATRICES +
                (length_of_B * width_of_B * sizeof(int)) +
                SPACE_BETWEEN_MATRICES +
                (y + (length_of_A*x))];
            matrixC[i][k] = val;
            printf("%d ", val);
        }
        printf("\n");
    }

    // after everything, deallocate shared memory
    if (shmctl(shmid, IPC_RMID, 0) < 0){
        printf("shmctl error");
        exit(1);
    }

    fflush(stdout);
    return 0;
}
