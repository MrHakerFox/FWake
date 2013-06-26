//============================================================================
// Name        : FPHY.hpp
// Author      : Yakimov Y.A.
// Version     :
// Copyright   : Your copyright notice
// Description : PHY for Win32
// Created on  : 14.08.2011
// Modified on : 19.06.2011
//============================================================================



#ifndef FPHY_HPP
#define FPHY_HPP



#include <windows.h>
#include <inttypes.h>
#include <stdio.h>



#include <FWakeConfig.hpp>
#include <FRetVals.hpp>
#include <FWakeBase.hpp>



FRetVals::value wakePHYOpen( char* );
bool wakePHYSend( uint8_t* src, uint8_t size );
bool wakePHYGet( uint8_t* dst, uint8_t* size );
bool wakePHYPurge();
bool wakePHYClose();

#endif
