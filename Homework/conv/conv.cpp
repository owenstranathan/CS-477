// conv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <thread>
#include <iostream>
#include "../../external/include/cs477.h"
#include "../../external/include/image.h"

using namespace cs477;

void printm(matrix&);
void split_matrix(matrix orig, matrix & one, matrix& two);
matrix join_matrix(matrix & one, matrix & two);
matrix recursively_convolute(matrix & in, matrix & kernel, unsigned concurrency);

void conv(matrix &x, const matrix &k)
{
	matrix y;
	y.create(x.rows + k.rows, x.cols + k.cols);

	for (unsigned row = 0; row < x.rows; row++)
	{
		for (unsigned col = 0; col < x.cols; col++)
		{
			auto yrow = row + k.rows / 2;
			auto ycol = col + k.cols / 2;
			y(yrow, ycol) = x(row, col);
		}
	}

	// Compute sum of k for image normalization
	int weight = 0;
	for (unsigned row = 0; row < k.rows; row++)
	{
		for (unsigned col = 0; col < k.cols; col++)
		{
			weight += k(row, col);
		}
	}

	// Do the convolution

	// for every row in the original image
	for (unsigned row = 0; row < x.rows; row++)
	{
		// for every pixel in the row
		for (unsigned col = 0; col < x.cols; col++)
		{
			int t = 0;


			auto yrow = row - k.rows / 2 + 1;
			for (int krow = k.rows - 1; krow >= 0; krow--, yrow++)
			{
				auto ycol = col - k.cols / 2 + 1;
				for (int kcol = k.cols - 1; kcol >= 0; kcol--, ycol++)
				{
					t += y(yrow, ycol) * k(krow, kcol);
				}
			}

			// perform normalization
			if (weight != 0)
			{
				t /= weight;
			}


			x(row, col) = t;
		}
	}
}


int binomial_coefficient(int n, int k)
{
	if (n <= 1 || k == 0)
	{
		return 1;
	}
	else
	{
		return binomial_coefficient(n - 1, k - 1) * n / k;
	}
}

matrix binomial(int n)
{
	if ((n & 1) == 0)
	{
		throw std::invalid_argument("n must be odd");
	}

	matrix x, y;
	x.create(1, n);
	y.create(n, 1);

	for (int i = 0; i < n / 2; i++)
	{
		x(0, i) = x(0, n - i - 1) = binomial_coefficient(n - 1, i);
		y(i, 0) = y(n - i - 1, 0) = binomial_coefficient(n - 1, i);
	}

	x(0, n / 2) = binomial_coefficient(n - 1, n / 2);
	y(n / 2, 0) = binomial_coefficient(n - 1, n / 2);

	return y * x;
}




int main()
{
	unsigned concurrency = std::thread::hardware_concurrency();
	auto bmp = load_image("nature.jpg");
	auto orig = bmp;

	//get dat kernel
	matrix kernel = binomial(9);
	printm(kernel);

	// queue up some work son
	auto start = now();
	bmp = queue_work(recursively_convolute(bmp, kernel, concurrency)).get();
	auto stop = now();
	printf("%g\n", to_seconds(start, stop));
	save_png(orig - bmp, "diff.png");
	save_png(bmp, "nature_blur.png");
	getchar();
    return 0;
}

matrix recursively_convolute(matrix & in, matrix & kernel, unsigned concurrency) {
	if (concurrency <= 1) {
		conv(in, kernel);
		return in;
	}
	else {
		concurrency /= 2;
		matrix first, second;
		split_matrix(in, first, second);
		//std::vector<future<matrix>> futes;
		first = queue_work(recursively_convolute(first, kernel, concurrency)).get();
		second = queue_work(recursively_convolute(second, kernel, concurrency)).get();
		in = join_matrix(first, second);
		return in;
	}
}

void split_matrix(matrix orig, matrix & one, matrix& two) {


	std::cout << "the original has: " << orig.rows << " rows\n";

	unsigned one_rows = orig.rows / 2;

	std::cout << "one has: " << one_rows << " rows\n";
	unsigned two_rows = orig.rows - one_rows;
	std::cout << "two has: " << two_rows << " rows\n";

	std::cout << "There are " << orig.cols << " columns\n";
	getchar();
#if 1
	one.create(one_rows, orig.cols);
	two.create(two_rows, orig.cols);
	for (unsigned i = 0; i < one_rows; ++i) {
		for (unsigned j = 0; j < orig.cols; ++j) {
			//std::cout << "the value at one(" << i << ", " << j << ") is being set\n";
			one(i, j) = orig(i, j);
		}
	}
	for (unsigned i = 0; i < two_rows; ++i) {
		for (unsigned j = 0; j < orig.cols; ++j) {
			//std::cout << "the value at two(" << i << ", " << j << ") is being set\n";
			two(i, j) = orig(i + one_rows, j);
		}
		//std::cout << i + one_rows<< std::endl;
	}
#endif
}

matrix join_matrix(matrix & one, matrix & two) {
	matrix ret;
	assert(one.cols == two.cols);
	ret.create(one.rows + two.rows, one.cols);
	for (unsigned i = 0; i < ret.rows; ++i) {
		for (unsigned j = 0; j < one.cols; ++j) {
			if (i < one.rows) {
				ret(i, j) = one(i, j);
			}
			else {
				ret(i, j) = two(i, j);
			}
		}
	}
	return ret;
}

void printm(matrix & m) {
	for (unsigned r = 0; r < m.rows; ++r) {
		for (unsigned c = 0; c < m.cols; ++c) {
			printf(" %d |", m(r, c));
		}
		printf("\n");
	}
}