# Quick start!

In the installation directory you can find an example project: "example_number_one" (share/UDT/).
At the beginning build it for your favorite integrated development environment (IDE).

## Building the example project

### Qt creator

1. Do `Open file or project` and open the CMakeLists.txt file of "example_number_one".

2. Choose the Build directory properly and press `Configure project`. 
    Qt Creator will create a .cbp file here with the name of the "example_number_one" project.

3. Click the "example_number_one" button in left column and you will be able to set the command options to `build`.


### Microsoft Visual C++ or other IDE 
1. Run `cmake-gui`

2. Press `Browse Source` to set the "example_number_one" source code directory. 

3. Press button `configure`
	1) In the window that appears, select requirement Visual Studio version (or IDE)
	2) Press button `finish`
4. Press button `configure` again.

5. Press button  `generate`

6. Open IDE

## Example description 
If you have builded the documentation then do open it and input "example_easy_program" 
to the search field to see API of the example.

In a few words:

1. The “initialize_library” function initializes the library and subscribers to messages and events.
2. The "msg_test_handler" function handles received messages
3. The "send_messages" function publishes the messages (sends it)
4. The "event_new_receiver" function notifies you of new subscribers (The recipient of the message)
5. The "event_fail_sent_handler" function notifies you about undeliverable messages
6. The "event_customers_update_handler" function  function notifies you about connected(disconnected) programs to the Kernel
7. The "event_connect_handler" ("event_disconnect_handler") function notifies you about your connection to the Kernel.
    (When you can publishers (sends) messages)

## Run example
1. Run the "Kernel" program, see installation directory.
2. Run the "example_easy_program": 

Attention! Only one "Kernel" program can work in one port. If you run two copies, the behavior will be undefined.

## Other examples
You can see other examples in the source directory “examples”.  