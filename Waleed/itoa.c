/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2015  Luis Claudio Gamb�a Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */
#define _XTAL_FREQ 8000000
#include <xc.h>
void itoa(unsigned int val, unsigned char* str )
{
//   __delay_ms(500);
//  str[0]=(val/10000)+0x30;  
//  str[1]=((val%10000)/1000)+0x30;  
  str[2]=((val%1000)/100);  
  str[3]=((val%100)/10);
  if (str[3] != 0)
      str[3]-=1;
//  str[4]=(val%10)+0x30;
//  str[5]=0;
  
}
