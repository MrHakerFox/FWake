/*
 * main.cpp
 *
 * Description	: Wake Base Mechanism
 *
 *  Created on	: 09.06.2013
 *      Author	: Fox
 *
 *  Revisions	:
 */



#include "FWakeBase.hpp"


extern bool wakePHYSend( uint8_t* src, uint8_t size );
extern bool wakePHYGet( uint8_t* dst, uint8_t* size );
extern bool wakePHYPurge();



FRetVals::value FWakeBase::sendPacket( uint8_t TxAddr, uint8_t TxCmd, uint8_t TxNumOfBytes )
{
	lastTx.addr 	= TxAddr;
	lastTx.cmd 		= TxCmd;
	lastTx.n 		= TxNumOfBytes;

	txTotal++;


	crc = WAKE_INITIAL_CRC;
	uint8_t tmpBuff[ WAKE_BUFFER_TOTAL_SIZE ];
	pBuff = tmpBuff;
	cnt = 0;

	// if MSB is set then return error: given address must be less than 128. Because MSB is going to be set
	// before we transmit the packet
	if( !isAddressValid( TxAddr ) )
	{
		txTotalFailed++;
		return FRetVals::WakeAddrOutOfRange;
	}

	// if MSB is set then return error: given command's value must be less than 128.
	if( TxCmd & 0x80 )
	{
		txTotalFailed++;
		return FRetVals::WakeCmdOutOfRange;
	}

	if( TxNumOfBytes > F_WAKE_SETTINGS::BUFF_SIZE )
	{
		txTotalFailed++;
		return FRetVals::WakeBuffOverflow;
	}

	*pBuff++ = FEND;
	cnt++;
	crc = crc8_1wire( crc, FEND );

	// Send address if it isn't zero.
	staff_tx( TxAddr | 0x80 );
	crc = crc8_1wire( crc, TxAddr );

	// Send command
	*pBuff++ = TxCmd;
	cnt++;
	crc = crc8_1wire( crc, TxCmd );

	// Send size of the data
	staff_tx( TxNumOfBytes );
	crc = crc8_1wire( crc, TxNumOfBytes );

	// Send the rest of the packet (data)
	uint8_t i = 0;
	while( TxNumOfBytes-- )
	{
		staff_tx( txBuff[ i ] );
		crc = crc8_1wire( crc, txBuff[ i++ ] );
	}

	// Send CRC8
	staff_tx( crc );

	if( wakePHYSend( tmpBuff, cnt ) )
		return FRetVals::OK;
	else
	{
		txTotalFailed++;
		return FRetVals::WakePHYTXError;
	}

}



FRetVals::value FWakeBase::getPacket( uint8_t* RxAddr, uint8_t* RxCmd, uint8_t* RxNumOfBytes, uint8_t MatchAddr )
{
	rxTotal++;

	uint8_t crc = WAKE_INITIAL_CRC;

	uint8_t tmpBuff[ WAKE_BUFFER_TOTAL_SIZE ];
	uint8_t size;

	// Wait for a start frame
	bool res = wakePHYGet( tmpBuff, &size );
	if( !res )
	{
		rxTotalFailed++;
		return FRetVals::WakePHYRXError;
	}

	crc = crc8_1wire( crc, FEND );

	pBuff = tmpBuff;

	// Get address
	staff_rx( RxAddr );
	( *RxAddr ) &= ~0x80;
	lastRx.addr 	= *RxAddr;

	crc = crc8_1wire( crc, *RxAddr );

	// Get the command
	staff_rx( RxCmd );
	lastRx.cmd 		= *RxCmd;
	crc = crc8_1wire( crc, *RxCmd );

	// Get the size of data
	staff_rx( RxNumOfBytes );
	lastRx.n 		= *RxNumOfBytes;
	crc = crc8_1wire( crc, *RxNumOfBytes );

	// Receive the data
	for( uint8_t i = 0; ( i < *RxNumOfBytes ) && ( i < F_WAKE_SETTINGS::BUFF_SIZE ); i++ )
	{
		staff_rx( &rxBuff[ i ] );
		crc = crc8_1wire( crc, rxBuff[ i ] );
	}

	// Receive CRC
	uint8_t rxCrc;
	staff_rx( &rxCrc );

	if(rxCrc != crc )
	{
		rxTotalFailed++;
		return FRetVals::WakeCRC;
	}

	// Is this really address?
	if( !isAddressValid( *RxAddr ) )
	{
		rxTotalFailed++;
		return FRetVals::WakeAddrOutOfRange;
	}

	if( MatchAddr )
	{
		if( *RxAddr != MatchAddr )
		{
			rxTotalFailed++;
			return FRetVals::WakeAddrMatchFailed;
		}
	}

	// if MSB is set then return error: given command's value must be less than 128.
	if( *RxCmd & 0x80 )
	{
		rxTotalFailed++;
		return FRetVals::WakeCmdOutOfRange;
	}

	if( *RxNumOfBytes > F_WAKE_SETTINGS::BUFF_SIZE )
	{
		rxTotalFailed++;
		return FRetVals::WakeBuffOverflow;
	}

	return FRetVals::OK;
}



