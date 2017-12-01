#include "StdAfx.h"
#include "Matr.h"
#include <random>
#include <stdio.h>


Matr::Matr(int rows, int cols) : rows(rows), cols(cols) {
    arr = new long double[rows * cols];
}

long double &Matr::operator()(const int &row, const int &col) {
    return this->arr[row * cols + col];
}

void Matr::print() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%.6Lf ", arr[i * cols + j]);
        }
        printf("\n");
    }
}

long double *Matr::getAsArray() {
    return this->arr;
}



Matr::Matr(void)
{
}


Matr::~Matr(void)
{
}
