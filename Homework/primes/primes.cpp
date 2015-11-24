// primes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <thread>
#include <typeinfo>
#include <chrono>
#include <algorithm>
#include <exception>


#include "../../external/include/cs477.h"

using namespace cs477;

using namespace std;

mutex mtex;

void dot_dot_dot();

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
	std::vector<int> primes;

#if 0
	for (auto i = 0; i < 1000000; ++i) {
		if (is_prime(i)) {
			primes.push_back(i);
		}
	}

#endif

#if 1
	int concurrent_threads = 4;
	printf("%d\n", concurrent_threads);
	std::vector<cs477::thread> threads;
	std::vector<std::vector<int>> return_vectors;
	for (int i = 0; i < concurrent_threads ; i++)
	{
		threads.push_back(cs477::create_thread([i, &return_vectors]()
		{
			try {
				std::vector<int> primes;
				for (int j = i; j < 1000000; j += 4)
				{
					if (is_prime(j))
					{
						primes.push_back(j);
					}
				}
				return_vectors.push_back(primes);
			}
			catch (exception anything) {
				printf("WTF? It didn't work? well that never happend for me");
				dot_dot_dot();
			}
		}));
	}

	for (auto && a : threads) {
		cs477::join(a);
	}
#endif
#if 1
	printf("To start printing all the primes press enter, but I warn you,\n It's gonna take awhile...");
	getchar();
	for (auto &&a : return_vectors) {
		for (auto &&e : a) {
			printf("%d\n", e);
		}
	}
#endif

	printf("That's all the primes! They were out of order, but who really cares?\n");

	dot_dot_dot();

	printf("Oh aright!\n");
	dot_dot_dot();
	printf("Let's see how long it takes to sort them with std::sort\n I'm sure it'll be faster than printing them to windows std output!\n");
	printf("Press enter to get this thing started");
	getchar();

	//Coallate the vectors
	std::vector<int> all_primes;
	for (auto && sub_vec : return_vectors) {
		for (auto && element : sub_vec) {
			all_primes.push_back(element);
		}
	}
	//start timing sort
	auto start = std::chrono::high_resolution_clock::now();
	
	//Sort the the primes
	std::sort(all_primes.begin(), all_primes.end(), [](int a, int b) { return a < b; });

	auto end = std::chrono::high_resolution_clock::now();

	printf("Ok the sorting's done, how fast was that?\n");
	dot_dot_dot();

	double time_in_seconds = std::chrono::duration_cast<chrono::seconds>(end - start).count();
	double time_in_microseconds = std::chrono::duration_cast<chrono::microseconds>(end - start).count();
	double time_in_nanoseconds = std::chrono::duration_cast<chrono::nanoseconds>(end - start).count();

	printf("That was a whopping %f seconds \nA ridiculous %f microseconds\nAnd an OUTRAGEOUS %f nanoseconds!", time_in_seconds, time_in_microseconds, time_in_nanoseconds);
	dot_dot_dot();
	printf("Pretty slow\n");
	printf("Hit enter to stop this silly nonsense!");
	getchar();
	dot_dot_dot();
	printf("What was that?!?!? I said HIT enter\nNot gingerly tap it. Now slam that thing and put a stop to this follishness!");
	getchar();

    return 0;
}

void dot_dot_dot() {
	for (auto i = 0; i < 3; i++) {
		printf(".");
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
	printf("\n");
}