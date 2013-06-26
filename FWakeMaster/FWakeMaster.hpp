/*
 * FWakeMaster.hpp
 *
 *  Created on: 09.06.2013
 *      Author: Fox
 */

#ifndef FWAKEMASTER_HPP_
#define FWAKEMASTER_HPP_



#include <FWakeBase.hpp>



class FWakeMaster :
	public FWakeBase
{
public:
		FRetVals::value exchange( uint8_t TxAddr, uint8_t TxCmd = FWakeCMD::NOP, uint8_t TxNumOfBytes = 0,
				uint8_t* RxCmd = 0, uint8_t* RxNumOfBytes = 0 );

private:

};




#endif /* FWAKEMASTER_HPP_ */
