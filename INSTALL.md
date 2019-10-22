UDT Install Guide
=====================================================

The UDT build system is based on the cmake project builder. <br />
Before starting the installation, it is recommended that you read about it. <br />
 Further in the instruction the gui version of cmake will be used. <br />
 To avoid problems with cmake, you can clear the environment variables, <br />
 keeping on only the directories of the target compiler, <br />
 first of all it concerns the PATH variable. <br />
 (For example, if you build a project for mingw, <br />
 cmake can "pick up" sh.exe from another compiler).

# Requirements
### Common Requirements

* CMake-GUI for building everything:
	https://cmake.org

### For generation documentation with doxygen
* Doxygen
	https://www.doxygen.nl

### Requirements for Windows
* Microsoft Visual C++ or MinGW compiler
	http://www.mingw.org

### Requirements for GNU/Linux
* GNU Compiler Collection (GCC) compiler

### Requirements for QNX
* Installed QNX Build tools for QNX Version above 6.3.0 where has been 
	defined the following enviroments:
	- QNX_CONFIGURATION
	- QNX_HOST
	- QNX_TARGET
* QNX Momentics

# Dependencies:

| Title | Version | Required | Note |
| --- | --- | --- | --- |
| Boost | 1.47 and higher | Header only files | The required version <br /> is already in the <br /> UDT/SHARE/dependencies <br />directory|
| rapidjson | 1.02 and higher | Header only files | The required version <br /> is already in the <br /> UDT/SHARE/dependencies  <br />directory|
| Tclap | 1.2.1 and higher | Header only files | The required version <br />  is already in the <br /> UDT/SHARE/dependencies <br /> directory|
| Log4cplus | 1.1.3 and 1.2. * | No if option is not  <br /> specified LOGGING_TO_LOG4CPLUS | It compiles automatically  <br />  if the option is specified: <br />  LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE|

# ​Download the source
Download and extract the latest version of the source code from from [https://github.com/CrazyLauren/UDT](https://github.com/CrazyLauren/UDT)
press button "Clone or download"

# Building
## Build Options:

| Title | Description |
| --- | --- |
| *_WITH_STATIC_DEPENDENCIES | Compile the project with static dependencies. |
| *_BUILD_STATIC_TOO | Optionally build a static library |
| CUSTOMER_AVAILABLE_MODULES | List of existing add-on modules |
| CUSTOMER_WITH_STATIC_MODULES | Build the customer library <br />  with the modules specified in <br />  CUSTOMER _LIST_STATIC_MODULES |
| LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE | Also build log4cplus 1.2 |
| LOGGING_HAS_TO_BE_REMOVED | Delete all logs and assert from source code |
| LOGGING_IS_DISABLED_IN_SHARE | Disable logs only in SHARE libraries, but leave assertions |
| LOGGING_TO_LOG4CPLUS | For logging use the log4cplus library. **It is recommended to set this option.** |
| LOGGING_TO_COUT | Output logs to standard output stream |
Usually it’s enough to configure only project logging. For logging, <br /> 
 it is recommended to use the log4cplus library option LOGGING_TO_LOG4CPLUS,  <br /> 
 if the library is not installed on the system, when the option  <br />
  LOGGING_BUILD_LOG4CPLUS_FROM_SOURCE is set, it will be compiled automatically and statically linked.
  
## Microsoft Windows (Microsoft Visual C++)

1. Install Windows version of cmake-gui

2. Run `cmake-gui`

3. Press 'Browse Source' to set the source code directory. 
	This is the directory where the source code is located.
	
4. Press 'Browse Build' to set the build directory. That  
	is where Microsoft Visual C++ will build the source and 
	will create many many files.
	This should be an empty or non-existent folder that will be created automatic.	
	
5. Press button `configure`
	a) In the window that appears, select requirement Visual Studio version
	b) Press button `finish`
	
6. A check will then be performed to ensure all source dependencies are exists.
	Wait until it's finished, of course, without errors.
	
	a) The compiled applications, libraries and configured headers are installed onto the 
		directory which indicated in the CMAKE_INSTALL_PREFIX CMake's variable.
		Thus, change the option for CMAKE_INSTALL_PREFIX. If You won't do it,
		the administrator (root) access can be required for the installation operation.	
	
	b) OPTIONAL: You can change the used boost version by changing the Boost_INCLUDE_DIR variable.
	
	c) OPTIONAL: You can enable multi-thread compilation by changing the BUILD_BY_MP variable
	
7. Press button `configure` again.

8. Press button  `generate`

9. Press button  `Open Project`

10. Click right mouse button on the project with the name `INSTALL` in `Solution Explorer`.

    a) Select `Build`
    The requirement build type is customized by the Solution configuration (see MSVC documentation)

11. Wait until the end of the build and installation operation

12. You can see the builded applications and libraries and the headers files
    in the directory that is specified in the variable CMAKE_INSTALL_PREFIX.

13. To build documentation, click right mouse button on the project with the name `doc` in `Solution Explorer`.

    a) Select 'Build'

## Microsoft Windows (MINGW32)

1. Install Windows version of cmake-gui

2. Install mingw32. The default options work

3. Add the mingw32 programs to the system PATH enviroment variable 
	(eg. append this string C:\mingw32\bin)
	
4. Run `cmake-gui`

5. Press `Browse Source` to set the source code directory. 
	This is the directory where the source code is located.
	
