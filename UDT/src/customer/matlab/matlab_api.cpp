// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * matlab_api.cpp
 *
 *  Created on: 27.07.2020
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2020  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <SHARE/matlab_serialize.h>
#include <customer.h>
#include <SHARE/revision.h>
#include <UDT/impl/customer_covert_impl_matlab.h>

using namespace NSHARE;
using namespace NSHARE::matlab;
DECLARATION_VERSION_FOR(customer_matlab)

namespace commands
{
mxChar const init_command[] =
		{ (mxChar) 'i', (mxChar) 'n', (mxChar) 'i', (mxChar) 't' };

mxChar const free_command[] =
		{ (mxChar) 'f', (mxChar) 'r', (mxChar) 'e', (mxChar) 'e' };

mxChar const help[] =
		{ (mxChar) 'h', (mxChar) 'e', (mxChar) 'l', (mxChar) 'p' };

mxChar const customer_version[] =
		{ (mxChar) 'v', (mxChar) 'e', (mxChar) 'r', (mxChar) 's', (mxChar) 'i',
				(mxChar) 'o', (mxChar) 'n' };

mxChar const api_version[] =
		{ (mxChar) 'v', (mxChar) 'e', (mxChar) 'r', (mxChar) 's', (mxChar) 'i',
				(mxChar) 'o', (mxChar) 'n', (mxChar) 'A', (mxChar) 'P',
				(mxChar) 'I' };

mxChar const is_connected[] =
		{ (mxChar) 'i', (mxChar) 's', (mxChar) 'C',
				(mxChar) 'o', (mxChar) 'n', (mxChar) 'n', (mxChar) 'e'
				, (mxChar) 'c', (mxChar) 't', (mxChar) 'e', (mxChar) 'd'
		};

mxChar const is_opened[] =
		{ (mxChar) 'i', (mxChar) 's', (mxChar) 'O',
				(mxChar) 'p', (mxChar) 'e', (mxChar) 'n'
		};

mxChar const open_cmd[] =
		{ (mxChar) 'o', (mxChar) 'p', (mxChar) 'e', (mxChar) 'n' };

mxChar const close_cmd[] =
		{ (mxChar) 'c', (mxChar) 'l', (mxChar) 'o', (mxChar) 's', (mxChar) 'e' };

mxChar const wait_cmd[] =
		{ (mxChar) 'w', (mxChar) 'a', (mxChar) 'i', (mxChar) 't' };

mxChar const get_id_cmd[] =
		{ (mxChar) 'i', (mxChar) 'd' };

mxChar const customers_cmd[] =
		{ (mxChar) 'c', (mxChar) 'u', (mxChar) 's', (mxChar) 't', (mxChar) 'o',
				(mxChar) 'm', (mxChar) 'e', (mxChar) 'r', (mxChar) 's' };

mxChar const join_cmd[] =
		{ (mxChar) 'j', (mxChar) 'o', (mxChar) 'i', (mxChar) 'n' };

/** Position of input argument in mex Function
 */
enum eArgumentInputId
{
	eCOMMAND = 0, //!< The command is first param

	eUDT_NAME = 1, //!< Name of UDT param
	eMAJOR_VERSION = 2, //!< Major version of library
	eMINOR_VERSION = 3, //!< Minor version of library
	eINIT_PARAM = 4, //!< Initialize params (arvg)
	ePATH_TO_CONFIG = 5, //!< Path to config

