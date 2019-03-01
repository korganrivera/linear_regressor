# Linear regressor

Reads data from csv file and performs a linear regression on it if possible.

## How to compile
`gcc linear_fit.c -o linear_fit -lm`


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

where the v's are your variables and the C's are the constants that give you your solution. When linear_fit says, for example:

```
solution vector:
1.640 -0.434 14.229
```

it's giving you the C values. For the above case, the linear function would be:

```
z = 1.64x - 0.434y + 14.229
```

Here's a 3D plot of the above function:

![3D model](3d_graph.png)

(I need to make this more explicit though, so I don't need to explain it in a README.)

## It didn't work when I tried it.
I mean, what do you want from me?

I haven't stress-tested this at all. It'll probably fail with large numbers and/or large data sets.
