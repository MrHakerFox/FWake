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
}



class FWakeBase
{
public:
	FRetVals::value sendPckt();
	FRetVals::value getPckt();
	
	
private:
};




#endif /* FWAKEBASE_HPP_ */