	eEVENT_NAME = 1, //!< Name of event
	eWAIT_TIME = 2, //!< Timeout
};

#define MAX_OUTPUT_NUMBER 6

}
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	VLOG(1) << "Parse nlhs=" << nlhs << " nrhs=" << nrhs;
	bool _is_error = false;
	bool _is_print_help = false;

	size_t const aFirstInput = 0;
	size_t const aFirstOutput = 0;
	if (nrhs == 0)
	{
		VLOG(ERROR) << "No input value ";
		mexWarnMsgIdAndTxt("easyprotocol:InvalidArgument",
				"No input value. The first value has to be command.");
		_is_error = true;
	}
	else
	{
#define DEFINE_COMMAND_ARG(aName, aId)\
		mxArray const* const aName =\
						(aFirstInput + commands:: aId) < nrhs ?\
								prhs[aFirstInput + commands:: aId] :\
								NULL;\
								/*END*/

		DEFINE_COMMAND_ARG(_cmd_ptr, eCOMMAND)

		DEFINE_COMMAND_ARG(_udt_name_ptr, eUDT_NAME)
		DEFINE_COMMAND_ARG(_major_version_ptr, eMAJOR_VERSION)
		DEFINE_COMMAND_ARG(_minor_version_ptr, eMINOR_VERSION)
		DEFINE_COMMAND_ARG(_init_param_ptr, eINIT_PARAM)
		DEFINE_COMMAND_ARG(_path_to_config_ptr, ePATH_TO_CONFIG)

		DEFINE_COMMAND_ARG(_event_name_ptr, eEVENT_NAME)
		DEFINE_COMMAND_ARG(_wait_time_ptr, eWAIT_TIME)

		mxArray ** _output_ptr[MAX_OUTPUT_NUMBER];

		memset(_output_ptr, 0, sizeof(_output_ptr));

		for (unsigned i = 0; (i + aFirstOutput) < nlhs //
		&& i < MAX_OUTPUT_NUMBER; ++i)
			_output_ptr[i] = &plhs[i + aFirstOutput];

		mxClassID const category = mxGetClassID(_cmd_ptr);
		if (category != mxCHAR_CLASS)
		{
			//category
			mexErrMsgIdAndTxt("InvalidArgument",
					"First argument has to be command - type char: %s ",
					print_matlab_type(category));
			_is_error = true;
		}
		else
		{
			size_t const total_num_of_elements =
					static_cast<size_t>(mxGetNumberOfElements(_cmd_ptr));

#define IS_COMMAND(aVar) sizeof(commands:: aVar)/sizeof(commands:: aVar[0]) == total_num_of_elements \
			                && memcmp(commands:: aVar, mxGetData(_cmd_ptr), sizeof(commands:: aVar) ) == 0\
							/*END*/

			if (IS_COMMAND(init_command))
			{
				VLOG(1) << "Init command detected ";
				if (_udt_name_ptr == NULL
						|| _major_version_ptr == NULL
						|| _minor_version_ptr == NULL
				)
				{
					mexErrMsgIdAndTxt("InvalidArgument",
							"No name or version major or version minor ");

					_is_error = true;
				}
				else
				{
					NSHARE::CText _name;
					uint8_t _major = 0;
					uint8_t _minor = 1;
					if (!deserialize(&_name, _udt_name_ptr))
					{
						mexErrMsgIdAndTxt("InvalidName",
								"Cannot deserialize name ");

						_is_error = true;
					}
					else if (!deserialize(&_major, _major_version_ptr))
					{
						mexErrMsgIdAndTxt("InvalidMajorVersion",
								"Cannot deserialize major version ");

						_is_error = true;
					}
					else if (!deserialize(&_minor, _minor_version_ptr))
					{
						mexErrMsgIdAndTxt("InvalidMinorVersion",
								"Cannot deserialize minor version ");

						_is_error = true;
					}
					else
					{
						NSHARE::CText _init_param;
						NSHARE::CText _path;

						if (_init_param_ptr != NULL)
							deserialize(&_init_param, _init_param_ptr);

						if (_path_to_config_ptr != NULL)
							deserialize(&_path, _path_to_config_ptr);

						NSHARE::version_t _ver;
						_ver.FMajor = _major;
						_ver.FMinor = _minor;

						char const* _argv[2] =
								{ NULL, NULL };
						NSHARE::CProgramName const _name_full(_name);
						NSHARE::CText _str("matlab_");
						_str += _name_full.MGetName();

						int _arc = 0;
						_argv[_arc++] = (char const*) _str.c_str();
						if (!_init_param.empty())
							_argv[_arc++] = (char const*) _init_param.c_str();

						int const _rval =
								_path.empty() ?
										NUDT::CCustomer::sMInit(_arc, _argv,
												_name.c_str(),
												_ver)
												:
										NUDT::CCustomer::sMInit(_arc, _argv,
												_name.c_str(),
												_ver, _path);

						if (_output_ptr[0] != NULL)
							*_output_ptr[0] = NSHARE::matlab::serialize(_rval);
					}
				}
			}
			else
			if (IS_COMMAND(free_command))
			{
				VLOG(1) << "Free command detected ";
				NUDT::CCustomer::sMFree();
			}
			else if (IS_COMMAND(customer_version))
			{
				VLOG(1) << "Version command detected ";
				NSHARE::version_t const & _ver = NUDT::CCustomer::sMVersion();

				if (_output_ptr[0] != NULL)
				{
					*_output_ptr[0] = NSHARE::matlab::serialize(_ver.FMajor);
				}
				if (_output_ptr[1] != NULL)
				{
					*_output_ptr[1] = NSHARE::matlab::serialize(_ver.FMinor);
				}
				if (_output_ptr[2] != NULL)
				{
					*_output_ptr[2] = NSHARE::matlab::serialize(_ver.FRelease);
				}
			}
			else if (IS_COMMAND(api_version))
			{
				VLOG(1) << "Api version command detected ";

				if (_output_ptr[0] != NULL)
				{
					*_output_ptr[0] = NSHARE::matlab::serialize<uint32_t>(
							MAJOR_VERSION_OF(customer_matlab));
				}
				if (_output_ptr[1] != NULL)
				{
					*_output_ptr[1] = NSHARE::matlab::serialize<uint32_t>(
							MINOR_VERSION_OF(customer_matlab));
				}
				if (_output_ptr[2] != NULL)
				{
					*_output_ptr[2] = NSHARE::matlab::serialize<uint32_t>(
							REVISION_OF(customer_matlab));
				}
				if (_output_ptr[3] != NULL)
				{
					std::string const _rval(REVISION_PATH_OF(customer_matlab));
					*_output_ptr[3] = NSHARE::matlab::serialize(
							_rval);
				}
				if (_output_ptr[4] != NULL)
				{
					std::string _rval(COMPILE_DATA_OF(customer_matlab));

					_rval += ' ';
					_rval += COMPILE_TIME_OF(customer_matlab);

					*_output_ptr[4] = NSHARE::matlab::serialize(
							_rval);
				}
			}
			else if (IS_COMMAND(is_connected))
			{
				VLOG(1) << "Is connected command detected ";
				if (NUDT::CCustomer::sMGetInstancePtr() == NULL
						|| _output_ptr[0] == NULL
				)
				{
					if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
						mexErrMsgIdAndTxt("InvalidArgument",
								"No output variable ");
					else
						mexErrMsgIdAndTxt("DoesntInited",
								"Library doesn't inited");

					_is_error = true;
				}
				else
				{
					bool const _rval =
							NUDT::CCustomer::sMGetInstance().MIsConnected();
					*_output_ptr[0] = NSHARE::matlab::serialize(_rval);
				}
			}
			else if (IS_COMMAND(is_opened))
			{
				VLOG(1) << "Is opened command detected ";
				if (NUDT::CCustomer::sMGetInstancePtr() == NULL
						|| _output_ptr[0] == NULL
				)
				{
					if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
						mexErrMsgIdAndTxt("InvalidArgument",
								"No output variable ");
					else
						mexErrMsgIdAndTxt("DoesntInited",
								"Library doesn't inited");

					_is_error = true;
				}
				else
				{
					bool const _rval =
							NUDT::CCustomer::sMGetInstance().MIsOpened();
					*_output_ptr[0] = NSHARE::matlab::serialize(_rval);
				}
			}
			else if (IS_COMMAND(open_cmd))
			{
				VLOG(1) << "Open command detected ";
				if (NUDT::CCustomer::sMGetInstancePtr() == NULL)
				{
					mexErrMsgIdAndTxt("DoesntInited",
								"Library doesn't inited");

					_is_error = true;
				}
				else
				{
					bool const _rval = NUDT::CCustomer::sMGetInstance().MOpen();
					if( _output_ptr[0] != NULL)
						*_output_ptr[0] = NSHARE::matlab::serialize(_rval);
				}
			}
			else if (IS_COMMAND(close_cmd))
			{
				VLOG(1) << "Close command detected ";
				if (NUDT::CCustomer::sMGetInstancePtr() == NULL
				)
				{
					mexErrMsgIdAndTxt("DoesntInited",
							"Library doesn't inited");

					_is_error = true;
				}
				else
				{
					NUDT::CCustomer::sMGetInstance().MClose();
				}
			}
			else if (IS_COMMAND(wait_cmd))
			{
				VLOG(1) << "Wait command detected ";
				if (NUDT::CCustomer::sMGetInstancePtr() == NULL
						|| _event_name_ptr == NULL
				)
				{
					if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
						mexErrMsgIdAndTxt("InvalidArgument",
								"No event name");
					else
						mexErrMsgIdAndTxt("DoesntInited",
								"Library doesn't inited");

					_is_error = true;
				}
				else
				{
					NSHARE::CText _name;
					if (!deserialize(&_name, _event_name_ptr))
					{
						mexErrMsgIdAndTxt("InvalidName",
								"Cannot deserialize name ");

						_is_error = true;
					}
					else
					{
						double _time = -1;

						if (_wait_time_ptr != NULL)
						{
							if (!deserialize(&_time, _wait_time_ptr))
							{
								mexErrMsgIdAndTxt("InvalidName",
										"Cannot deserialize time ");

							}
						}

						int const _rval =
								NUDT::CCustomer::sMGetInstance().MWaitForEvent(
										_name, _time);
						if (_output_ptr[0] != NULL)
							*_output_ptr[0] = NSHARE::matlab::serialize(_rval);
					}
				}
			}
			else if (IS_COMMAND(get_id_cmd))
			{
				VLOG(1) << "Get ID command detected ";
				if (NUDT::CCustomer::sMGetInstancePtr() == NULL
						|| _output_ptr[0] == NULL
				)
				{
					if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
						mexErrMsgIdAndTxt("InvalidArgument",
								"No output variable ");
					else
						mexErrMsgIdAndTxt("DoesntInited",
								"Library doesn't inited");

					_is_error = true;
				}
				else
				{

					NUDT::program_id_t const _rval =
							NUDT::CCustomer::sMGetInstance().MGetID();
					if (_output_ptr[0] != NULL)
						*_output_ptr[0] = NSHARE::matlab::serialize(_rval);

				}
			}
			else if (IS_COMMAND(customers_cmd))
			{
				VLOG(1) << "Get customers command detected ";
				if (NUDT::CCustomer::sMGetInstancePtr() == NULL
						|| _output_ptr[0] == NULL
				)
				{
					if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
						mexErrMsgIdAndTxt("InvalidArgument",
								"No output variable ");
					else
						mexErrMsgIdAndTxt("DoesntInited",
								"Library doesn't inited");

					_is_error = true;
				}
				else
				{

					NUDT::CCustomer::customers_t const _rval =
							NUDT::CCustomer::sMGetInstance().MCustomers();
					if (_output_ptr[0] != NULL)
						*_output_ptr[0] = NSHARE::matlab::serialize(_rval);

				}
			}
			else if (IS_COMMAND(join_cmd))
			{
				VLOG(1) << "Get ID command detected ";
				if (NUDT::CCustomer::sMGetInstancePtr() == NULL)
				{
					mexErrMsgIdAndTxt("DoesntInited",
								"Library doesn't inited");

					_is_error = true;
				}
				else
				{
					NUDT::CCustomer::sMGetInstance().MJoin();
				}
			}
	        else if(IS_COMMAND(help))
	        {
	            _is_print_help = true;
	        }
	        else
	        {
	            LOG(ERROR)<<"No command";
	            _is_error = true;
	            mexErrMsgIdAndTxt( "{{ name }}:InvalidCommand",
	                "Invalid command");
	        }
#undef IS_COMMAND
		}
	}
    if(_is_error || _is_print_help)
    {
           	mexPrintf("\nUsage:\n\n"
           			"     \t# Initialize library:\n\n"
           			"     \t\terror = customer( 'init', name, mj, mn, params, config )\n\n"
           			"     \t      error (%s) - Error code ( 0 - no error) \n"
           			"     \t      name (%s) - The unique name format (name@ru.kremlin.www) \n"
       				"     \t      mj (%s) - Major version of program\n"
       				"     \t      mn (%s) - Minor version of program\n"
       				"     \t      params (%s) - Initialize parameters (doesn't required)\n"
       				"     \t      config (%s) - Path to config (doesn't required)\n\n\n"

           			"     \t# Free resources of library:\n\n"
           			"     \t\tcustomer('free')\n\n\n"

           			"     \t# Get UDT Customer version:\n\n"
           			"     \t\t[ mj, mn, revision ] = customer( 'version' )\n\n"
       				"     \t      mj (%s) - Major version\n"
       				"     \t      mn (%s) - Minor version\n"
       				"     \t      revision (%s) - Realization version\n\n\n"

                    "     \t#  Matlab API version:\n\n"
           			"     \t\t[major, minor, revision, path, compile_time ] = customer('versionAPI')\n\n"
           			"     \t      major (%s) - API major version\n"
           			"     \t      minor (%s) - API minor version\n"
           			"     \t      revision (%s) - Realization revision\n"
           			"     \t      path (%s) - Source path\n"
           			"     \t      compile_time (%s) - Compile time\n\n\n"

                    "     \t#  Check for connected to UDT:\n\n"
           			"     \t\tis_connected = customer('isConnected')\n\n"
           			"     \t      is_connected (%s) - true if connected\n\n\n"

                    "     \t#  Check for opened:\n\n"
           			"     \t\tis_opened = customer('isOpen')\n\n"
           			"     \t      is_opened (%s) - true if opened\n\n\n"

                    "     \t#  Open (Connect to UDT):\n\n"
           			"     \t\tis_opened = customer('open')\n\n"
           			"     \t      is_opened (%s) - true if opened ( doesn't required )\n\n\n"

           			"     \t# Close (Disconnect from UDT):\n\n"
           			"     \t\tcustomer('close')\n\n\n"

           			"     \t# Expect event:\n\n"
           			"     \t\tresult = customer('wait', event, time)\n\n"
           			"     \t      event (%s) - Event name ('%s' - wait to connected, '%s' - wait to disconnected)\n"
           			"     \t      time (%s) - Timeout if less zero - infinity (doesn't required)\n"
           			"     \t      result (%s) - <0 - error code\n\n\n"

                    "     \t#  Gets own ID:\n\n"
           			"     \t\tid = customer('id')\n\n"
           			"     \t      id (%s) - program id \n\n\n"

                    "     \t#  Gets list of all program:\n\n"
           			"     \t\tid = customer('customers')\n\n"
           			"     \t      id (array of %s) - programs id \n\n\n"

           			"     \t# Join to main thread:\n\n"
           			"     \t\tcustomer('join')\n\n\n"

           			"     \t# Print help:\n\n"
           			"     \t\tcustomer('help')\n\n\n"

                    	, matlab_code_t<int>::type()//init error
						, matlab_code_t<utf32>::type()//init name
						, matlab_code_t<NSHARE::version_t::major_t>::type()//init mj
						, matlab_code_t<NSHARE::version_t::minor_t>::type()//init mn
						, matlab_code_t<utf32>::type()//init params
						, matlab_code_t<utf32>::type()//init config

						, matlab_code_t<NSHARE::version_t::major_t>::type()//version mj
						, matlab_code_t<NSHARE::version_t::minor_t>::type()//version mn
						, matlab_code_t<NSHARE::version_t::release_t>::type()//version revision

			            , matlab_code_t<uint32_t>::type()//versionAPI major
			            , matlab_code_t<uint32_t>::type()//versionAPI minor
			            , matlab_code_t<uint32_t>::type()//versionAPI revision
			            , matlab_code_t<std::string::value_type>::type()//versionAPI path
			            , matlab_code_t<std::string::value_type>::type()//versionAPI compile_time

			            , matlab_code_t<bool>::type()//isConnected is_connected

			            , matlab_code_t<bool>::type()//isOpen is_opened

			            , matlab_code_t<bool>::type()//open is_opened

						//close

			            , matlab_code_t<utf32>::type()//wait event
			            , NUDT::CCustomer::EVENT_CONNECTED.c_str()//wait event connected
			            , NUDT::CCustomer::EVENT_DISCONNECTED.c_str()//wait event disconnected
			            , matlab_code_t<double>::type()//wait time
			            , matlab_code_t<int>::type()//wait result

			            , matlab_code_t<NUDT::program_id_t>::type()//id id

						, matlab_code_t<NUDT::program_id_t>::type()//customers id

						//join
            );
    }
}

