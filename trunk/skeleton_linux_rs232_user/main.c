#define  F_CPU   16000000

#include <stdlib.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <inttypes.h>

#include "usbn2mc.h"
#include "uart.h"
#include "usbn2mc/fifo.h"
#include "../usbprog_base/firmwarelib/avrupdate.h"


void interrupt_ep_send(void);
void rs232_send(void);

volatile int tx1togl=0;     // inital value of togl bit

char toRS232Buf[100];
char toUSBBuf[100];

fifo_t toRS232FIFO;
fifo_t toUSBFIFO;

int togl3=0;
int togl1=0;

/*
struct {
  char dwDTERrate[4]; //data terminal rate, in bits per second
  char bCharFormat;   //num of stop bits (0=1, 1=1.5, 2=2)
  char bParityType;   //parity (0=none, 1=odd, 2=even, 3=mark, 4=space)
  char bDataBits;     //data bits (5,6,7,8 or 16)
} usb_cdc_line_coding;
*/

enum {
  SEND_ENCAPSULATED_COMMAND = 0,
  GET_ENCAPSULATED_RESPONSE,
  SET_COMM_FEATURE,
  GET_COMM_FEATURE,
  CLEAR_COMM_FEATURE,
  SET_LINE_CODING = 0x20,
  GET_LINE_CODING,
  SET_CONTROL_LINE_STATE,
  SEND_BREAK
};


/* Device Descriptor */

unsigned char usbrs232[] =
{ 
  0x12,       // 18 length of device descriptor
  0x01,       // descriptor type = device descriptor
  0x10,0x01,  // version of usb spec. ( e.g. 1.1)
  0x02,       // device class
  0x00,       // device subclass
  0x00,       // protocol code
  0x08,       // deep of ep0 fifo in byte (e.g. 8)
  0x81,0x17,  // vendor id
  0x64,0x0c,  // product id
  0x00,0x01,  // revision id (e.g 1.02)
  0x01,       // index of manuf. string
  0x02,       // index of product string
  0x00,       // index of ser. number
  0x01        // number of configs
};

/* Configuration descriptor */

unsigned char usbrs232Conf[] =
{ 
  0x09,       // 9 length of this descriptor
  0x02,       // descriptor type = configuration descriptor
  0x48,0x00,  // total length with first interface ...
  0x02,       // number of interfaces //bene 01
  0x01,       // number if this config. ( arg for setconfig)
  0x00,       // string index for config
  0xA0,       // attrib for this configuration ( bus powerded, remote wakup support)
  0x1A,       // power for this configuration in mA (e.g. 50mA)
    //InterfaceDescriptor
  0x09,       // 9 length of this descriptor
  0x04,       // descriptor type = interface descriptor
  0x00,       // interface number
  0x00,       // alternate setting for this interface
  0x01,       // number endpoints without 0
  0x02,       // class code
  0x02,       // sub-class code
  0x01,       // protocoll code
  0x00,       // string index for interface

  /* CDC Class-Specific descriptor */
  5,           /* sizeof(usbDescrCDC_HeaderFn): length of descriptor in bytes */
  0x24,        /* descriptor type */
  0,           /* header functional descriptor */
  0x10, 0x01,

  4,           /* sizeof(usbDescrCDC_AcmFn): length of descriptor in bytes */
  0x24,        /* descriptor type */
  2,           /* abstract control management functional descriptor */
  0x02,        /* SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE    */

  5,           /* sizeof(usbDescrCDC_UnionFn): length of descriptor in bytes */
  0x24,        /* descriptor type */
  6,           /* union functional descriptor */
  0,           /* CDC_COMM_INTF_ID */
  1,           /* CDC_DATA_INTF_ID */

  5,           /* sizeof(usbDescrCDC_CallMgtFn): length of descriptor in bytes */
  0x24,        /* descriptor type */
  1,           /* call management functional descriptor */
  3,           /* allow management on data interface, handles call management by itself */
  1,           /* CDC_DATA_INTF_ID */

  /* Endpoint Descriptor */
  7,           /* sizeof(usbDescrEndpoint) */
  5,  /* descriptor type = endpoint */
  0x83,        /* IN endpoint number 3 */
  0x03,        /* attrib: Interrupt endpoint */
  8, 0,        /* maximum packet size */
  10,         /* in ms */

  /* Interface Descriptor  */
  9,           /* sizeof(usbDescrInterface): length of descriptor in bytes */
  4,           /* descriptor type */
  1,           /* index of this interface */
  0,           /* alternate setting for this interface */
  2,           /* endpoints excl 0: number of endpoint descriptors to follow */
  0x0A,        /* Data Interface Class Codes */
  0,
  0,           /* Data Interface Class Protocol Codes */
  0,           /* string index for interface */

  /* Endpoint Descriptor */
  7,           /* sizeof(usbDescrEndpoint) */
  5,  /* descriptor type = endpoint */
  0x01,        /* OUT endpoint number 1 */
  0x02,        /* attrib: Bulk endpoint */
#if UART_CFG_HAVE_USART
  8, 0,        /* maximum packet size 8->6 */
#else
  8, 0,        /* maximum packet size */
#endif
  10,           /* in ms */

  /* Endpoint Descriptor */
  7,           /* sizeof(usbDescrEndpoint) */
  5,  /* descriptor type = endpoint */
  0x81,        /* IN endpoint number 1 */
  0x02,        /* attrib: Bulk endpoint */
  8, 0,        /* maximum packet size */
  10,         /* in ms */
};


