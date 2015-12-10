#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
// std::sort, std::inplace_merge
#include <algorithm>
// std::thread::hardwareconcurrency
#include <thread>
// std::async, std::future
#include <future>
#include <cstdlib>
#include <ctime>
#include <iomanip>



template <typename T>
std::ostream & operator <<(std::ostream& o, std::vector<T> list)
{
  for( auto && a : list ){
    o << a << " ";
  }
  return o;
}

template <typename K, typename V >
std::ostream& operator << (std::ostream& o, std::unordered_map<K,V> m)
{
  o << "{ ";
  for(auto && a : m){
    o << a.first << " : " << a.second << ", ";
  }
  o << " }";
  return o;
}


template <typename T>
void merge_sort(T start, T end, const int tolerance)
{
  unsigned size = end - start;
  if(size < tolerance){
    // printf("FUCKING");
    std::sort<T>(start, end);
    return;
  }
  else{

    auto mid = start + (size/2);
    std::future<void> left = std::async(std::launch::async, merge_sort<T>, start, mid, tolerance);
    std::future<void> right = std::async(std::launch::async, merge_sort<T>, mid, end, tolerance);

    left.wait();
    right.wait();
    std::inplace_merge<T>(start, mid, end);
    // printf("SHIT");
  }
}

int main(){

  const int list_size = 1000000;
  unsigned int concurrency = std::thread::hardware_concurrency();
  concurrency = concurrency;
  std::srand(std::time(0));
  std::unordered_map<int, double> timing_map;

  std::cout << "Determining optimal concurrency for sorting " << list_size << " integers..." << std::endl;

  for(int n = 10000; n < list_size/(concurrency); n+=1000)
  {
    std::vector<int> list;

    for(int i = 0; i < list_size; ++i)
    {
      list.push_back(std::rand()%1000);
    }



    auto start = std::chrono::high_resolution_clock::now();
    // start sorting
    merge_sort(list.begin(), list.end(), n);
    // stop counting
    auto end = std::chrono::high_resolution_clock::now();
    // get seconds
    auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

    // look for the entry in the map
    auto it = timing_map.find(n);
    if(it != timing_map.end())
    {
      if(it->second > seconds)
      {
        timing_map.erase(it);
        // insert timming into map
        timing_map.insert(std::make_pair(n, seconds));
      }
    }
    // not in map so insert it
    else{
      // insert timming into map
      timing_map.insert(std::make_pair(n, seconds));
    }
  }
  int minimum_size = timing_map.begin()->first;
  double minimum_time = timing_map.begin()->second;
  for(auto && a : timing_map)
  {
    if(a.second < minimum_time)
    {
      minimum_time = a.second;
      minimum_size = a.first;
    }
  }
  std::cout << "Optimal vector size to begin sorting serialy is "
            << minimum_size << " which takes " << minimum_time
            << " milliseconds to sort a vector of size " << list_size << std::endl
            << "Which means you're running " << list_size/minimum_size << " threads when you sort " << std::endl;
  if((list_size/minimum_size) == concurrency)
  {
    std::cout << "That's hilarious, we went through all that to find out that the optimal number of threads"
    << " was the hardware_concurrency\n";
  }
}
