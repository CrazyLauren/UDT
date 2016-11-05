/*
 * console.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 22.06.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <console.h>
namespace NSHARE
{
namespace NCONSOLE

{
#ifdef _WIN32
	static HANDLE hStdout =INVALID_HANDLE_VALUE;// GetStdHandle(STD_OUTPUT_HANDLE);
#endif
void clear()
{
#ifdef __QNX__
	printf("\E[2J\n");
#elif defined(_WIN32)
	if(hStdout ==INVALID_HANDLE_VALUE)
		hStdout =GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout==INVALID_HANDLE_VALUE)
	{
		return;
	}
	COORD coordScreen = { 0, 0 };    // home for the cursor 
   DWORD cCharsWritten;
   CONSOLE_SCREEN_BUFFER_INFO csbi; 
   DWORD dwConSize;

// Get the number of character cells in the current buffer. 

   if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
   {
      return;
   }

   dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

   // Fill the entire screen with blanks.

   if( !FillConsoleOutputCharacter( hStdout,        // Handle to console screen buffer 
                                    (TCHAR) ' ',     // Character to write to the buffer
                                    dwConSize,       // Number of cells to write 
                                    coordScreen,     // Coordinates of first cell 
                                    &cCharsWritten ))// Receive number of characters written
   {
      return;
   }

   // Get the current text attribute.

   if( !GetConsoleScreenBufferInfo( hStdout, &csbi ))
   {
      return;
   }

   // Set the buffer's attributes accordingly.

   if( !FillConsoleOutputAttribute( hStdout,         // Handle to console screen buffer 
                                    csbi.wAttributes, // Character attributes to use
                                    dwConSize,        // Number of cells to set attribute 
                                    coordScreen,      // Coordinates of first cell 
                                    &cCharsWritten )) // Receive number of characters written
   {
      return;
   }

   // Put the cursor at its home coordinates.

   SetConsoleCursorPosition( hStdout, coordScreen );
#else
	throw std::domain_error("clear");
#endif
}

std::ostream&print_color(std::ostream& aStream,
		const NSHARE::NCONSOLE::eCode& aCode)
{
#ifdef _WIN32
	if(hStdout ==INVALID_HANDLE_VALUE)
		hStdout =GetStdHandle(STD_OUTPUT_HANDLE);

	static WORD _color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	if (hStdout!=INVALID_HANDLE_VALUE)
	{
		switch (aCode)
		{
		case eNORMAL:
			_color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case eFG_DEFAULT:
			_color |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case eFG_RED:
			_color = (_color & (~(FOREGROUND_GREEN | FOREGROUND_BLUE)));
			_color |= FOREGROUND_RED;
			break;
		case eFG_GREEN:
			_color = (_color & (~(FOREGROUND_RED | FOREGROUND_BLUE)));
			_color |= FOREGROUND_GREEN;
			break;
		case eFG_YELLOW:
			_color = (_color & (~FOREGROUND_RED));
			_color |= FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case eFG_BLUE:
			_color = (_color & (~(FOREGROUND_RED | FOREGROUND_GREEN)));
			_color |= FOREGROUND_BLUE;
			break;

		case eBG_DEFAULT:
			_color |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case eBG_RED:
			_color = (_color & (~(BACKGROUND_GREEN | BACKGROUND_BLUE)));
			_color |= BACKGROUND_RED;
			break;
		case eBG_GREEN:
			_color = (_color & (~(BACKGROUND_RED | BACKGROUND_BLUE)));
			_color = BACKGROUND_GREEN;
			break;
		case eBG_BLUE:
			_color = (_color & (~(BACKGROUND_RED | BACKGROUND_GREEN)));
			_color = BACKGROUND_BLUE;
			break;

		default:
			break;
		}
		SetConsoleTextAttribute(hStdout, _color);
	}
	return aStream;
#else
	NSHARE::NCONSOLE::CModifier const _mod(aCode);
	return aStream << _mod;
#endif
}
} //namespace NConsole
}
