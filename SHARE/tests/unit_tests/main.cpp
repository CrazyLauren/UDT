// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <deftype>
#include <SHARE/unit_tests.h>

using namespace NSHARE;

namespace share_unit_tests
{
int main(int argc, char const*argv[])
{		
	unit_testing();
	return EXIT_SUCCESS;
}
}
int main(int argc, char const*argv[])
{
	return share_unit_tests::main(argc,argv);
}
