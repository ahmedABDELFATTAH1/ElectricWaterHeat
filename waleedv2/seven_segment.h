/* 
 * File:   seven_segment.h
 * Author: ahmed
 *
 * Created on June 30, 2020, 6:07 PM
 */

#define _XTAL_FREQ 8000000
#include <xc.h>

#ifndef SEVEN_SEGMENT_H
#define	SEVEN_SEGMENT_H

void lcd_writeNumber(unsigned char number);

void displayTempreture(unsigned int TEMPRETURE);

void displayDeiredTempreture(int TargetTempreture);

#endif



