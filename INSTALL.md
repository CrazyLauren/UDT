UDT Install Guide
=====================================================

Requirements
------------
### Common Requirements

* CMake-GUI for building everything:
	https://cmake.org
* Boost (at least 1.47) is needed, but only template libraries, so no
  system wide installation or static linking is needed here:

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


	
Download the source
------------
Download and extract the latest version of the source code from from https://github.com/CrazyLauren/UDT
press button "Clone or download"
 
Microsoft Windows (Microsoft Visual C++)
-----------------

1. Install Windows version of cmake-gui
2. Run `cmake-gui.exe`
3. Press 'Browse Source' to set the source code directory. 
	This is the directory where the source code is located.
4. Press 'Browse Build' to set the build directory. That  
	is where Microsoft Visual C++ will build the source and 
	will create many many files.
	This should be an empty or non-existent folder that will be created automatic.	
5. Press button `configure`
	a) In the window that appears, select requrement Visual Studio version
	b) Press button `finish`
6. A check will then be performed to ensure all source dependencies are exists.
	Wait until it's finished, of course, without errors.
	a) The compiled applications, libraries and configured headers are installed onto the 
		directory which indicated in the CMAKE_INSTALL_PREFIX CMake's variable.
		Thus, change the option for CMAKE_INSTALL_PREFIX. If You won't do it,
		the administrator (root) access can be required for the installation operation.	
	b) OPTIONAL: You can change the used boost version by changing the Boost_INCLUDE_DIR variable.
	c) OPTIONAL: You can enable multithread compilation by changing the BUILD_BY_MP variable
7. Press button `configure` again.
8. Press button  'Generate'
9. Press button  'Open Project'
10. Click right mouse button on the project with the name "INSTALL" in 'Solution Explorer'.
	a) Select 'Build'
	The requrement build type is customized by the Solution configuration (see MSVC documentation)
11. Wait until the end of the build and installation operation
12. You can see the builded applications and libraries and the headers files
	in the directory that is specified in the variable CMAKE_INSTALL_PREFIX.
13. To build documentation, click right mouse button on the project with the name "doc" in 'Solution Explorer'.
	a) Select 'Build'

Microsoft Windows (MINGW32)
-----------------

1. Install Windows version of cmake-gui
2. Install Mingw32. The default options work
3. Add the mingw32 programs to the system PATH enviroment variable 
	(eg. append this string C:\mingw32\bin)
4. Run `cmake-gui.exe`
5. Press 'Browse Source' to set the source code directory. 
	This is the directory where the source code is located.
6. Press 'Browse Build' to set the build directory. That  
	is where Microsoft Visual C++ will build the source and 
	will create many many files.
	This should be an empty or non-existent folder that will be created automatic.	
5. Press button `configure`
	a) In the window that appears, select "MinGW Makefiles"
	b) Press button `finish`
6. A check will then be performed to ensure all source dependencies are exists.
	Wait until it's finished, of course, without errors.
	a) The compiled applications, libraries and configured headers are installed onto the 
		directory which indicated in the CMAKE_INSTALL_PREFIX CMake's variable.
		Thus, change the option for CMAKE_INSTALL_PREFIX. If You won't do it,
		the administrator (root) access can be required for the installation operation.
	b)  You can customize the requrement build type by changing the variable 
		CMAKE_BUILD_TYPE to "Release" for compilation in release mode
		or "Debug"
	c) OPTIONAL: You can change the used boost version by changing the Boost_INCLUDE_DIR variable.	
7. Press button `configure` again.
8. Press button  'Generate'
9. Open a command shell (cmd.exe) and navigate to the folder specified  in 'the build directory' of CMake-GUI	
10. Execute mingw32-make install
11. Wait until the end of the build and installation operation
12. You can see the builded applications and libraries and the headers files
	in the directory that is specified in the variable CMAKE_INSTALL_PREFIX.
	
GNU/Linux
---------
1. Run `cmake-gui`
2. Press 'Browse Source' to set the source code directory.
3. Press 'Browse Build' to set the build directory.
5. Press button `configure`
	a) In the window that appears, select "Unix Makefiles"
	b) Press button `finish`
6. A check will then be performed to ensure all source dependencies are exists.
	Wait until it's finished, of course, without errors.
	a) The compiled applications, libraries and configured headers are installed onto the 
		directory which indicated in the CMAKE_INSTALL_PREFIX CMake's variable.
		Thus, change the option for CMAKE_INSTALL_PREFIX. If You won't do it,
		the administrator (root) access can be required for the installation operation.
	b)  You can customize the requrement build type by changing the variable 
		CMAKE_BUILD_TYPE to "Release" for compilation in release mode
		or "Debug"
	c) OPTIONAL: You can change the used boost version by changing the Boost_INCLUDE_DIR variable.	
7. Press button `configure` again.
8. Press button  'generate'
9. Open a shell and navigate to the folder specified  in 'the build directory' of CMake-GUI	
10. Execute make install
11. Wait until the end of the build and installation operation
12. You can see the builded applications and libraries and the headers files
	in the directory that is specified in the variable CMAKE_INSTALL_PREFIX.
	
QNX
-----------------
For building in Windows, the sequence of actions is as follows (For Linux is similar):
1. Install Windows version of cmake-gui
2. Run `cmake-gui.exe`
3. Press 'Browse Source' to set the source code directory. 
	This is the directory where the source code is located.
4. Press 'Browse Build' to set the build directory. That  
	is where The QNX Momentics will build the source (template directory).
	This should be an empty or non-existent folder that will be created automatic.
5. Press button `configure`
	a) In the window that appears, select "Eclipse CDT4 - Unix Makefiles"
	b) Choise 'Specify toolchain file for cross-compiling'
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
	b)  You can customize the requrement build type by changing the variable 
		CMAKE_BUILD_TYPE to "Release" for compilation in release mode (add -O3 flag)
		or "Debug" debug mode with -g flag
	c) OPTIONAL: You can choose the correct version of eclipse  that corresponds to 
		QNX Momentics by changing the variable CMAKE_ECLIPSE_VERSION.
		The requrement version of eclipse you can see into file .eclipseproduct in the 
		QNX Momentics directory.
	d) OPTIONAL: You can change the used boost version be changing the Boost_INCLUDE_DIR variable.
	e) Choose the requrement version of compiler (3.3.5 and above) by changing the QNX_COMPILER_VERSION
7. Press button `configure` again.
8. Press button  'Generate'
9. Run QNX Momentics
	a) In the menu, choose File -> Import.
	b) Expand the General folder and select Existing Projects into Workspace
	c) Press button  'Next'
	d) Choose 'Select root directory' and Press button  'Browse'
	e) Specify the path to the directory similar to 'the build directory' of CMake-GUI
	f) Choose the project and press button 'Finish'
10. Click right mouse button on the project in 'Project Explorer'.
	Choose Build Targets -> Build.
	a) Select a target with the name 'install', for which the 'location' is empty.
		This is usually the latest 'install' target.
	b) Press button 'Build'
11. Wait until the end of the build and installation operation
12. You can see the builded applications and libraries and the headers files
	in the directory that is specified in the variable CMAKE_INSTALL_PREFIX.
13. To build documentation, in the item 13 You need choose a target with the name 'doc'
