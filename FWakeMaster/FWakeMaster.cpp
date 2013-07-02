/*
 * main.cpp
 *
 * Description	: Wake Master Mechanism
 *
 *  Created on	: 09.06.2013
 *      Author	: Fox
 *
 *  Revisions	:
 */



#include "FWakeMaster.hpp"



FRetVals::value FWakeMaster::exchange( uint8_t TxAddr, uint8_t TxCmd, uint8_t TxNumOfBytes, uint8_t* RxCmd, uint8_t* RxNumOfBytes )
{
	FRetVals::value result;


	// Try to exchange ( maximum 'tryToExchanging' times )

	result = sendPacket( TxAddr, TxCmd, TxNumOfBytes );

	if( result != FRetVals::OK )
	{
		return result;
	}

	lastRx.addr = 0xf7;
	lastRx.cmd 	= 0xf7;
	lastRx.n	= 0xf7;

	uint8_t rxAddr, rxCmd, rxN;
	result = getPacket( &rxAddr, &rxCmd, &rxN, TxAddr );

	if( result != FRetVals::OK )
	{
		purge();
		return result;
	}

	if( rxCmd != TxCmd )
	{
		if( rxCmd == FWakeCMD::UNDEF )
			return FRetVals::WakeUndefCmd;
		purge();
		return FRetVals::WakeInvalidRxCmd;
	}

	if( RxCmd )
		*RxCmd = rxCmd;

	if( RxNumOfBytes )
		*RxNumOfBytes = rxN;

	return result;
}



