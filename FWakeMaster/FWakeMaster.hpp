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
		FWakeMaster();

		FRetVals::value exchange();

private:
};




#endif /* FWAKEMASTER_HPP_ */
