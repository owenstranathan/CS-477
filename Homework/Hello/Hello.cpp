// Hello.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../external/include/cs477.h"
#include "../../external/include/future.h"

int main()
{
	auto ftr = cs477::queue_work([]{
		printf("Hello from another thread!");
	});
	printf("Hello from the main thread");
	ftr.wait();
    return 0;
}

