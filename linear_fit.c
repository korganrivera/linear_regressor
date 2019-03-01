/*
 * started: Thu Feb 28 09:38:51 CST 2019
 * ended:   Thu Feb 28 18:07:09 CST 2019 (Thanks, Adderall!)
 * Takes data from csv file, gives you a linear fit function.
 * This is a rewrite of ugly code I wrote 4 years ago.
 * Korgan Rivera
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linear_algebra.c"

int main(int argc, char **argv){
    FILE *fp;
    unsigned i, j, rows = 0, cols = 0;
    char c;
    double **x;
    double *temp_x1, *temp_x2, *b, *s;
    double **x_trans, **trans_mult, **trans_mult2;

    // check command line input.
    if(argc != 2){
        printf("USAGE: %s <csv file>\n", argv[0]);
        puts("Your csv file should have the following format:");
        puts("1.0, 2.3, 3.6,");
        puts("2.1, -0.9, 4.5,");
        puts("3,3, -3.5, 5.1,");
        puts("5.0, -8.8, 7.0,");
        puts("\nIf you have x columns, you need at least x rows.");
        puts("if rows = cols, you will get a perfect fit.");
        puts("if rows > cols, you will get a linear regression fit.");
        puts("your csv file should have NO TRAILING NEWLINE.");
        exit(0);
    }

    // open given file.
    if((fp = fopen(argv[1],"r")) == NULL){
        printf("I can't open %s.\n", argv[1]);
        exit(1);
    }

    // count rows in file.
    while((c = fgetc(fp)) != EOF)
        if(c == '\n')
            rows++;
    rewind(fp);

    // count columns in file.
    while((c = fgetc(fp)) != '\n')
        if(c == ',')
            cols++;
    rewind(fp);

    /* rows must be >= cols, otherwise you don't have enough data to make a
     * model.  e.g. you can't fit a line to a single point; you can't fit a
     * surface to only two points.
     */
    if(rows < cols){
        printf("PROBLEM-your data has %u rows and %u columns.\n", rows, cols);
        printf("You need >= %u rows to fit %u-dimensional data.", cols, cols);
        exit(0);
    }

    // malloc space for matrix x.
    if((x = malloc(rows * sizeof(double*))) == NULL){ printf("malloc failed.\n"); exit(1); }
    for(i = 0; i < rows; i++)
        if((x[i] = malloc(cols * sizeof(double))) == NULL){ printf("malloc failed.\n"); exit(1); }

    // read values from file into array.
    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            fscanf(fp, "%lf,", &x[i][j]);
        }
        fgetc(fp); // skip over newline.
    }
    fclose(fp);

    // malloc space for vectors.
    if((temp_x1 = malloc(rows * sizeof(double))) == NULL) { puts("malloc failed"); exit(1); }
    if((temp_x2 = malloc(rows * sizeof(double))) == NULL) { puts("malloc failed"); exit(1); }
    if((b =       malloc(cols * sizeof(double))) == NULL) { puts("malloc failed"); exit(1); }
    if((s =       malloc(rows * sizeof(double))) == NULL) { puts("malloc failed"); exit(1); }

    // malloc space for matrices.
    if((x_trans = malloc(cols * sizeof(double*))) == NULL){ puts("malloc failed"); exit(1); }
    for(i = 0; i < cols; i++)
        if((x_trans[i] = malloc(rows * sizeof(double))) == NULL){ puts("malloc failed"); exit(1); }

    if((trans_mult = malloc(cols * sizeof(double*))) == NULL){ puts("malloc failed"); exit(1); }
    for(i = 0; i < cols; i++)
        if((trans_mult[i] = malloc(cols * sizeof(double))) == NULL){ puts("malloc failed"); exit(1); }

    if((trans_mult2 = malloc(cols * sizeof(double*))) == NULL){ puts("malloc failed"); exit(1); }
    for(i = 0; i < cols; i++)
        if((trans_mult2[i] = malloc(rows * sizeof(double))) == NULL){ puts("malloc failed"); exit(1); }

    // check array that was read from file.
    puts("your data from file:");
    echo_array(rows, cols, x);

    // copy right-most column of x into s.
    for(i = 0; i < rows; i++)
        s[i] = x[i][cols - 1];

    // make right-most column of x all 1s.
    for(i = 0; i < rows; i++)
        x[i][cols - 1] = 1.0;

    // transpose x into x_trans.
    transpose(rows, cols, x, x_trans);

    // multiply x_trans by x, put into trans_mult.
    multiply(cols, rows, rows, cols, x_trans, x, trans_mult);

    // invert trans_mult.
    invert(cols, trans_mult);

    // multiply trans_mult by x_trans. Put into trans_mult2.
    multiply(cols, cols, cols, rows, trans_mult, x_trans, trans_mult2);

    // free x_trans and trans_mult.
    for(i = 0; i < cols; i++){
        free(x_trans[i]);
        free(trans_mult[i]);
    }
    free(x_trans);
    free(trans_mult);

    // multiply trans_mult2 by s, put into b.
    multiply_x_by_v(cols, rows, trans_mult2, s, b);

    // free trans_mult2.
    for(i = 0; i < cols; i++)
        free(trans_mult2[i]);
    free(trans_mult2);

    // show solution.
    puts("solution vector:");
    echo_vector(cols, b);

    // multiply x by b. Put in temp_x1;
    multiply_x_by_v(rows, cols, x, b, temp_x1);

    // free x and b.
    for(i = 0; i < rows; i++)
        free(x[i]);
    free(x);
    free(b);

    // show predicted values.
    puts("predicted values using this model:");
    echo_vector(rows, temp_x1);

    // Now calculate r^2 to measure how good the model is.

    // multiply temp_x1 by -1.0.
    scale_v(rows, -1.0, temp_x1);

    // temp_x2 = s - temp_x1
    add_v(rows, s, temp_x1, temp_x2);

    free(temp_x1);

    // sum squares of elements of temp_x2.
    double d = dot(rows, temp_x2, temp_x2);

    free(temp_x2);

    // calculate average of values in s.
    double average = 0.0;
    for(i = 0; i < rows; i++)
        average += s[i];
    average /= rows;

    // sum square of differences between s values and average s value.
    double f = 0.0;
    for(i = 0; i < rows; i++)
        f += (s[i] - average) * (s[i] - average);

    free(s);

    double r_2 = d / f;
    r_2 = 1.0 - r_2;

    printf("r² = %lf\n", r_2);
}