FRetVals::value FWakeBase::purge() const
{
	return wakePHYPurge() ? FRetVals::OK : FRetVals::FAILED;
}



FRetVals::value FWakeBase::setData( uint8_t data, uint8_t ind )
{
	if( ind > ( F_WAKE_SETTINGS::BUFF_SIZE - 1 ) )
		return FRetVals::WakeOutOfRange;

	txBuff[ ind ] = data;

	return FRetVals::OK;
}



FRetVals::value FWakeBase::setData( uint8_t *src, uint8_t size )
{
	if( size > F_WAKE_SETTINGS::BUFF_SIZE )
		return FRetVals::WakeOutOfRange;

	uint8_t* p = txBuff;

	while( size-- )
		*p++ = *src++;

	return FRetVals::OK;
}



FRetVals::value FWakeBase::getData( uint8_t* data, uint8_t ind ) const
{
	if( ind > ( F_WAKE_SETTINGS::BUFF_SIZE - 1 ) )
		return FRetVals::WakeOutOfRange;

	*data = rxBuff[ ind ];

	return FRetVals::OK;
}



FRetVals::value FWakeBase::getDataBuff( uint8_t* dst, uint8_t size )
{
	if( size > F_WAKE_SETTINGS::BUFF_SIZE )
		return FRetVals::WakeOutOfRange;

	uint8_t* p = rxBuff;

	while( size-- )
		*dst++ = *p++;

	return FRetVals::OK;
}



uint8_t FWakeBase::getData( uint8_t ind ) const
{
	if( ind > ( F_WAKE_SETTINGS::BUFF_SIZE - 1 ) )
		return FRetVals::WakeOutOfRange;

	return rxBuff[ ind ];
}



bool			FWakeBase::isAddressValid( uint8_t addr ) const
{
	return ( !( addr & 0x80 ) && addr) ;
}



uint8_t			FWakeBase::getSizeOfBuffer() const
{
	return F_WAKE_SETTINGS::BUFF_SIZE;
}



FWakeHeader		FWakeBase::getLastTxHeader() const
{
	return lastTx;
}



FWakeHeader		FWakeBase::getLastRxHeader() const
{
	return lastRx;
}



void FWakeBase::staff_tx( uint8_t byte )
{
	if( byte == FEND )
	{
		*pBuff++ = FESC;
		*pBuff++ = TFEND;
		cnt += 2;
	}
	else
	{
		if( byte == FESC )
		{
			*pBuff++ = FESC;
			*pBuff++ = TFESC;
			cnt += 2;
		}
		else
		{
			*pBuff++ = byte;
			cnt++;
		}
	}
}



void FWakeBase::staff_rx( uint8_t* byte )
{
	*byte = *pBuff++;
	if( *byte == FESC )
	{
		if( *pBuff++ == TFEND )
			*byte = FEND;
		else
			*byte = FESC;
	}
}