/* uart interrupt - send complete */
ISR(USART_UDRE_vect)
{
  if(toRS232FIFO.count > 0)
  {
    UDR = fifo_get_nowait(&toRS232FIFO);
  }
  else
    UCSRB &= ~(1 << UDRIE);
}

/* uart interrupt - receive complete */
SIGNAL (SIG_UART_RECV)
{
  char recv = UDR; //Zeichen vom Register holen
  fifo_put(&toUSBFIFO, recv);
}


void RS232_to_USB(void)
{
  uint16_t i;
  uint8_t j;
  char buf[8];

  while(1)
  {
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
      i = toUSBFIFO.count; // 16 bit
    }

    if(i <= 0)
       break;
    else if(i > 8) 
      i = 8;


    for(j = 0; j < i; j++)
    {
      ATOMIC_BLOCK(ATOMIC_FORCEON)
      {
        buf[j] = fifo_get_nowait(&toUSBFIFO);
      }
    }

    USBNWrite(TXC2,FLUSH);
    for(j = 0; j < i; j++)
    {
      USBNWrite(TXD2, buf[j]);
    }
    rs232_send();
  }
}

/* interrupt signal from usb controller */

ISR(INT0_vect)
{
  USBNInterrupt();
}


/*************** usb class HID requests  **************/

// reponse for requests on interface
void USBNInterfaceRequests(DeviceRequest *req,EPInfo* ep)
{
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

// class requests
void USBNDecodeClassRequest(DeviceRequest *req,EPInfo* ep)
{
  static unsigned char serialNotification[10] = {0xa1,0x20,0,0,0,0,2,0,3,0};
  int loop;
  switch(req->bRequest)
  {
    case 0x20:  //SET_LINE_CODING:
      USBNWrite(RXC0,RX_EN);
      
      USBNRead(RXD0);
      USBNRead(RXD0);
      USBNRead(RXD0);
      USBNRead(RXD0);
      USBNRead(RXD0);
      USBNRead(RXD0);
      USBNRead(RXD0);
      
      //USBNWrite(RXC0,RX_EN);
      //USBNWrite(RXC0,FLUSH);
    
      USBNWrite(TXC0,FLUSH);
      USBNWrite(TXC0,TX_TOGL+TX_EN);
    break;
    case 0x21:  // GET_LINE_CODING:
      //UARTWrite("get line coding");
      USBNWrite(TXC0,FLUSH);

      // baud rate
      USBNWrite(TXD0,0x80);
      USBNWrite(TXD0,0x25);
      USBNWrite(TXD0,0);
      USBNWrite(TXD0,0);

      USBNWrite(TXD0,0); //stopbit
      USBNWrite(TXD0,0); //parity
      USBNWrite(TXD0,8); //databits

      interrupt_ep_send();

      USBNWrite(TXC0,TX_TOGL+TX_EN);
      
    break;
    case 0x22:  //SET_CONTROL_LINE_STATE:
      //UARTWrite("set ctrl line state");
      USBNWrite(TXC1,FLUSH);
      // fill endpoint fifo
      for(loop=0;loop<8;loop++)
        USBNWrite(TXD1,serialNotification[loop]);

      //send and control togl bit
      interrupt_ep_send();
      USBNWrite(TXC0,TX_TOGL+TX_EN);
    break;
  }
}


// togl pid for in endpoint
void interrupt_ep_send(void)
{
  if(togl3==1) {
    togl3=0;
    USBNWrite(TXC1,TX_LAST+TX_EN);
  } else {
    togl3=1;
    USBNWrite(TXC1,TX_LAST+TX_EN+TX_TOGL);
  }
}

// togl pid for in endpoint
void rs232_send(void)
{
  if(togl1==1) {
    togl1=0;
    USBNWrite(TXC2,TX_LAST+TX_EN);
  } else {
    togl1=1;
    USBNWrite(TXC2,TX_LAST+TX_EN+TX_TOGL);
  }
}

void USBtoRS232(char * buf, int length)
{
  int i;
  for(i = 0; i < length; i++)
  {
    fifo_put (&toRS232FIFO, buf[i]);
    UCSRB |= (1 << UDRIE);
  }
}

/*************** main function  **************/

int main(void)
{
  // init fifos
  fifo_init(&toRS232FIFO, toRS232Buf, 100);
  fifo_init(&toUSBFIFO,   toUSBBuf,   100);

  //baudrate stopbit parity databits
  uart_init(57600UL, 1, 0, 8);

  USBNCallbackFIFORX1(&USBtoRS232);
  // setup usbstack with your descriptors
  USBNInit(usbrs232,usbrs232Conf);

  _USBNAddStringDescriptor(""); //pseudo lang
  _USBNAddStringDescriptor("USBprog EmbeddedProjects");
  _USBNAddStringDescriptor("usbprogRS232");
  _USBNCreateStringField();

  USBNInitMC();  // start usb controller
  sei();
  USBNStart();   // start device stack

  USBNCallbackFIFORX1(&USBtoRS232);
  //USBNCallbackFIFOTX2Ready(&USBtoRS232);


  _delay_ms(200);
  PORTA |= (1 << PA4);
  _delay_ms(200);
  PORTA &= ~(1 << PA4);

  // strange
  UCSRB |= (1 << RXCIE);

  while(1)
  {
    RS232_to_USB();
    _delay_us(10);
  }
}
