// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <deftype>
#include <SHARE/UType/CSharedMemory.h>
#ifdef __linux__
#	include <spawn.h>
#	include <signal.h>
#else
#	include <process.h>
#endif
#include <tclap/CmdLine.h>
#include <SHARE/logging/CShareLogArgsParser.h>

namespace test_selection
{

static bool g_is_child = false;

static NSHARE::CSharedMemory g_memory;
NSHARE::CIPCSem g_mutex_stream;
size_t const g_buf_shm_size = 1024;

extern std::string const g_subscriber_name = "subscriber@gudt.test.control";

extern std::string g_name;
std::string g_name= g_subscriber_name;

static std::string const g_sem_name = g_subscriber_name;

#define LOCK_STREAM NSHARE::CRAII<NSHARE::CIPCSem> _block(g_mutex_stream);

extern std::vector<std::pair<ptrdiff_t, std::string> > g_child_pid;
std::vector<std::pair<ptrdiff_t, std::string> > g_child_pid =
{
{ 0, "publisher@gudt" },
{ 0, "publisher@gudt.test" },
{ 0, "publisher@gudt.test.control" },
{ 0, "publisher@gudt.test.control.sub" } };
int g_argc;
char const** g_argv;

extern "C" char **environ; //for linux

extern int start_child(char const* aName)
{
	std::string _str_t("-t");
	const int _index_t = g_argc;

	std::string _str_name("-n ");
	_str_name += aName;
	const int _index_n = g_argc + 1;

	const int _amount_of_additional_param = 2;
	const int _index_of_null = g_argc + _amount_of_additional_param;
	const unsigned _sizeof_array = g_argc + _amount_of_additional_param
			+ 1/*NULL*/;

	char const * * const _p = new char const *[_sizeof_array];

	for (int i = 0; i < g_argc; ++i)
		_p[i] = g_argv[i];

	_p[_index_t] = _str_t.c_str();
	_p[_index_n] = _str_name.c_str();
	_p[_index_of_null] = NULL;

	NSHARE::CThread::process_id_t _id = 0;
#ifdef __linux__
	int const exit_val=posix_spawn((pid_t*)&_id, g_argv[0],NULL,NULL,(char *const*)_p,(char* const*)environ);
	signal(SIGCHLD,SIG_IGN);
#else
	int const exit_val = spawnv(P_NOWAIT, g_argv[0], (char *const*)_p);
	_id = exit_val;
#endif
	if (exit_val < 0)
	{
		std::cerr << "Cannot start process " << std::endl;
		exit(EXIT_FAILURE);
	}

	delete[] _p;

	return _id;
}
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
bool start_subscriber(int argc, char const *argv[], char const * aName);
bool start_publisher(int argc, char const *argv[], char const * aName);

bool create_ipc_mutex()
{
	using namespace NSHARE;

	bool const _is = g_memory.MOpenOrCreate(g_sem_name, g_buf_shm_size)==CSharedMemory::E_NO_ERROR;
	if (!_is)
	{
		std::cerr << "Cannot create shared memory:" << g_sem_name << std::endl;
		return false;
	}
	void* const _p = g_memory.MMallocTo(NSHARE::CIPCSem::eReguredBufSize, 0);

	if (!_p)
	{
		std::cerr << "Cannot allocate memory" << std::endl;
        return false;
	}

	if (!g_mutex_stream.MInit((uint8_t*) _p, CIPCSem::eReguredBufSize, 1,
			CIPCSem::E_HAS_TO_BE_NEW))
	{
		std::cerr << "Semaphore  " << g_sem_name << " is exist" << std::endl;
        return false;
	}
	return true;
}
bool initialize_ipc_mutex()
{
	using namespace NSHARE;

	bool const _is = g_memory.MOpen(g_sem_name) == CSharedMemory::E_NO_ERROR;
	if (!_is)
	{
		std::cerr << "Cannot open shared memory:" << g_sem_name << std::endl;
        return false;
	}
	void* const _p = g_memory.MGetIfMalloced(0);

	if (!_p)
	{
		std::cerr << "Cannot open memory" << std::endl;
        return false;
	}

	if (!g_mutex_stream.MInit((uint8_t*) _p, CIPCSem::eReguredBufSize, 1,
			CIPCSem::E_HAS_EXIST))
	{
		std::cerr << "Semaphore  " << g_sem_name << " is exist" << std::endl;
        return false;
	}
    return  true;
}
}
int main(int argc, char const*argv[])
{
	init_trace(argc, argv);
	using namespace test_selection;
	g_argc = argc;
	g_argv = argv;

	parse_cmd(argc, argv);

	bool _is=true;
	if (!g_is_child)
	{
        _is=_is&&create_ipc_mutex();

        _is=_is&&start_subscriber(argc, argv, g_name.c_str());

		g_mutex_stream.MFree();  //remove mutex

		g_memory.MGetAllocator()->MDeallocate(g_memory.MGetIfMalloced(0), 0); //free memory

		g_memory.MFree();  //free shm
	}
	else
	{
        _is=_is&&initialize_ipc_mutex();
		{
			LOCK_STREAM
			std::cout<<"\t started:"<<getpid()<<" ("<<g_name<<")"<<std::endl;
		}
        _is=_is&&start_publisher(argc, argv, g_name.c_str());
		g_mutex_stream.MFree();
		g_memory.MFree();
	}

	return _is?EXIT_SUCCESS:EXIT_FAILURE;
}

