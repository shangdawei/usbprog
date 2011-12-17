/*
 * usbprog - JTAG library
 * Copyright (C) 2006 Benedikt Sauter 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#define UNKOWN_COMMAND	0x00
#define PORT_DIRECTION	0x01
#define PORT_SET	0x02
#define PORT_GET	0x03
#define PORT_SETPIN	0x04
#define PORT_GETBIT	0x05

#define WRITE_TDI	0x06
#define READ_TDO	0x07
#define WRITE_AND_READ	0x08
#define WRITE_TMS	0x09


#define INIT_JTAG	0x0A
#define TAP_RESET	0x0B
#define TAP_SHIFT	0x0C
#define TAP_CAPTURE_DR	0x0D
#define TAP_CAPTURE_IR	0x0E
#define TAP_SHIFT_FINAL	0x0F


#define F_CPU 16000000
#include <util/delay.h>

#include "wait.h"

#include "../../usbprog_base/firmwarelib/avrupdate.h"
#include "usbn2mc.h"

#include "usbprogjtag.h"

SIGNAL(SIG_UART_RECV)
{
 //Terminal(UARTGetChar());
 //UARTWrite("usbn>");
}


char answer[64];
volatile struct usbprog_t 
{
  volatile int datatogl;
}usbprog;


SIGNAL(SIG_INTERRUPT0)
{
  USBNInterrupt();
}

/* id need for live update of firmware */

void USBNDecodeVendorRequest(DeviceRequest *req)
{
  switch(req->bRequest)
  {
    case STARTAVRUPDATE:
      avrupdate_start();
    break;
  }
}

void CommandAnswer(int length)
{
  int i;

  USBNWrite(TXC1, FLUSH);
  for(i = 0; i < length; i++)
    USBNWrite(TXD1, answer[i]);


  /* control togl bit */
  if(usbprog.datatogl == 1) {
    USBNWrite(TXC1, TX_LAST+TX_EN+TX_TOGL);
    usbprog.datatogl = 0;
  } else {
    USBNWrite(TXC1, TX_LAST+TX_EN);
    usbprog.datatogl = 1;
  }
}

/* central command parser */
void Commands(char *buf)
{
  //usbprog.datatogl = 1;   // 1MHz
  int i;
  switch(buf[0]) {
    case PORT_DIRECTION:
      set_direction((uint8_t)buf[1]);
      //answer[0] = PORT_DIRECTION; 
      //answer[1] = 0x00;
      //CommandAnswer(2);
    break;
    case PORT_SET:
      set_port((uint8_t)buf[1]);
      //answer[0] = PORT_SET; 
      //answer[1] = 0x00;
      //CommandAnswer(2);
    break;
    case PORT_GET:
      answer[0] = PORT_GET; 
      answer[1] = get_port();
      CommandAnswer(2);
    break;
    case PORT_SETPIN:
      set_bit((uint8_t)buf[1],(uint8_t)buf[2]);
      //answer[0] = PORT_SETPIN; 
      //answer[1] = 0x00;
      //CommandAnswer(2);
    break;
    case PORT_GETBIT:
      answer[0] = PORT_GETBIT; 
      answer[1] = (char)get_bit((uint8_t)buf[1]);
      CommandAnswer(2);
    break;
    
    case WRITE_TDI:
      write_tdi(buf,((uint8_t)buf[1]*256)+(uint8_t)buf[2]);	// size = numbers of byte not bits!!! round up
      #if 1
      // tck 0 tms 0 tdi 0
      CLEARPIN(PIN_WRITE,TCK);  // clk
      CLEARPIN(PIN_WRITE,TDI);  // tdi
      CLEARPIN(PIN_WRITE,TMS);  // tms
      
      // tck 1 tms 0 tdi 0
      SETPIN(PIN_WRITE,TCK);  // clk
      #endif
      //answer[0] = WRITE_TDI; 
      //answer[1] = 0x00;
      //CommandAnswer(2);
    break;
 
    case WRITE_TMS:
      write_tms((uint8_t)buf[1]);
      //answer[0] = WRITE_TDI; 
      //answer[1] = 0x00;
      //CommandAnswer(2);
    break;

    case READ_TDO:
      read_tdo(buf,((uint8_t)buf[1]*256)+(uint8_t)buf[2]);	// size = numbers of byte not bits!!! round up
      #if 1
      // tck 0 tms 0 tdi 0
      CLEARPIN(PIN_WRITE,TCK);  // clk
      CLEARPIN(PIN_WRITE,TDI);  // tdi
      CLEARPIN(PIN_WRITE,TMS);  // tms
      
      // tck 1 tms 0 tdi 0
      SETPIN(PIN_WRITE,TCK);  // clk
      #endif
      for(i=0;i<64;i++)
	answer[i]=buf[i];
      CommandAnswer(64);
    break;

    case WRITE_AND_READ:
      write_and_read(buf,((uint8_t)buf[1]*256)+(uint8_t)buf[2]);	// size = numbers of byte not bits!!! round up
      
      #if 0
      // tck 0 tms 0 tdi 0
      CLEARPIN(PIN_WRITE,TCK);  // clk
      CLEARPIN(PIN_WRITE,TDI);  // tdi
      CLEARPIN(PIN_WRITE,TMS);  // tms
      
      // tck 1 tms 0 tdi 0
      SETPIN(PIN_WRITE,TCK);  // clk
      #endif

      for(i=0;i<64;i++)
	answer[i]=buf[i];
      CommandAnswer(64);
    break;
    
    case INIT_JTAG:
      set_direction(0xf4);
    break;

    case TAP_RESET:
    break;

    case TAP_SHIFT:
      tap_shift(buf,(uint8_t)buf[1]);
      for(i=0;i<64;i++)
	answer[i]=buf[i];
      CommandAnswer(64);
    break;
 
    case TAP_SHIFT_FINAL:
      tap_shift_final(buf,(uint8_t)buf[1]);
      for(i=0;i<64;i++)
	answer[i]=buf[i];
      CommandAnswer(64);
    break;
    
    case TAP_CAPTURE_DR:
    break;

    case TAP_CAPTURE_IR:
    break;

    default:
      // unkown command
      answer[0] = UNKOWN_COMMAND; 
      answer[1] = 0x00; 
      CommandAnswer(2);
  }

}


int main(void)
{
    int conf, interf;

    USBNInit();

    usbprog.datatogl = 0;   // 1MHz

    //DDRA = (1 << PA4);

    USBNDeviceVendorID(0x1781);	//atmel ids
    USBNDeviceProductID(0x0c62); // atmel ids

    USBNDeviceBCDDevice(0x0200);

    char lang[]={0x09,0x04};
    _USBNAddStringDescriptor(lang); // language descriptor

    USBNDeviceManufacture("B.Sauter");
    USBNDeviceProduct("USB JTAG Interface");
    USBNDeviceSerialNumber("GNU/GPL2");

    conf = USBNAddConfiguration();

    USBNConfigurationPower(conf,50);

    interf = USBNAddInterface(conf,0);
    USBNAlternateSetting(conf,interf,0);

    USBNAddInEndpoint(conf,interf,1,0x02,BULK,64,0,NULL);
    USBNAddOutEndpoint(conf,interf,1,0x03,BULK,64,0,&Commands);

    USBNInitMC();
    sei();

    // start usb chip
    USBNStart();
	


    while(1);
}


