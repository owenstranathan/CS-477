// primes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <thread>
#include <typeinfo>


#include "../../external/include/cs477.h"

using namespace cs477;

using namespace std;

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
	unsigned concurrent_threads = thread::hardware_concurrency();
	
	std::vector<future<std::vector<int>>> fvec;
	for (unsigned i = 1; i <= concurrent_threads ; i++)
	{
		printf("%d\n", concurrent_threads);
		fvec.push_back(queue_work([i]() -> std::vector<int>
		{
			std::vector<int> primes;
			for (unsigned j = 0; j < 1000000; j+=i)
			{
				if (is_prime(i))
				{
					primes.push_back(i);
				}
			}
			return primes;
		}));
	}

	future<std::vector<future<std::vector<int>>>> f = when_all(fvec.begin(), fvec.end());

	std::vector<future<std::vector<int>>> ff = f.get();

	for(auto && ftr : ff)
	{
		std::vector<int> v = ftr.get();
		for (auto && i : v)
		{
			printf("%d", i);
		}
	}

	char i;
	cin >> i;

    return 0;
}