6. Press `Browse Build` to set the build directory. That  
	is where Microsoft Visual C++ will build the source and 
	will create many many files.
	This should be an empty or non-existent folder that will be created automatic.	
	
5. Press button `configure`
	
	a) In the window that appears, select "MinGW Makefiles"
	
	b) Press button `finish`
	
8. A check will then be performed to ensure all source dependencies are exists.
  Wait until it's finished, of course, without errors.

  a) The compiled applications, libraries and configured headers are installed onto the 
  	directory which indicated in the CMAKE_INSTALL_PREFIX CMake's variable.
  	Thus, change the option for CMAKE_INSTALL_PREFIX. If You won't do it,
  	the administrator (root) access can be required for the installation operation.

  b)  You can customize the requrement build type by changing the variable 
  	CMAKE_BUILD_TYPE to `Release` for compilation in release mode
  	or `Debug`

  c) OPTIONAL: You can change the used boost version by changing the Boost_INCLUDE_DIR variable.	

9. Press button `configure` again.

10. Press button  `generate`

11. Open a command shell (cmd.exe) and navigate to the folder specified  in `the build directory` of CMake-GUI	

12. Execute `mingw32-make install`

13. Wait until the end of the build and installation operation

14. You can see the builded applications and libraries and the headers files
    in the directory that is specified in the variable CMAKE_INSTALL_PREFIX.

## GNU/Linux
1. Run `cmake-gui`

2. Press `Browse Source` to set the source code directory.

3. Press `Browse Build` to set the build directory.

5. Press button `configure`
	
	a) In the window that appears, select "Unix Makefiles"
	
	b) Press button `finish`
	
5. A check will then be performed to ensure all source dependencies are exists.
  Wait until it's finished, of course, without errors.

  a) The compiled applications, libraries and configured headers are installed onto the 
  	directory which indicated in the CMAKE_INSTALL_PREFIX CMake's variable.
  	Thus, change the option for CMAKE_INSTALL_PREFIX. If You won't do it,
  	the administrator (root) access can be required for the installation operation.

  b)  You can customize the requrement build type by changing the variable 
  	CMAKE_BUILD_TYPE to "Release" for compilation in release mode
  	or "Debug"

  c) OPTIONAL: You can change the used boost version by changing the Boost_INCLUDE_DIR variable.	

6. Press button `configure` again.

7. Press button  `generate`

8. Open a shell and navigate to the folder specified  in `the build directory` of CMake-GUI	

9. Execute `make install`

10. Wait until the end of the build and installation operation

11. You can see the builded applications and libraries and the headers files
    in the directory that is specified in the variable CMAKE_INSTALL_PREFIX.

## QNX
For building in Windows, the sequence of actions is as follows (For Linux is similar):
1. Install Windows version of cmake-gui

2. Run `cmake-gui`

3. Press `Browse Source` to set the source code directory. 
	This is the directory where the source code is located.
	
4. Press `Browse Build` to set the build directory. That  
	is where The QNX Momentics will build the source (template directory).
	This should be an empty or non-existent folder that will be created automatic.
	
5. Press button `configure`
	
	a) In the window that appears, select "Eclipse CDT4 - Unix Makefiles"
	
	b) Choose `Specify toolchain file for cross-compiling`
	
	c) Press button `next`
	
	d) Specify toolchain that corresponds to the QNX version and platform
		from the directory "UDT\SHARE\CMakeModules\toolchain"
	
	e) Press button `finish`
	
6. A check will then be performed to ensure all source dependencies are exists. 
  Wait until it's finished, of course, without errors.

  a) The compiled applications, libraries and configured headers are installed onto the 
  	directory which indicated in the CMAKE_INSTALL_PREFIX CMake's variable.
  	Thus, change the option for CMAKE_INSTALL_PREFIX. If You won't do it,
  	the administrator (root) access can be required for the installation operation.

  b)  You can customize the requirement build type by changing the variable 
  	CMAKE_BUILD_TYPE to "Release" for compilation in release mode (add -O3 flag)
  	or "Debug" debug mode with -g flag

  c) OPTIONAL: You can choose the correct version of eclipse  that corresponds to 
  	QNX Momentics by changing the variable CMAKE_ECLIPSE_VERSION.
  	The requirement version of eclipse you can see into file .eclipseproduct in the 
  	QNX Momentics directory.

  d) OPTIONAL: You can change the used boost version be changing the Boost_INCLUDE_DIR variable.

  e) Choose the requirement version of compiler (3.3.5 and above) by changing the QNX_COMPILER_VERSION

7. Press button `configure` again.

8. Press button  `Generate`

9. Run QNX Momentics

  a) In the menu, choose `File -> Import`.

  b) Expand the `General folder` and select `Existing Projects into Workspace`

  c) Press button  `Next`

  d) Choose `Select root directory` and Press button  `Browse`

  e) Specify the path to the directory similar to `the build directory` of CMake-GUI

  f) Choose the project and press button `Finish`

10. Click right mouse button on the project in `Project Explorer`.
    Choose `Build Targets -> Build`.

    a) Select a target with the name `install`, for which the 'location' is empty.
    	This is usually the latest `install` target.

    b) Press button `Build`

11. Wait until the end of the build and installation operation

12. You can see the builded applications and libraries and the headers files
    in the directory that is specified in the variable CMAKE_INSTALL_PREFIX.

13. To build documentation, in the item 13 You need choose a target with the name `doc`
