//============================================================================
// Name        : 	FPHY.cpp
// Author      : 	Yakimov Y.A.
// Version     :
// Copyright   : 	Deltafox corp.
// Description : 	PHY driver for Win32
// Created on  :	14.08.2011
// History	   :	20120523 - comment added
//					20120523 - argument 'int baude' has been removed
//					20120617 - timeouts from 'FWakeConfig.h' added, function 'void delayBeforeAttempt' added
//					20130619 - rewritten under NG
//============================================================================



#include <FPHY.hpp>



HANDLE hSerial;
DCB dcbSerialParams = { 0 };
COMMTIMEOUTS timeouts = { 0 };



/*
	Function 'wakePHYOpen'

	Description: this function opens serial port

	IN:		*portname	- pointer to port name string

	OUT:	see FRetVals.hpp file
 */
FRetVals::value wakePHYOpen( char* portname )
{

	hSerial = CreateFile( portname,
							GENERIC_READ | GENERIC_WRITE,
							0,
							0,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							0 );

	if( hSerial == INVALID_HANDLE_VALUE )
	{
		return FRetVals::WakePHYGeneralError;
	}

	dcbSerialParams.DCBlength = sizeof( dcbSerialParams );

	if( !GetCommState( hSerial, &dcbSerialParams ) )
	{
		CloseHandle( hSerial );
		return FRetVals::WakePHYGeneralError;
	}

	bool baudFlag = false;

	switch( F_WAKE_SETTINGS::BAUDRATE )
	{
	case 19200:
		baudFlag = true;
		dcbSerialParams.BaudRate = CBR_19200;
		break;

	case 38400:
		baudFlag = true;
		dcbSerialParams.BaudRate = CBR_38400;
		break;

	case 57600:
		baudFlag = true;
		dcbSerialParams.BaudRate = CBR_57600;
		break;

	case 115200:
		baudFlag = true;
		dcbSerialParams.BaudRate = CBR_115200;
		break;
	}

	if( !baudFlag )
	{
		CloseHandle( hSerial );
		return FRetVals::WakePHYGeneralError;
	}

	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;

	if( !SetCommState( hSerial, &dcbSerialParams ) )
	{
		CloseHandle( hSerial );
		return FRetVals::WakePHYGeneralError;
	}

	timeouts.ReadIntervalTimeout 			= F_WAKE_SETTINGS::ONE_BYTE_RX_TIMEOUT;
	timeouts.ReadTotalTimeoutConstant 		= F_WAKE_SETTINGS::START_FRAME_TIMEOUT;
	timeouts.ReadTotalTimeoutMultiplier 	= F_WAKE_SETTINGS::ONE_BYTE_RX_TIMEOUT;
	timeouts.WriteTotalTimeoutConstant 		= 1;
	timeouts.WriteTotalTimeoutMultiplier 	= 10;

	if( !SetCommTimeouts( hSerial, &timeouts ) )
	{
		CloseHandle( hSerial );
		return FRetVals::WakePHYGeneralError;
	}

	return FRetVals::OK;
}



/*
	Function 'wakePHYSend'

	Description: this function sends wake packet via serial port

	IN:		src		- a pointer to a buffer that contains data
			size	- number of bytes to be sent

	OUT:	true if success, false if failed
 */
bool  wakePHYSend( uint8_t* src, uint8_t size )
{
	//printf( "\r\nwakePHYSend: src %08x, size %i", src, size );
	DWORD byteWrote;
	return ( WriteFile( hSerial, src, size, &byteWrote, NULL ) && ( byteWrote == size ) );
}



/*
	Function 'wakePHYGet'

	Description: this function gets wake packet via serial port

	IN:		*dst		- a pointer to a buffer where received data will be put to
			*size		- number of how many bytes have been received (without FRAME START FEND)

	OUT:	true if success, false if failed
 */
bool  wakePHYGet( uint8_t* dst, uint8_t* size )
{
	DWORD byteRead;

	//printf( "\r\n1" );
	uint8_t frameStart;
	if( !ReadFile( hSerial, &frameStart, 1, &byteRead, NULL ) )
		return false;

	//printf( "\r\n2" );
	if( frameStart != FEND )
		return false;

	//printf( "\r\n3" );
	bool res = ( ReadFile( hSerial, dst, WAKE_BUFFER_TOTAL_SIZE, &byteRead, NULL ) );
	*size = byteRead;
	//printf( "\r\n4, read %i", byteRead );
	return res && byteRead;
}



/*
	Function 'wakePHYPurge'

	Description: this function clears all buffers

	IN:		none

	OUT:	result of open: true if success, false if failed
 */
bool  wakePHYPurge()
{
	 return PurgeComm( hSerial, PURGE_TXABORT |
	                        PURGE_RXABORT |
	                        PURGE_TXCLEAR |
	                        PURGE_RXCLEAR);
}



/*
	Function 'wakePHYClose'

	Description: this function really (as compared with AVR) closes serial port.

	IN:		none

	OUT:	result of open: true if success, false if failed
 */
bool wakePHYClose()
{
	return  CloseHandle( hSerial );
}

