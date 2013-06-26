/*
 * FWakeBase.hpp
 *
 *  Created on: 09.06.2013
 *      Author: Fox
 */

#ifndef FWAKEBASE_HPP_
#define FWAKEBASE_HPP_



#include <inttypes.h>



#include <FRetVals.hpp>
#include <FCRC.hpp>
#include <FWakeConfig.hpp>



// Frame End
const uint8_t FEND = 0xC0;
// Frame Escape
const uint8_t FESC = 0xDB;
// Transposed Frame End
const uint8_t TFEND = 0xDC;
// Transposed Frame Escape
const uint8_t TFESC = 0xDD;



// Standard commands
namespace FWakeCMD
{
// No operation
const uint8_t NOP = 0;
// Error code
const uint8_t ERR = 1;
// Request for transmitted packet
const uint8_t ECHO = 2;
// Request for information about device
const uint8_t INFO = 3;
// My extension: device transmit this to master in case of unknown command
const uint8_t UNDEF	= 4;
// My extension: restart device (just restart, enter bootloader... )
const uint8_t BOOT_START = 7;
}


const uint8_t WAKE_HEADER	= 	1 +	// Frame Start
					 	2 + // Address
					 	1 + // Command
					 	2 +	// Number of bytes
					 	2;  // CRC
const uint8_t WAKE_BUFFER_TOTAL_SIZE = 2 * F_WAKE_SETTINGS::BUFF_SIZE + WAKE_HEADER;

const uint8_t WAKE_INITIAL_CRC = 0xDE;


struct FWakeHeader
{
	uint8_t addr;
	uint8_t cmd;
	uint8_t n;
};


class FWakeBase
{
public:

	FWakeBase()
	{
		txTotal 		= 0;
		txTotalFailed 	= 0;
		rxTotal		= 0;
		rxTotalFailed 	= 0;
	}

	FRetVals::value sendPacket( uint8_t TxAddr, uint8_t TxCmd = FWakeCMD::NOP, uint8_t TxNumOfBytes = 0 );
	FRetVals::value getPacket( uint8_t* RxAddr, uint8_t* RxCmd, uint8_t* RxNumOfBytes, uint8_t MatchAddr = 0 );
	FRetVals::value purge() const;
	
	FRetVals::value setData( uint8_t data, uint8_t ind );
	FRetVals::value setData( uint8_t *src, uint8_t size = F_WAKE_SETTINGS::BUFF_SIZE );

	FRetVals::value getData( uint8_t* data, uint8_t ind ) const;
	FRetVals::value getDataBuff( uint8_t* dst, uint8_t size = F_WAKE_SETTINGS::BUFF_SIZE );
	uint8_t			getData( uint8_t ind ) const;

	bool			isAddressValid( uint8_t addr ) const;

	uint8_t			getSizeOfBuffer() const;

	FWakeHeader		getLastTxHeader() const;
	FWakeHeader		getLastRxHeader() const;

	uint32_t		getTxTotal() const { return txTotal; }
	uint32_t		getTxTotalFailed() const { return txTotalFailed; }
	uint32_t		getRxTotal() const { return rxTotal; }
	uint32_t		getRxTotalFailed() const { return rxTotalFailed; }
	


private:
	uint8_t txBuff[ F_WAKE_SETTINGS::BUFF_SIZE ], rxBuff[ F_WAKE_SETTINGS::BUFF_SIZE ];
	uint8_t* pBuff;
	uint8_t crc;
	FRetVals::value result;
	uint8_t cnt;

	void staff_tx( uint8_t byte );
	void staff_rx( uint8_t* byte );



protected:
	FWakeHeader lastTx, lastRx;

	uint32_t txTotal, rxTotal, txTotalFailed, rxTotalFailed;
};




#endif /* FWAKEBASE_HPP_ */
