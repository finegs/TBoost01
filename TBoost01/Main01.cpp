#define _DF_0 0
#define _DF_1 0
#define _DF_2 0
#define _DF_3 0
#define _DF_4 0
#define _DF_5 0
#define _DF_6 0
#define _DF_7 1


#if _DF_0

#include <iostream>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <functional>
#include <utility>


int main()
{
	using namespace boost::interprocess;

	//Remove shared memory on construction and destruction
	struct shm_remove
	{
		shm_remove() { shared_memory_object::remove("MySharedMemory"); }
		~shm_remove() { shared_memory_object::remove("MySharedMemory"); }
	} remover;

	//Shared memory front-end that is able to construct objects
	//associated with a c-string. Erase previous shared memory with the name
	//to be used and create the memory segment at the specified address and initialize resources
	managed_shared_memory segment
	(create_only
		, "MySharedMemory" //segment name
		, 65536);          //segment size in bytes

						   //Note that map<Key, MappedType>'s value_type is std::pair<const Key, MappedType>,
						   //so the allocator must allocate that pair.
	typedef int    KeyType;
	typedef float  MappedType;
	typedef std::pair<const int, float> ValueType;

	//Alias an STL compatible allocator of for the map.
	//This allocator will allow to place containers
	//in managed shared memory segments
	typedef allocator<ValueType, managed_shared_memory::segment_manager>
		ShmemAllocator;

	//Alias a map of ints that uses the previous STL-like allocator.
	//Note that the third parameter argument is the ordering function
	//of the map, just like with std::map, used to compare the keys.
	typedef map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MyMap;

	//Initialize the shared memory STL-compatible allocator
	ShmemAllocator alloc_inst(segment.get_segment_manager());

	//Construct a shared memory map.
	//Note that the first parameter is the comparison function,
	//and the second one the allocator.
	//This the same signature as std::map's constructor taking an allocator
	MyMap *mymap =
		segment.construct<MyMap>("MyMap")      //object name
		(std::less<int>() //first  ctor parameter
			, alloc_inst);     //second ctor parameter

							   //Insert data in the map
	for (int i = 0; i < 100; ++i) {
		mymap->insert(std::pair<const int, float>(i, (float)i * i));
	}

	std::cout << "Done." << std::endl;

	for (int i = 0; i < 100; ++i) {
		iterator<std::pair<const int, float>> iter = mymap->find(i);
	}

	return 0;
}
#endif

#if _DF_1

#ifndef __cplusplus
#error A C++ Compiler is required!
#endif

#line 20 "mcp"

#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

#define toStr(x) #x

struct movies_t {
	string title;
	int year;
};

int main(int argc, char *argv[]) {
	if (argc != 3) {
		cout << "Usage : " << argv[0] << " sourcefile destfile" << endl;
		return EXIT_FAILURE;
	}
	string line;
	ifstream myfile(argv[1]);
	ofstream ofile(argv[2], ios::out|ios::app);
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			ofile << line << endl;
		}
		myfile.close();
		ofile.close();
	}

	cout << "copy is done. source=" << argv[1] << ", dest=" << argv[2] << endl;

	cout << "toStr(1)=" << toStr(001) << ", " << 1 << endl;

	return EXIT_SUCCESS;
}

#endif

#if _DF_2
#include <iostream>
#include <algorithm>
#include <cstdlib>

using namespace std;

const int SIZE = 10;

int main(int argc, char* argv[]) {

	int ia[SIZE] = { 1, 1, 3, 52, 10u, 10 };

	sort(ia, ia+SIZE);

	cout << "Sort result : " << endl;

	for (size_t i = 0; i != SIZE; i++) {
		cout << ia[i] << ", ";
	}
	cout << endl;

	return EXIT_SUCCESS;
}
#endif

#if _DF_3

#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/exception/all.hpp>
#include <cstring>
#include <cstdlib>
#include <string>

int main(int argc, char *argv[])
{
	using namespace boost::interprocess;

	if (argc == 1) {  //Parent process
					  //Remove shared memory on construction and destruction
		struct shm_remove
		{
			shm_remove() { shared_memory_object::remove("MySharedMemory"); }
			~shm_remove() { shared_memory_object::remove("MySharedMemory"); }
		} remover;

		//Create a shared memory object.
		shared_memory_object shm(create_only, "MySharedMemory", read_write);

		//Set size
		shm.truncate(1000);

		//Map the whole shared memory in this process
		mapped_region region(shm, read_write);

		//Write all the memory to 1
		std::memset(region.get_address(), 1, region.get_size());

		//Launch child process
		std::string s(argv[0]); s += " child ";
		if (0 != std::system(s.c_str()))
			return 1;

		std::cout << "Parent Process is completed to set '1' to all region, then call child process in MySharedMemory" << std::endl;			std::cout << "Child Process is completed to check all values in MySharedMemory" << std::endl;

	}
	else {

		try {

			//Open already created shared memory object.
			shared_memory_object shm(open_only, "MySharedMemory", read_only);

			//Map the whole shared memory in this process
			mapped_region region(shm, read_only);

			//Check that memory was initialized to 1
			char *mem = static_cast<char*>(region.get_address());
			for (std::size_t i = 0; i < region.get_size(); ++i)
				if (*mem++ != 1)
					return 1;   //Error checking memory


		} catch (const std::exception& ex) {
			std::cerr << "Exception is occured : " << ex.what() << std::endl;
		} catch (const std::string& ex) {
			std::cerr << "String Exception is occured : " << ex << std::endl;
		} catch (...) {
			std::exception_ptr p = std::current_exception();
			std::clog << boost::current_exception_diagnostic_information() << std::endl;
		}
	}
	return 0;
}


