/*
	WiFiLibrary.h

	By Jim Davies
	Jacobus Systems, Brighton & Hove, UK
	http://www.jacobus.co.uk

	Provided under the MIT license: https://github.com/jacobussystems/ArdJack/blob/master/LICENSE
	Copyright (c) 2019 James Davies, Jacobus Systems, Brighton & Hove, UK

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
	documentation files (the "Software"), to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions
	of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
	THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
	IN THE SOFTWARE.
*/

#pragma once


#ifdef ARDUINO

// Arduino only.

#include <arduino.h>

#include "DetectBoard.h"
#include "Globals.h"


//--------------------------------------------------------------------------------------------------------------------------
// IMPORTANT:
//--------------------------------------------------------------------------------------------------------------------------

// FOR VISUAL STUDIO + VISUAL MICRO, YOU NEED TO COMMENT/UNCOMMENT THE FOLLOWING THREE #includes TO MATCH THE BOARD IN USE.
// Tried using the ArdJack conditionals, but weird errors when compiling in Visual Studio + Visual Micro.
//		- Comment all 3 #includes for Arduino Due.
//		- Repeat this in Ardjack.ino.

// MKR1010 etc.
#include <WiFiNINA.h>

// ESP32 boards.
//#include <WiFi.h>

// Other boards with WiFi.
//#include <WiFi101.h>

//--------------------------------------------------------------------------------------------------------------------------


#endif

