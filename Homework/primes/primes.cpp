// primes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>

#include "../../external/include/cs477.h"

using namespace cs477;

bool is_prime(int n)
{
	auto j = static_cast<int>(sqrt(n));
	for (int i = 2; i <= j; i++)
	{
		if (n % i == 0) return false;
	}
	return true;
}


int main()
{
	std::vector<future<int>> fvec;
	for (int i = 2; i < 100000000; i++)
	{
		cs477::promise<int> prom;

		cs477::queue_work([prom,i]()
		{

			if (is_prime(i))
			{
				prom.set(i);
			}
		});
		fvec.push_back(prom.get_future());
	}

	future<std::vector<future<int>>> f = when_all(fvec.begin(), fvec.end());

	std::vector<future<int>> ff = f.get();

	for(auto && ftr : ff)
	{
		printf("%d", ftr.get());
	}

    return 0;
}

