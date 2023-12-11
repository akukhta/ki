#include <iostream>
#include <new>
#include "../queue/SynchronizedQueue.hpp"

int main(int argc, char ** argv)
{
	SynchronizedQueue q;
	std::cout << std::hardware_destructive_interference_size << std::endl;
	return 0;
}
