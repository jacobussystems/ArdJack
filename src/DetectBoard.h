/*
	DetectBoard.h

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

// Defaults.
#define ARDJACK_BOARD_TYPE "UNKNOWN"
#define ARDJACK_SRAM 2

#undef ARDJACK_ARM
#undef ARDJACK_ETHERNET_AVAILABLE
#undef ARDJACK_FLASH_AVAILABLE
#undef ARDJACK_MALLOC_AVAILABLE
#undef ARDJACK_NETWORK_AVAILABLE
#undef ARDJACK_RTC_AVAILABLE
#undef ARDJACK_WIFI_AVAILABLE


#if defined(TEENSYDUINO) 
	//  --------------- TEENSY -----------------

	#if defined(__AVR_ATmega32U4__)
		#define TEENSY_2.0
		#define ARDJACK_BOARD_TYPE "TEENSY_2.0"
	#elif defined(__AVR_AT90USB1286__)       
		#define TEENSY++_2.0
		#define ARDJACK_BOARD_TYPE "TEENSY++_2.0"
	#elif defined(__MK20DX128__)       
		#define TEENSY_3.0
		#define ARDJACK_BOARD_TYPE "TEENSY_3.0"
	#elif defined(__MK20DX256__)       
		#define TEENSY_3.1
		#define TEENSY_3.2
		#define ARDJACK_BOARD_TYPE "TEENSY_3.1" // and TEENSY 3.2
	#elif defined(__MKL26Z64__)       
		#define TEENSY_LC
		#define ARDJACK_BOARD_TYPE "TEENSY_LC"
	#elif defined(__MK66FX1M0__)
		#define TEENSY++_3.0 // coming soon
		#define ARDJACK_BOARD_TYPE "TEENSY++_3.0" // coming soon
	#else
		#error "Unknown TEENSY board"
	#endif

#else
	// --------------- Adafruit? ------------------

	#if defined(ADAFRUIT_FEATHER_M0)
		#define ARDJACK_ARM
		#define ARDJACK_FEATHER_M0
		#define ARDJACK_BOARD_TYPE "ADAFRUIT_FEATHER_M0"
		//#define ARDJACK_FLASH_AVAILABLE
		#define ARDJACK_MALLOC_AVAILABLE
		//#define ARDJACK_RTC_AVAILABLE
		#define ARDJACK_SRAM 32
		#define ARDJACK_USE_RTCZERO
		#define ARDJACK_WIFI_AVAILABLE

	// --------------- Arduino? ------------------

	#elif defined(ARDUINO_ARC32_TOOLS)
		#define ARDUINO_101
		#define ARDJACK_BOARD_TYPE "ARDUINO_101"
	#elif defined(ARDUINO_AVR_ADK)       
		#define ARDUINO_ADK
		#define ARDJACK_BOARD_TYPE "ARDUINO_ADK"
	#elif defined(ARDUINO_AVR_BT)    // Bluetooth
		#define ARDUINO_BT
		#define ARDJACK_BOARD_TYPE "ARDUINO_BT"
	#elif defined(ARDUINO_AVR_DUEMILANOVE)       
		#define ARDUINO_DUEMILANOVE
		#define ARDJACK_BOARD_TYPE "ARDUINO_DUEMILANOVE"
	#elif defined(ARDUINO_AVR_ESPLORA)       
		#define ARDUINO_ESPLORA
		#define ARDJACK_BOARD_TYPE "ARDUINO_ESPLORA"
	#elif defined(ARDUINO_AVR_ETHERNET)       
		#define ARDUINO_ETHERNET
		#define ARDJACK_BOARD_TYPE "ARDUINO_ETHERNET"
	#elif defined(ARDUINO_AVR_FIO)       
		#define ARDUINO_FIO
		#define ARDJACK_BOARD_TYPE "ARDUINO_FIO"
	#elif defined(ARDUINO_AVR_GEMMA)
		#define ARDUINO_GEMMA
		#define ARDJACK_BOARD_TYPE "ARDUINO_GEMMA"
	#elif defined(ARDUINO_AVR_LEONARDO)       
		#define ARDUINO_LEONARDO
		#define ARDJACK_BOARD_TYPE "ARDUINO_LEONARDO"
	#elif defined(ARDUINO_AVR_LILYPAD)
		#define ARDUINO_LILYPAD
		#define ARDJACK_BOARD_TYPE "ARDUINO_LILYPAD"
	#elif defined(ARDUINO_AVR_LILYPAD_USB)
		#define ARDUINO_LILYPAD_USB
		#define ARDJACK_BOARD_TYPE "ARDUINO_LILYPAD_USB"
	#elif defined(ARDUINO_AVR_MEGA)       
		#define ARDUINO_MEGA
		#define ARDJACK_BOARD_TYPE "ARDUINO_MEGA"
	#elif defined(ARDUINO_AVR_MEGA2560)       
		#define ARDJACK_ARDUINO_MEGA2560
		#define ARDJACK_BOARD_TYPE "ARDUINO_MEGA_2560"
		#define ARDJACK_SRAM 8
	#elif defined(ARDUINO_AVR_MICRO)       
		#define ARDUINO_MICRO
		#define ARDJACK_BOARD_TYPE "ARDUINO_MICRO"
	#elif defined(ARDUINO_AVR_MINI)       
		#define ARDUINO_MINI
		#define ARDJACK_BOARD_TYPE "ARDUINO_MINI"
	#elif defined(ARDUINO_AVR_NANO)       
		#define ARDUINO_NANO
		#define ARDJACK_BOARD_TYPE "ARDUINO_NANO"
	#elif defined(ARDUINO_AVR_NG)       
		#define ARDUINO_NG
		#define ARDJACK_BOARD_TYPE "ARDUINO_NG"
	#elif defined(ARDUINO_AVR_PRO)       
		#define ARDUINO_PRO
		#define ARDJACK_BOARD_TYPE "ARDUINO_PRO"
	#elif defined(ARDUINO_AVR_ROBOT_CONTROL)       
		#define ARDUINO_ROBOT_CONTROL
		#define ARDJACK_BOARD_TYPE "ARDUINO_ROBOT_CONTROL"
	#elif defined(ARDUINO_AVR_ROBOT_MOTOR)       
		#define ARDUINO_ROBOT_MOTOR
		#define ARDJACK_BOARD_TYPE "ARDUINO_ROBOT_MOTOR"
	#elif defined(ARDUINO_AVR_UNO)       
		#define ARDJACK_ARDUINO_UNO
		#define ARDJACK_BOARD_TYPE "ARDUINO_UNO"
	#elif defined(ARDUINO_AVR_YUN)       
		#define ARDUINO_YUN
		#define ARDJACK_BOARD_TYPE "ARDUINO_YUN"

	#elif defined(ARDUINO_SAM_DUE)
		#define ARDJACK_ARM
		#define ARDJACK_ARDUINO_DUE
		#define ARDJACK_BOARD_TYPE "ARDUINO_DUE"
		//#define ARDJACK_ETHERNET_AVAILABLE
		#define ARDJACK_MALLOC_AVAILABLE
		//#define ARDJACK_RTC_AVAILABLE
		#define ARDJACK_SRAM 96

	#elif defined(ARDUINO_SAMD_MKR1000)
		#define ARDJACK_ARM
		#define ARDJACK_ARDUINO_MKR
		#define ARDJACK_BOARD_TYPE "ARDUINO_MKR1000"
		//#define ARDJACK_FLASH_AVAILABLE
		#define ARDJACK_MALLOC_AVAILABLE
		//#define ARDJACK_RTC_AVAILABLE
		#define ARDJACK_SRAM 32
		#define ARDJACK_USE_RTCZERO
		//#define ARDJACK_WIFI_AVAILABLE

	#elif defined(ARDUINO_SAMD_MKRFox1200)
		#define ARDJACK_ARM
		#define ARDJACK_ARDUINO_MKR
		#define ARDJACK_BOARD_TYPE "ARDUINO_MKR_FOX_1200"
		//#define ARDJACK_FLASH_AVAILABLE
		#define ARDJACK_MALLOC_AVAILABLE
		//#define ARDJACK_RTC_AVAILABLE
		#define ARDJACK_SRAM 32
		//#define ARDJACK_WIFI_AVAILABLE

	#elif defined(ARDUINO_SAMD_MKRWIFI1010)
		#define ARDJACK_ARM
		#define ARDJACK_ARDUINO_MKR
		#define ARDJACK_BOARD_TYPE "ARDUINO_MKR_WIFI_1010"
		//#define ARDJACK_FLASH_AVAILABLE
		#define ARDJACK_MALLOC_AVAILABLE
		//#define ARDJACK_RTC_AVAILABLE
		#define ARDJACK_SRAM 32
		#define ARDJACK_USE_RTCZERO
		#define ARDJACK_WIFI_AVAILABLE

	#elif defined(ARDUINO_SAMD_ZERO)
		#define ARDJACK_ARM
		#define ARDJACK_ARDUINO_SAMD_ZERO
		#define ARDJACK_BOARD_TYPE "ARDUINO_SAMD_ZERO"
		//#define ARDJACK_FLASH_AVAILABLE
		#define ARDJACK_MALLOC_AVAILABLE
		//#define ARDJACK_RTC_AVAILABLE
		#define ARDJACK_SRAM 32
		//#define ARDJACK_WIFI_AVAILABLE


	// --------------- Espressif? ------------------
	#elif defined(ESP32)
		#define ARDJACK_ESP32
		#define ARDJACK_BOARD_TYPE "ESPRESSIF_ESP32"
		//#define ARDJACK_FLASH_AVAILABLE
		#define ARDJACK_MALLOC_AVAILABLE
		//#define ARDJACK_RTC_AVAILABLE
		#define ARDJACK_SRAM 320
		#define ARDJACK_WIFI_AVAILABLE
		#define SERIAL_PORT_MONITOR Serial

	#else
		#error "Unknown board"

	#endif

#endif


#ifdef ARDJACK_ETHERNET_AVAILABLE
	#define ARDJACK_NETWORK_AVAILABLE
#endif

#ifdef ARDJACK_WIFI_AVAILABLE
	#define ARDJACK_NETWORK_AVAILABLE
#endif

#endif
