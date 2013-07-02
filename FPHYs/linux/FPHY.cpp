/*
 * FPHY.cpp
 *
 *  Created on: 28.06.2013
 *      Author: Fox
 */



#include "FPHY.hpp"


int fd;
struct termios options;



/*
	Function 'wakePHYOpen'

	Description: this function opens serial port

	IN:		*portname	- pointer to port name string

	OUT:	see FRetVals.hpp file
 */
FRetVals::value wakePHYOpen( char* portname )
{
	fd = open( portname, O_RDWR | O_NDELAY | O_NOCTTY );

	if( fd == -1 )
	{
		printf( "\r\nCan't open serial port %s", portname );
		return FRetVals::WakePHYGeneralError;
	}

	//fcntl( fd, F_SETFL, FNDELAY);

	tcgetattr( fd, &options );

	switch( F_WAKE_SETTINGS::BAUDRATE )
	{
	case 38400:
		printf( "\r\nBaudrate 38400" );
		cfsetispeed( &options, B38400 );
		cfsetospeed( &options, B38400 );
		break;

	case 115200:
		cfsetispeed( &options, B115200 );
		cfsetospeed( &options, B115200 );
		break;

	default:
		return FRetVals::WakePHYBaudRateError;
	}

	options.c_cc[ VMIN ] = 1;
	options.c_cc[ VTIME ] = 100;

	options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
	                | INLCR | IGNCR | ICRNL | IXON);
	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	options.c_cflag &= ~(CSIZE | PARENB);
	options.c_cflag |= CS8;

	if( tcsetattr( fd, TCSANOW, &options ) == -1 )
		return FRetVals::WakePHYGeneralError;

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
	int wordsWritten = write( fd, src, size );

	if( wordsWritten == -1 )
	{
		printf( "\r\nSend1, errno %i", errno );
		return false;
	}

	printf( "\r\nSend2, size: %i, wordsWritten: %i\r\nData: ", size, wordsWritten );
	for( int i = 0; i < size; i++ )
		printf( "0x%02X ", *src++ );
	return wordsWritten == size;
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
	uint8_t byte;

	printf( "\r\nDEBUG FUNC wakePHYGet 1" );
	int wordsRead = read( fd, &byte, 1 );
	if( wordsRead != 1 )
	{
		printf( "\r\nDEBUG FUNC wakePHYGet 2, wordsRead %i", wordsRead );
		return false;
	}

	if( byte != FEND )
	{
		printf( "\r\nDEBUG FUNC wakePHYGet 3" );
		return false;
	}

	printf( "\r\nDEBUG FUNC wakePHYGet 4" );
	wordsRead = read( fd, dst, WAKE_BUFFER_TOTAL_SIZE );
	printf( "\r\nDEBUG FUNC wakePHYGet 5" );

	if( wordsRead == -1 )
	{
		printf( "\r\nDEBUG FUNC wakePHYGet 6" );
		return false;
	}

	printf( "\r\nDEBUG FUNC wakePHYGet 7, size %i", wordsRead );

	*size = wordsRead;

	return true;
}



/*
	Function 'wakePHYPurge'

	Description: this function clears all buffers

	IN:		none

	OUT:	result of open: true if success, false if failed
 */
bool  wakePHYPurge()
{
	tcflush(fd,TCIOFLUSH);
}



/*
	Function 'wakePHYClose'

	Description: this function really (as compared with AVR) closes serial port.

	IN:		none

	OUT:	result of open: true if success, false if failed
 */
bool wakePHYClose()
{
	return close( fd ) != -1;
}





