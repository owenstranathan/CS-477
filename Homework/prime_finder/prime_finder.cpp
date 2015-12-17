// prime_finder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../prime_driver/primes_shared.h"

#include "../../external/include/cs477.h"


//std::shared_ptr<cs477::shm_pool> mem_pool;
std::shared_ptr<cs477::bounded_queue<int, NUM_PRIMES / PRIMES_RANGE>> driver_send_queue;
std::shared_ptr<cs477::bounded_queue<std::array<int, PRIMES_RANGE>, NUM_PRIMES / PRIMES_RANGE>> driver_recieve_queue;

using future_of_vector_of_ints = cs477::future<std::vector<int>>;

bool is_prime(int n)
{
	auto j = static_cast<int>(sqrt(n));
	for (int i = 2; i <= j; i++)
	{
		if (n % i == 0) return false;
	}
	return true;
}

std::vector<int>& find_primes_on_range_async(int start, int range) {
	unsigned concurrency = std::thread::hardware_concurrency() * 5; // YAY MICROSOFT
	// ugly as hell
	std::vector<future_of_vector_of_ints> futures;
	// number of primes to calculate per thread
	unsigned unit_per_thread = range / concurrency;
	// queue up all threads
	for (unsigned i = start; i < concurrency; ++i) {
		// push the result of the thread work to the vector
		futures.push_back(cs477::queue_work([&] {
			std::vector<int> primes;
			// the index to start at
			unsigned startIndex = start + (i*unit_per_thread);
			// the index to stop at
			unsigned endIndex = start + ((i + 1)*unit_per_thread);
			for (startIndex ; startIndex < endIndex ; ++startIndex) {
				if (is_prime(startIndex)) {
					primes.push_back(startIndex);
				}
			}
			return primes;
		}));
	}
	std::vector<int> primes;
	for (auto && ftr : futures) {
		auto result = ftr.get();
		primes.insert(primes.end(), result.begin(), result.end());
	}
	
	return primes;
}



int main()
{
	using namespace cs477;


	driver_send_queue = std::make_shared<bounded_queue<int, NUM_PRIMES / PRIMES_RANGE>>();
	driver_send_queue->create("driver_send_queue");

	driver_recieve_queue = std::make_shared<bounded_queue<std::array<int, PRIMES_RANGE>, NUM_PRIMES / PRIMES_RANGE>>();
	driver_recieve_queue->create("driver_recieve_queue");


	for (int i = 0; i < NUM_PRIMES; i += 1000) {
		int start = driver_send_queue->read();
		auto primes = find_primes_on_range_async(start, PRIMES_RANGE);
		std::array<int, PRIMES_RANGE> return_array;
		std::copy(primes.begin(), primes.end(), return_array.begin());
		driver_recieve_queue->write(return_array);
	}

    return 0;
}

