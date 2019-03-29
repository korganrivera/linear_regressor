/*
 * started: Thu Feb 28 09:38:51 CST 2019
 * ended:   Thu Feb 28 18:07:09 CST 2019 (Thanks, Adderall!)
 * Takes data from csv file, gives you a linear fit function.
 * This is a rewrite of ugly code I wrote 4 years ago.
 * Korgan Rivera
 *
 * Something I need to add: I need to shift all points by the mean to make
 * calculations smaller and simpler.  E.g. take mean of all x coordinates.
 * Subtract that from all x-coords, do the fit, then add mean to x-coords in
 * the answer. Do same for all other coords.
 *
 * Also need the code not to explode when I send it a file that isn't formatted
 * in the way it wants.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "linear_algebra.h"

int solution_beautify(unsigned rows, double *b);

int main(int argc, char **argv){
    FILE *fp;
    unsigned i, j, rows = 0, cols = 0;
    char c;
    double **x;
    double *prediction, *b, *s;
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
    if((prediction = malloc(rows * sizeof(double))) == NULL) { puts("malloc failed"); exit(1); }
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
    puts("How the function will look:");
    solution_beautify(cols, b);

    // multiply x by b. Put in prediction;
    multiply_x_by_v(rows, cols, x, b, prediction);

    // free x and b.
    for(i = 0; i < rows; i++)
        free(x[i]);
    free(x);
    free(b);

    // show predicted values.
    puts("\npredicted values using this model:");
    echo_vector(rows, prediction);

    // Now calculate r^2 to measure how good the model is.

    // calculate average of s and prediction.
    double s_average = 0.0;
    double prediction_average = 0.0;
    for(i = 0; i < rows; i++){
        s_average += s[i];
        prediction_average += prediction[i];
    }
    s_average /= rows;
    prediction_average /= rows;

    // subtract s_average from each s value.
    // and prediction_average from each prediction value.
    for(i = 0; i < rows; i++){
        s[i] -= s_average;
        prediction[i] -= prediction_average;
    }

    double sp_sum = 0.0;
    double s2_sum = 0.0;
    double p_sq_sum = 0.0;
    for(i = 0; i < rows; i++){
        sp_sum += s[i] * prediction[i];
        s2_sum += s[i] * s[i];
        p_sq_sum += prediction[i] * prediction[i];
    }

    free(prediction);
    free(s);

    double r_2 = sp_sum / sqrt(s2_sum * p_sq_sum);
    printf("r² = %lf\n", r_2);
}


int solution_beautify(unsigned rows, double *b){
    if(rows == 0)
        return 0;
    assert(b != NULL && "solution_beautify(): pointer is NULL. Why?");
    if(rows == 1)
        printf("f() = %lf\n", b[0]);
    else if(rows == 2)
        printf("f(x) = %.3lfx + %.3lf\n", b[0], b[1]);
    else if(rows == 3)
        printf("f(x, y) = %.3lfx + %.3lfy + %.3lf\n", b[0], b[1], b[2]);
    else if(rows == 4)
        printf("f(x, y, z) = %.3lfx + %.3lfy + %.3lfz + %.3lf\n", b[0], b[1], b[2], b[3]);
    else{
        printf("f(v₀, v₁, v₂, ...) = %.3lfv₀ + %.3lfv₁ + %.3lfv₂ + ... + %.3lf\n", b[0], b[1], b[2], b[rows - 1]);
    }
    return 1;
}
