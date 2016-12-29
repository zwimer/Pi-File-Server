#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <functional>
#include <utility>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main ()
{
	using namespace boost::interprocess;

	//Remove shared memory on construction and destruction
	struct shm_remove
	{
		shm_remove() { shared_memory_object::remove("MySharedMemory"); }
		~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
	} remover;

	//Shared memory front-end that is able to construct objects
	//associated with a c-string. Erase previous shared memory with the name
	//to be used and create the memory segment at the specified address and initialize resources
	managed_shared_memory segment
		(create_only
		 ,"MySharedMemory" //segment name
		 ,65536);          //segment size in bytes

	//Alias an STL compatible allocator of for the set.
	//This allocator will allow to place containers
	//in managed shared memory segments
	typedef allocator<int, managed_shared_memory::segment_manager> ShmemAllocator;

	//Alias a set of ints that uses the previous STL-like allocator.
	//Note that the third parameter argument is the ordering function
	//of the set, just like with std::set, used to compare the keys.
	typedef set<int, std::less<int>, ShmemAllocator> Mset;

	//Initialize the shared memory STL-compatible allocator
	ShmemAllocator alloc_inst (segment.get_segment_manager());

	//Construct a shared memory set.
	//Note that the first parameter is the comparison function,
	//and the second one the allocator.
	//This the same signature as std::set's constructor taking an allocator
	Mset *myset =
		segment.construct<Mset>("My set")      //object name
		(std::less<int>() //first  ctor parameter
		 ,alloc_inst);     //second ctor parameter

	//Fork
	int rc = fork();
	if (rc < 0) { std::cerr << "fork() failed\n"; return 0; }

	//Parent, Insert data in the set
	else if (rc) myset->insert(rc);
	
	//Child
	else {

		sleep(1);
		auto ms = *(segment.find<Mset>("My set").first);
		printf("ms.size() = %d\n", (int)ms.size());
		if (ms.size()) printf("%d = %d\n", (unsigned int)getpid(), (unsigned int)*ms.begin());

	}

	//Exit
	perror("");	
	return 0;
}
