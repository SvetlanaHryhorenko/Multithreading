#pragma once
class Matr
{
public:

	private:
    long double *arr;
    int rows;
    int cols;
public:
    Matr(int rows, int cols);

    long double &operator()(const int &row, const int &col);

    void print();

    long double *getAsArray();

	Matr(void);
	~Matr(void);
};

