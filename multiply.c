#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

#define FTOK_ID 200

int main(int argc, char* argv[]) {
    // multiply a row of matrix A by a column of matrix B

    /*printf("in multiply with %d arguments\n", argc);*/

    int SPACE_BETWEEN_MATRICES = 0;
    int shmid = atoi(argv[0]);

    /*
     *printf("shmid = %d\n", shmid);
     *int argument = 0;
     *while (argv[argument] != NULL) {
     *    printf("argv[%d] = %s\n", argument, argv[argument]);
     *    argument++;
     *}
     */

    /*get shared memory region*/
    /*printf("about to attach shared mem\n");*/
    int SHARED_MEM_SIZE = atoi(argv[4]);
    /*printf("SHARED_MEM_SIZE = %d\n", SHARED_MEM_SIZE);*/
    /*printf("SHARED_MEM_SIZE = %s\n", argv[4]);*/
    int *ptr;
    /*int (*shmptr)[10];*/
    int *shmptr;
    key_t shm_key;

    shm_key = ftok("/home/ugrad/pflock/410", 'R');
    /*printf("ftok generated key = %d\n", shm_key);*/
    /*shm_key = FTOK_ID;*/
    if ((shmid = shmget(shm_key, 0, 0666)) < 0){
        printf("shmget error\n");
        /*exit(1);*/
    }
    if ((shmptr = shmat(shmid, NULL, 0)) == (void *)-1){
        printf("shmat error\n");
        /*exit(1);*/
    }
    /*printf("shared memory attached from %p to %p in multiply\n shmid = %d\n", (void *)shmptr,*/
            /*(void *)shmptr+SHARED_MEM_SIZE, shmid);*/

    /*fflush(stdout);*/

    // get values from shared mem segment
    // == TODO ==
    // grab correct values from two input matrices and multiply
    // then put in correct spot in output matrix
    int x = atoi(argv[2]);
    int y = atoi(argv[3]);
    int width_of_A = atoi(argv[5]);
    int length_of_A = atoi(argv[6]);
    int width_of_B = atoi(argv[7]);
    int length_of_B = atoi(argv[8]);

    /*int val = shmptr[x + 12*y];*/

    /*printf("value at matrixA[%d][%d] = %d\n", x, y, val);*/
    /*printf("A[%d][%d] B[%d][%d]\n", length_of_A, width_of_A, length_of_B, width_of_B);*/

    /*printf("about to do matrix_mult\n");*/
    // iterate through row of column A and row of column B and multiply each value
    int i; // index in row/column depending on matrix A or B
    int A_val, B_val, sum;
    sum = 0;
    A_val = 0;
    B_val = 0;


    /*
     *int k;
     *for (i = 0; i < length_of_A; i++) {
     *    for (k = 0; k < width_of_A; k++) {
     *        printf("A[%d][%d] = %d ", i, k, shmptr[k + width_of_A*i]);
     *    }
     *    printf("\n");
     *}
     *for (i = 0; i < length_of_B; i++) {
     *    for (k = 0; k < width_of_B; k++) {
     *        printf("%d ", (length_of_A * width_of_A * sizeof(int)) + SPACE_BETWEEN_MATRICES + (k + width_of_B*i));
     *    }
     *    printf("\n");
     *}
     */

    for (i = 0; i < width_of_A; i++) {
         /*get matrix[A]*/
        A_val = shmptr[i + width_of_A*x];
        /*printf("value at A[%d][%d] = %d\n", x, i, A_val);*/
        B_val = shmptr[(length_of_A * width_of_A * sizeof(int)) +
            SPACE_BETWEEN_MATRICES +
            (y + width_of_B*i)];
        /*printf("value at B[%d][%d] = %d\n", i, y, B_val);*/
        sum = sum + (A_val * B_val);
    }
        /*printf("multiplying matrix A row %d by matrix B column %d, result = %d\n", x, y, sum);*/
     /*set x, y coordinate of output matrix to the sum of the row/column*/

    shmptr[ (length_of_A * width_of_A * sizeof(int)) + SPACE_BETWEEN_MATRICES +
        (length_of_B * width_of_B * sizeof(int)) + SPACE_BETWEEN_MATRICES +
        (y + (length_of_A*x))] = sum;

    /*printf("output value at C[%d][%d] = %d\n", x, y, sum);*/

    /*
     *int val;
     */
    /*
     *for (x = 0; x < length_of_B; x++) {
     *    for (y = 0; y < width_of_B; y++) {
     *        val = shmptr[(length_of_A * width_of_A * sizeof(int)) +
     *            SPACE_BETWEEN_MATRICES +
     *            (y + (length_of_B*x))];
     *        printf("%d ", val);
     *    }
     *    printf("\n");
     *}
     */

    /*printf("detaching from shared memory from process %d, %d\n", x, y);*/
    if (shmdt(shmptr) < 0 ) {
        printf("shmdt error\n");
        /*exit(0);*/
    }
    /*printf("after attach/detach\n");*/
    fflush(stdout);

    return 0;

}
