/*
 * FPHY.hpp
 *
 *  Created on: 28.06.2013
 *      Author: Fox
 */

#ifndef FPHY_HPP_
#define FPHY_HPP_


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>




#include <FWakeConfig.hpp>
#include <FRetVals.hpp>
#include <FWakeBase.hpp>



FRetVals::value wakePHYOpen( char* );
bool wakePHYSend( uint8_t* src, uint8_t size );
bool wakePHYGet( uint8_t* dst, uint8_t* size );
bool wakePHYPurge();
bool wakePHYClose();




#endif /* FPHY_HPP_ */
