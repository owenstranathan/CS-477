// conv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <thread>
#include <future>
#include <iostream>
#include "../../external/include/cs477.h"
#include "../../external/include/image.h"

using namespace cs477;

void printm(matrix&);


// ******************************************************************************************************* //

//Factored weight and finding y out of convolution algorithm because this only 
// needs to be done once per image, not on every thread

matrix get_y(matrix & x, const matrix &k) {
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
	return y;
}

int get_weight(const matrix& k) {
	int weight = 0;
	for (unsigned row = 0; row < k.rows; row++)
	{
		for (unsigned col = 0; col < k.cols; col++)
		{
			weight += k(row, col);
		}
	}
	return weight;
}

// ******************************************************************************************************* //



// ******************************************************************************************************* //
// Modified version of your original convolution algorithm,
// Takes a weight a y and a starting and ending row
// basically each thread only works on a small view of the original image

void conv(matrix &x, const matrix &k, const matrix& y, const int weight, int start_row, int stop_row)
{
	// Do the convolution
	// for every row in the original image
	for (unsigned row = start_row; row < x.rows; row++)
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

// ******************************************************************************************************* //

// UNCHANGED
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

// UNCHANGED
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
	

	// div size
	int division_size = bmp.rows / concurrency;
	
	auto start = now();
	
	matrix y = get_y(bmp, kernel);
	int weight = get_weight(kernel);
	
	std::vector<future<void>> ftrs;
	for (unsigned i = 0; i < concurrency; ++i) {
		auto ftr = queue_work([&] {
			conv(bmp, kernel, y, weight, division_size * i, division_size*(i + 1));
		});
		ftrs.push_back(std::move(ftr));
	}

	// Why is this wrong??
	// when_all(futures.begin(), futures.end()).get();
	
	// Had to do this!!, LAME!
	for ( auto && ftr : ftrs) {
		ftr.wait();
	}

	auto stop = now();
	printf("%g\n", to_seconds(start, stop));
	save_png(bmp, "nature_blur.png");
	getchar();
    return 0;
}





void printm(matrix & m) {
	for (unsigned r = 0; r < m.rows; ++r) {
		for (unsigned c = 0; c < m.cols; ++c) {
			printf(" %d |", m(r, c));
		}
		printf("\n");
	}
}