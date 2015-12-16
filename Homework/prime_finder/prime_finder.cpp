// prime_finder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../external/include/cs477.h"

const int NUM_PRIMES = 1000000;
const int PRIMES_RANGE = 1000;

//std::shared_ptr<cs477::shm_pool> mem_pool;
std::shared_ptr<cs477::bounded_queue<std::tuple<int, int>, NUM_PRIMES / PRIMES_RANGE>> driver_send_queue;
std::shared_ptr<cs477::bounded_queue< int[1000], NUM_PRIMES / PRIMES_RANGE>> driver_recieve_queue;

void find_primes_on_range(std::tuple<int, int> t, int array[1000]) {
	for (int i = 0; i < 1000; i++)
}

int main()
{
	using namespace cs477;


	driver_send_queue = std::make_shared<bounded_queue<std::tuple<int, int>, NUM_PRIMES / PRIMES_RANGE>>();
	driver_send_queue->create("driver_send_queue");

	driver_recieve_queue = std::make_shared<bounded_queue<int[1000], NUM_PRIMES / PRIMES_RANGE>>();
	driver_recieve_queue->create("driver_recieve_queue");


	for (int i = 0; i < NUM_PRIMES; i += 1000) {
		std::tuple<int, int> t = driver_send_queue->read();

	}

    return 0;
}

