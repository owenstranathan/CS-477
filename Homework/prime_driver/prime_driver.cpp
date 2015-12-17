// prime_driver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "primes_shared.h"
#include "../../external/include/cs477.h"
#include <iostream>


//std::shared_ptr<cs477::shm_pool> mem_pool;
std::shared_ptr<cs477::bounded_queue<int, NUM_PRIMES/PRIMES_RANGE>> driver_send_queue;
std::shared_ptr<cs477::bounded_queue< std::array<int, PRIMES_RANGE>, NUM_PRIMES/PRIMES_RANGE>> driver_recieve_queue;

std::ostream& operator<<(std::ostream& o, std::array<int, PRIMES_RANGE> arr) {
	for (auto && element : arr) {
		o << element << ", ";
	}
}

int main()
{
	using namespace cs477;
	
	driver_send_queue = std::make_shared<bounded_queue<int, NUM_PRIMES / PRIMES_RANGE>>();
	driver_send_queue->create("driver_send_queue");

	driver_recieve_queue = std::make_shared<bounded_queue<int[1000], NUM_PRIMES / PRIMES_RANGE>>();
	driver_recieve_queue->create("driver_recieve_queue");

	for (int i = 0; i < NUM_PRIMES; i+=PRIMES_RANGE) {
		driver_send_queue->write(i);
	}


	for (;;) {
		auto primes = driver_recieve_queue->read();
		std::cout << primes << '\n';
	}


    return 0;
}

