// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <deftype>
#include <SHARE/UType/CSharedMemory.h>
#include <tclap/CmdLine.h>
#include <SHARE/logging/CShareLogArgsParser.h>

#include "api_test.h"
namespace test_rtc
{

bool g_is_child = false;
NSHARE::CIPCSem g_mutex_stream;
double const g_start_time=1;
std::string const g_subscriber_name = "rtc@gudt.test.control";
std::string const g_publisher_name = "rtc@gudt.test.user";
std::string g_name = g_subscriber_name;
std::vector<std::pair<ptrdiff_t, std::string> > g_child_pid =
{
{ 0, g_publisher_name+std::string(".1") },
{ 0, g_publisher_name+std::string(".2") },
{ 0, g_publisher_name+std::string(".3") },
{ 0, g_publisher_name+std::string(".4") } };
const char * g_rtc_name="rtc";

static std::string const g_sem_name = g_subscriber_name;
static NSHARE::CSharedMemory g_memory;
size_t const g_buf_shm_size = 1024;

int g_argc;
char const** g_argv;

extern "C" char **environ; //for linux


void parse_cmd(int argc, char const* argv[])
{
	using namespace TCLAP;
	CmdLine _cmd("Selection test", ' ');
	NSHARE::CShareLogArgsParser _logging("v", "verbose");
	_cmd.add(_logging);

	SwitchArg _type("t", "type", "type program", _cmd);

	ValueArg<std::string> _name("n", "name",
			"id name of kernel (by default program name)", false, "", "string",
			_cmd);

	try
	{
		_cmd.parse(argc, argv);
		g_is_child = _type.getValue();

		if (_name.isSet())
			g_name = _name.getValue();

	} catch (ArgException& e)  // catch any exceptions
	{
		std::cerr << "Invalid argument : " << e.error() << " for  " << e.argId()
				<< std::endl;
		exit(EXIT_FAILURE);
	}
}
void create_ipc_mutex()
{
	using namespace NSHARE;
	g_memory.sMRemove(g_sem_name);

	bool const _is = g_memory.MOpenOrCreate(g_sem_name, g_buf_shm_size)==CSharedMemory::E_NO_ERROR;
	if (!_is)
	{
		std::cerr << "Cannot create shared memory:" << g_sem_name << std::endl;
		exit(EXIT_FAILURE);
	}
	void* const _p = g_memory.MMallocTo(NSHARE::CIPCSem::eReguredBufSize, 0);

	if (!_p)
	{
		std::cerr << "Cannot allocate memory" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!g_mutex_stream.MInit((uint8_t*) _p, CIPCSem::eReguredBufSize, 1,
			CIPCSem::E_HAS_TO_BE_NEW))
	{
		std::cerr << "Semaphore  " << g_sem_name << " is exist" << std::endl;
		exit(EXIT_FAILURE);
	}
}
void initialize_ipc_mutex()
{
	using namespace NSHARE;

	bool const _is = g_memory.MOpen(g_sem_name) == CSharedMemory::E_NO_ERROR;
	if (!_is)
	{
		std::cerr << "Cannot open shared memory:" << g_sem_name << std::endl;
		exit(EXIT_FAILURE);
	}
	void* const _p = g_memory.MGetIfMalloced(0);

	if (!_p)
	{
		std::cerr << "Cannot open memory" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!g_mutex_stream.MInit((uint8_t*) _p, CIPCSem::eReguredBufSize, 1,
			CIPCSem::E_HAS_EXIST))
	{
		std::cerr << "Semaphore  " << g_sem_name << " is exist" << std::endl;
		exit(EXIT_FAILURE);
	}
}
}
int main(int argc, char const*argv[])
{
	init_trace(argc, argv);
	using namespace test_rtc;
	g_argc = argc;
	g_argv = argv;

	parse_cmd(argc, argv);

	if (!g_is_child)
	{
		create_ipc_mutex();

		start_rtc_control(argc, argv, g_name.c_str());

		g_mutex_stream.MFree();  //remove mutex

		g_memory.MGetAllocator()->MDeallocate(g_memory.MGetIfMalloced(0), 0); //free memory

		g_memory.MFree();  //free shm
	}
	else
	{
		initialize_ipc_mutex();
		start_rtc_user(argc, argv, g_name.c_str());
		g_mutex_stream.MFree();
		g_memory.MFree();
	}

	return EXIT_SUCCESS;
}

