/*
 * SerialFunction.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 23.03.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
 #if defined(__linux__)
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Socket/SerialFunction.h>

namespace NSHARE
{
namespace NSerial
{
using std::istringstream;
using std::ifstream;
using std::getline;
using std::vector;
using std::string;
using std::cout;
using std::endl;

static vector<string> glob(const vector<string>& patterns);
static string basename(const string& path);
static string dirname(const string& path);
static bool path_exists(const string& path);
static string realpath(const string& path);
static string usb_sysfs_friendly_name(const string& sys_usb_path);
static vector<string> get_sysfs_info(const string& device_path);
static string read_line(const string& file);
static string usb_sysfs_hw_string(const string& sysfs_path);
static string format(const char* format, ...);

vector<string> glob(const vector<string>& patterns)
{
	vector<string> paths_found;

	if (patterns.size() == 0)
		return paths_found;

	glob_t glob_results;

	int glob_retval = ::glob(patterns[0].c_str(), 0, NULL, &glob_results);

	vector<string>::const_iterator iter = patterns.begin();

	while (++iter != patterns.end())
	{
		glob_retval = ::glob(iter->c_str(), GLOB_APPEND, NULL, &glob_results);
	}

	for (int path_index = 0; path_index < glob_results.gl_pathc; path_index++)
	{
		paths_found.push_back(glob_results.gl_pathv[path_index]);
	}

	globfree(&glob_results);

	return paths_found;
}

string basename(const string& path)
{
	size_t pos = path.rfind("/");

	if (pos == std::string::npos)
		return path;

	return string(path, pos + 1, string::npos);
}

string dirname(const string& path)
{
	size_t pos = path.rfind("/");

	if (pos == std::string::npos)
		return path;
	else if (pos == 0)
		return "/";

	return string(path, 0, pos);
}

bool path_exists(const string& path)
{
	struct stat sb;

	if (stat(path.c_str(), &sb) == 0)
		return true;

	return false;
}

string realpath(const string& path)
{
	char* real_path = ::realpath(path.c_str(), NULL);

	string result;

	if (real_path != NULL)
	{
		result = real_path;

		free(real_path);
	}

	return result;
}

string usb_sysfs_friendly_name(const string& sys_usb_path)
{
	unsigned int device_number = 0;

	istringstream(read_line(sys_usb_path + "/devnum")) >> device_number;

	string manufacturer = read_line(sys_usb_path + "/manufacturer");

	string product = read_line(sys_usb_path + "/product");

	string serial = read_line(sys_usb_path + "/serial");

	if (manufacturer.empty() && product.empty() && serial.empty())
		return "";

	return format("%s %s %s", manufacturer.c_str(), product.c_str(),
			serial.c_str());
}

vector<string> get_sysfs_info(const string& device_path)
{
	string device_name = basename(device_path);

	string friendly_name;

	string hardware_id;

	string sys_device_path = format("/sys/class/tty/%s/device",
			device_name.c_str());

	if (device_name.compare(0, 6, "ttyUSB") == 0)
	{
		sys_device_path = dirname(dirname(realpath(sys_device_path)));

		if (path_exists(sys_device_path))
		{
			friendly_name = usb_sysfs_friendly_name(sys_device_path);

			hardware_id = usb_sysfs_hw_string(sys_device_path);
		}
	}
	else if (device_name.compare(0, 6, "ttyACM") == 0)
	{
		sys_device_path = dirname(realpath(sys_device_path));

		if (path_exists(sys_device_path))
		{
			friendly_name = usb_sysfs_friendly_name(sys_device_path);

			hardware_id = usb_sysfs_hw_string(sys_device_path);
		}
	}
	else
	{
		// Try to read ID string of PCI device

		string sys_id_path = sys_device_path + "/id";

		if (path_exists(sys_id_path))
			hardware_id = read_line(sys_id_path);
	}

	if (friendly_name.empty())
		friendly_name = device_name;

	if (hardware_id.empty())
		hardware_id = "n/a";

	vector<string> result;
	result.push_back(friendly_name);
	result.push_back(hardware_id);

	return result;
}

string read_line(const string& file)
{
	ifstream ifs(file.c_str(), ifstream::in);

	string line;

	if (ifs)
	{
		getline(ifs, line);
	}

	return line;
}

string format(const char* format, ...)
{
	va_list ap;

	size_t buffer_size_bytes = 256;

	string result;

	char* buffer = (char*) malloc(buffer_size_bytes);

	if (buffer == NULL)
		return result;

	bool done = false;

	unsigned int loop_count = 0;

	while (!done)
	{
		va_start(ap, format);

		int return_value = vsnprintf(buffer, buffer_size_bytes, format, ap);

		if (return_value < 0)
		{
			done = true;
		}
		else if (return_value >= buffer_size_bytes)
		{
			// Realloc and try again.

			buffer_size_bytes = return_value + 1;

			char* new_buffer_ptr = (char*) realloc(buffer, buffer_size_bytes);

			if (new_buffer_ptr == NULL)
			{
				done = true;
			}
			else
			{
				buffer = new_buffer_ptr;
			}
		}
		else
		{
			result = buffer;
			done = true;
		}

		va_end(ap);

		if (++loop_count > 5)
			done = true;
	}

	free(buffer);

	return result;
}

string usb_sysfs_hw_string(const string& sysfs_path)
{
	string serial_number = read_line(sysfs_path + "/serial");

	if (serial_number.length() > 0)
	{
		serial_number = format("SNR=%s", serial_number.c_str());
	}

	string vid = read_line(sysfs_path + "/idVendor");

	string pid = read_line(sysfs_path + "/idProduct");

	return format("USB VID:PID=%s:%s %s", vid.c_str(), pid.c_str(),
			serial_number.c_str());
}

void get_all_ports(ports_t *aTo)
{

	vector < std::string > search_globs;
	search_globs.push_back("/dev/ttyACM*");
	search_globs.push_back("/dev/ttyS*");
	search_globs.push_back("/dev/ttyUSB*");
	search_globs.push_back("/dev/tty.*");
	search_globs.push_back("/dev/cu.*");

	std::vector < std::string > devices_found = glob(search_globs);

	std::vector<std::string>::iterator iter = devices_found.begin();

	while (iter != devices_found.end())
	{
		std::string device = *iter++;

		std::vector < std::string > sysfs_info = get_sysfs_info(device);

		std::string friendly_name = sysfs_info[0];

		std::string hardware_id = sysfs_info[1];

		port_info_t device_entry;
		device_entry.FPort = device;
		device_entry.FDescription = friendly_name;
		device_entry.FHardwareId = hardware_id;

		aTo->push_back(device_entry);

	}

}
}
}

#elif defined(_WIN32)//#if defined(unix) || defined(__QNX__)
#include <deftype>
#include <tchar.h>
#include <windows.h>
#include <initguid.h>
#include <setupapi.h>
#include <devguid.h>
#include <cstring>
#include <Socket/SerialFunction.h>
#include <type_utf8.h>

static const DWORD port_name_max_length = 256;
static const DWORD friendly_name_max_length = 256;
static const DWORD hardware_id_max_length = 256;

namespace NSHARE
{
	namespace NSerial
	{
		using std::vector;
		using std::string;
		using NSHARE::utf8_encode;

		extern void get_all_ports(ports_t *aTo)
		{
			CHECK_NOTNULL(aTo);
			ports_t& devices_found=*aTo;

			HDEVINFO device_info_set = SetupDiGetClassDevs(
					(const GUID *) &GUID_DEVCLASS_PORTS,
					NULL,
					NULL,
					DIGCF_PRESENT);

			unsigned int device_info_set_index = 0;
			SP_DEVINFO_DATA device_info_data;

			device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

			while(SetupDiEnumDeviceInfo(device_info_set, device_info_set_index, &device_info_data))
			{
				device_info_set_index++;

				// Get port name

				HKEY hkey = SetupDiOpenDevRegKey(
						device_info_set,
						&device_info_data,
						DICS_FLAG_GLOBAL,
						0,
						DIREG_DEV,
						KEY_READ);

				TCHAR port_name[port_name_max_length];
				DWORD port_name_length = port_name_max_length;

				LONG return_code = RegQueryValueEx(
						hkey,
						_T("PortName"),
						NULL,
						NULL,
						(LPBYTE)port_name,
						&port_name_length);

				RegCloseKey(hkey);

				if(return_code != EXIT_SUCCESS)
				continue;

				if(port_name_length > 0 && port_name_length <= port_name_max_length)
				port_name[port_name_length-1] = '\0';
				else
				port_name[0] = '\0';

				// Ignore parallel ports

				if(_tcsstr(port_name, _T("LPT")) != NULL)
				continue;

				// Get port friendly name

				TCHAR friendly_name[friendly_name_max_length];
				DWORD friendly_name_actual_length = 0;

				BOOL got_friendly_name = SetupDiGetDeviceRegistryProperty(
						device_info_set,
						&device_info_data,
						SPDRP_FRIENDLYNAME,
						NULL,
						(PBYTE)friendly_name,
						friendly_name_max_length,
						&friendly_name_actual_length);

				if(got_friendly_name != FALSE && friendly_name_actual_length > 0)
				friendly_name[friendly_name_actual_length-1] = '\0';
				else
				friendly_name[0] = '\0';

				// Get hardware ID

				TCHAR hardware_id[hardware_id_max_length];
				DWORD hardware_id_actual_length = 0;

				BOOL got_hardware_id = SetupDiGetDeviceRegistryProperty(
						device_info_set,
						&device_info_data,
						SPDRP_HARDWAREID,
						NULL,
						(PBYTE)hardware_id,
						hardware_id_max_length,
						&hardware_id_actual_length);

				if(got_hardware_id != FALSE && hardware_id_actual_length > 0)
				hardware_id[hardware_id_actual_length-1] = '\0';
				else
				hardware_id[0] = '\0';

#ifdef UNICODE
				std::string portName = utf8_encode(port_name);
				std::string friendlyName = utf8_encode(friendly_name);
				std::string hardwareId = utf8_encode(hardware_id);
#else
				std::string portName = port_name;
				std::string friendlyName = friendly_name;
				std::string hardwareId = hardware_id;
#endif

				port_info_t port_entry;
				port_entry.FPort = portName;
				port_entry.FDescription = friendlyName;
				port_entry.FHardwareId = hardwareId;

				devices_found.push_back(port_entry);
			}

			SetupDiDestroyDeviceInfoList(device_info_set);

		}
	} //namespace NSerial
} //namespace NSHARE
#elif defined(__QNX__)//#elif defined(_WIN32)
#include <deftype>
#include <Socket/SerialFunction.h>
#include<dirent.h>
#include<string.h>

#define DEV_DIR "/dev"
#define PREFIX_PORT "ser"
namespace NSHARE
{
	namespace NSerial
	{
		void get_all_ports(ports_t *aTo)
		{
			DIR *dp;
			dp = opendir(DEV_DIR);
			if (dp)
			{
				for (dirent *d; (d = readdir(dp));)
				{
					port_info_t port_entry;
					if (!strncmp(d->d_name, PREFIX_PORT, strlen(PREFIX_PORT)))
					{
						Strings::value_type _str(DEV_DIR);
						_str += "/";
						_str += d->d_name;
						port_entry.FPort = _str;
						aTo->push_back(port_entry);
					}
				}
				closedir(dp);
			}
		}
	}
}
#endif//#elif defined(_WIN32)