#include <iostream>
#include <cstdlib>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib> //std::system
#include <cstddef>
#include <cassert>
#include <utility>

int main(int argc, char *argv[])
{
	using namespace boost::interprocess;
	typedef std::pair<double, int> MyType;

	if (argc == 1) {  //Parent process
					  //Remove shared memory on construction and destruction
		struct shm_remove
		{
			shm_remove() { shared_memory_object::remove("MySharedMemory"); }
			~shm_remove() { shared_memory_object::remove("MySharedMemory"); }
		} remover;

		//Construct managed shared memory
		managed_shared_memory segment(create_only, "MySharedMemory", 65536);

		//Create an object of MyType initialized to {0.0, 0}
		MyType *instance = segment.construct<MyType>
			("MyType instance")  //name of the object
			(0.0, 0);            //ctor first argument

								 //Create an array of 10 elements of MyType initialized to {0.0, 0}
		MyType *array = segment.construct<MyType>
			("MyType array")     //name of the object
			[10]                 //number of elements
		(0.0, 0);            //Same two ctor arguments for all objects

							 //Create an array of 3 elements of MyType initializing each one
							 //to a different value {0.0, 0}, {1.0, 1}, {2.0, 2}...
		float float_initializer[3] = { 0.0, 1.0, 2.0 };
		int   int_initializer[3] = { 0, 1, 2 };

		MyType *array_it = segment.construct_it<MyType>
			("MyType array from it")   //name of the object
			[3]                        //number of elements
		(&float_initializer[0]    //Iterator for the 1st ctor argument
			, &int_initializer[0]);    //Iterator for the 2nd ctor argument

									   //Launch child process
		std::string s(argv[0]); s += " child ";
		if (0 != std::system(s.c_str()))
			return 1;


		//Check child has destroyed all objects
		if (segment.find<MyType>("MyType array").first ||
			segment.find<MyType>("MyType instance").first ||
			segment.find<MyType>("MyType array from it").first)
			return 1;

		std::clog << "Parent is done." << std::endl;
	}
	else {
		//Open managed shared memory
		managed_shared_memory segment(open_only, "MySharedMemory");

		std::pair<MyType*, managed_shared_memory::size_type> res;

		//Find the array
		res = segment.find<MyType>("MyType array");
		//Length should be 10
		if (res.second != 10) return 1;

		//Find the object
		res = segment.find<MyType>("MyType instance");
		//Length should be 1
		if (res.second != 1) return 1;

		//Find the array constructed from iterators
		res = segment.find<MyType>("MyType array from it");
		//Length should be 3
		if (res.second != 3) return 1;

		//We're done, delete all the objects
		segment.destroy<MyType>("MyType array");
		segment.destroy<MyType>("MyType instance");
		segment.destroy<MyType>("MyType array from it");

		std::clog << "Child is done." << std::endl;
	}
	return EXIT_SUCCESS;
}
#endif

#if _DF_4

#include <iostream>
#include <cstdlib>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/offset_ptr.hpp>

using namespace boost::interprocess;

//Shared memory linked list node
struct list_node
{
	offset_ptr<list_node> next;
	int                   value;
};

int main(int argc, char* argv[]) {

	//Remove shared memory on construction and destruction
	struct shm_remove
	{
		shm_remove() { shared_memory_object::remove("MySharedMemory"); }
		~shm_remove() { shared_memory_object::remove("MySharedMemory"); }
	} remover;

	//Create shared memory
	managed_shared_memory segment(create_only,
		"MySharedMemory",  //segment name
		65536);

	//Create linked list with 10 nodes in shared memory
	offset_ptr<list_node> prev = 0, current, first;

	int i;
	for (i = 0; i < 10; ++i, prev = current) {
		current = static_cast<list_node*>(segment.allocate(sizeof(list_node)));
		current->value = i;
		current->next = 0;

		if (!prev)
			first = current;
		else
			prev->next = current;
	}


	std::cout << "segment is allocated done." << std::endl;

	//Communicate list to other processes
	//. . .
	//When done, destroy list
	for (current = first; current; /**/) {
		prev = current;
		current = current->next;
		segment.deallocate(prev.get());
	}

	std::cout << "segment is searched done." << std::endl;

	return EXIT_SUCCESS;
}

#endif

#if _DF_5

#include <iostream>
#include <cstdlib>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <functional>
#include <utility>

