# Linear regressor

Reads data from csv file and performs a linear regression on it if possible.

## How to compile
type `make`

## Usage
`./linear_fit <csv file>`

## Example
![screenshot](linear_fit_screenshot.png)

## csv file format
every number will end with a comma. Each line except the last will end with a newline. See test.csv for an example.

## What use is this?
If you have data that you need to fit a linear model to, then this should do it.

## How do I interpret the output?
The solution is a linear function of the form:

![f(v_1, v_2, v_3, ..., v_n) = C_1*v_1 + C_2*v_2 + C_3*v_3 + ... + C_n*v_n + C_0](linear_function.png)

where the v's are your variables and the C's are the constants that give you your solution.

Here's a 3D plot of the above function:

![3D model](3d_graph.png)

## It didn't work when I tried it.
I mean, what do you want from me?

I haven't stress-tested this at all. It'll probably fail with large numbers and/or large data sets. It works pretty well for me.

## TO-DO
* I'll include polynomial regression later.
* make a smarter makefile later.
