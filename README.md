## Parallel Jacobi Method

This repository contains the final project for "__Parallel and Distributed Systems: Paradigms and Models__" (a.y. 2021/2022)
course at University of Pisa.

### Problem Description

The Jacobi iterative method computes the result of a system of equations Ax = B (with x vector of
variable of length n, A matrix of coefficients of dimension n by n and B vector of known terms of length
n) iteratively computing a new approximation of the values of the different variables according to the
formula:

$$
x_i^{(k + 1)} = \frac{1}{a_{ii}}(b_i - \sum_{j != i}a_{ij} * x_j^k)
$$

starting from some initial assignment of each xi (e.g. 0).

We require to implement the Jacobi method with both native C++ threads and FastFlow.

### Usage

The program expects four positional arguments:

* __size__: how big the matrix will be.
* __nw__: how many workers will be used in the parallel computation.
* __seed__: pseudo-random generator's seed used to generate diagonally dominant matrices.
* __iterations__: how many iterations the solver will perform.

This is a sample run on my machine:
```shell
./jacobi 4096 8 42 1000
```
It runs the Jacobi solver using a matrix of size 4096 x 4096 doubles with 8 parallel workers.

### Report

The report will contain information about the project's performances (such as the speedup) with some charts.

### Tests

No tests have been performed, yet.

### Dependencies

The project has three dependencies. Each of them is downloaded using CMake's FetchContent module.

* [argparse](https://github.com/p-ranav/argparse.git)
* [fmt](https://github.com/fmtlib/fmt.git)
* [FastFlow](https://github.com/fastflow/fastflow.git)
* CMake 3.23

### License

MIT License

Copyright (c) 2022 Gabriele Pappalardo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.