int main(int argc, char* argv[])
{
	using namespace boost::interprocess;

	//Remove shared memory on construction and destruction
	struct shm_remove
	{
		shm_remove() { shared_memory_object::remove("MySharedMemory"); }
		~shm_remove() { shared_memory_object::remove("MySharedMemory"); }
	} remover;

	//Shared memory front-end that is able to construct objects
	//associated with a c-string. Erase previous shared memory with the name
	//to be used and create the memory segment at the specified address and initialize resources
	managed_shared_memory segment
	(create_only
		, "MySharedMemory" //segment name
		, 65536);          //segment size in bytes

						   //Note that map<Key, MappedType>'s value_type is std::pair<const Key, MappedType>,
						   //so the allocator must allocate that pair.
	typedef int    KeyType;
	typedef float  MappedType;
	typedef std::pair<const int, float> ValueType;

	//Alias an STL compatible allocator of for the map.
	//This allocator will allow to place containers
	//in managed shared memory segments
	typedef allocator<ValueType, managed_shared_memory::segment_manager>
		ShmemAllocator;

	//Alias a map of ints that uses the previous STL-like allocator.
	//Note that the third parameter argument is the ordering function
	//of the map, just like with std::map, used to compare the keys.
	typedef map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MyMap;

	//Initialize the shared memory STL-compatible allocator
	ShmemAllocator alloc_inst(segment.get_segment_manager());

	//Construct a shared memory map.
	//Note that the first parameter is the comparison function,
	//and the second one the allocator.
	//This the same signature as std::map's constructor taking an allocator
	MyMap *mymap =
		segment.construct<MyMap>("MyMap")      //object name
		(std::less<int>() //first  ctor parameter
			, alloc_inst);     //second ctor parameter

							   //Insert data in the map
	for (int i = 0; i < 100; ++i) {
		mymap->insert(std::pair<const int, float>(i, (float)i));
	}


	return EXIT_SUCCESS;
}

#endif

#if _DF_6

#include <iostream>
#include <cstdlib>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstddef>
#include <cstdlib>

int main(int argc, char *argv[])
{
	using namespace boost::interprocess;

	//Define file names
	const char *FileName = "file.bin";
	const std::size_t FileSize = 10000;

	if (argc == 1) { //Parent process executes this
		{  //Create a file
			file_mapping::remove(FileName);
			std::filebuf fbuf;
			fbuf.open(FileName, std::ios_base::in | std::ios_base::out
				| std::ios_base::trunc | std::ios_base::binary);
			//Set the size
			fbuf.pubseekoff(FileSize - 1, std::ios_base::beg);
			fbuf.sputc(0);
		}

		//Remove on exit
		struct file_remove
		{
			file_remove(const char *FileName)
				: FileName_(FileName) {}
			~file_remove() { file_mapping::remove(FileName_); }
			const char *FileName_;
		} remover(FileName);

		//Create a file mapping
		file_mapping m_file(FileName, read_write);

		//Map the whole file with read-write permissions in this process
		mapped_region region(m_file, read_write);

		//Get the address of the mapped region
		void * addr = region.get_address();
		std::size_t size = region.get_size();

		//Write all the memory to 1
		std::memset(addr, 1, size);

		std::cout << "Parent : " << "memset(" << addr << ", " << 1 << ", " << size << ")" << std::endl;

		//Launch child process
		std::string s(argv[0]); s += " child ";
		if (0 != std::system(s.c_str()))
			return 1;
	}
	else {  //Child process executes this
		{  //Open the file mapping and map it as read-only
			file_mapping m_file(FileName, read_only);

			mapped_region region(m_file, read_only);

			//Get the address of the mapped region
			void * addr = region.get_address();
			std::size_t size = region.get_size();

			//Check that memory was initialized to 1
			const char *mem = static_cast<char*>(addr);
			for (std::size_t i = 0; i < size; ++i)
				if (*mem++ != 1)
					return 1;   //Error checking memory
		}
		{  //Now test it reading the file
			std::filebuf fbuf;
			fbuf.open(FileName, std::ios_base::in | std::ios_base::binary);

			//Read it to memory
			std::vector<char> vect(FileSize, 0);
			fbuf.sgetn(&vect[0], std::streamsize(vect.size()));

			//Check that memory was initialized to 1
			const char *mem = static_cast<char*>(&vect[0]);
			for (std::size_t i = 0; i < FileSize; ++i)
				if (*mem++ != 1)
					return 1;   //Error checking memory
		}

		std::cout << "Child : done." << std::endl;

	}

	return 0;
}

#endif

#if _DF_7

#include <iostream>
#include <cstdlib>

using namespace std;

int operate(int a, int b) {
	return (a*b);
}

double operate(double a, double b) {
	return (a / b);
}

int main(int argc, char* argv[]) {

	int x = 5, y = 2;
	double n = 5.0, m = 2.0;
	cout << operate(x, y) << endl;
	cout << operate(n, m) << endl;

	return EXIT_SUCCESS;
}

#endif // _df
