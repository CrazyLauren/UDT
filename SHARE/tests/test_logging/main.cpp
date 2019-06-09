// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#define UNW_LOCAL_ONLY

#include <deftype>
//#include <boost/core/demangle.hpp>

#include "test_api.h"
#include <logging/CStackTrace.h>
using namespace NSHARE;

namespace test_sigmentation_failed
{
int call_function_with_segmentation(void)
{
	/*int* a=NULL;
	return *a;*/
	CStackTrace _stack;
	_stack.MPrint(std::cout);
	return 0;
}

template<class T>
T call_function_1(void)
{
	return call_function_with_segmentation();
}

int call_function_2(void)
{
	return call_function_1<unsigned>();
}

int main(int argc, char const*argv[])
{		
	call_function_2();
	return EXIT_SUCCESS;
}
}
void handler() 
{
	fprintf(stderr, "Segmentation fault\n");
//	test_sigmentation_failed::printStackTrace(stderr,2);
	abort();
}
int main(int argc, char const*argv[])
{
//	signal(SIGSEGV,handler);
	return test_sigmentation_failed::main(argc,argv);
}
