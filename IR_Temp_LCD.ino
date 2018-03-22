#define ADDR 0x5A

// RAM
#define RAWIR1 0x04
#define RAWIR2 0x05
#define TA 0x06
#define TOBJ1 0x07
#define TOBJ2 0x08
// EEPROM
#define TOMAX 0x20
#define TOMIN 0x21
#define PWMCTRL 0x22
#define TARANGE 0x23
#define EMISS 0x24
#define CONFIG 0x25
#define ADDRE 0x0E
#define ID1 0x3C
#define ID2 0x3D
#define ID3 0x3E
#define ID4 0x3F



#include <SPI.h>
#include <SoftWire.h>
#include "Ucglib.h"

SoftWire SWire(PB6, PB7, SOFT_STANDARD);
Ucglib_ILI9163_18x128x128_HWSPI ucg(/*cd=*/ PA12, /*cs=*/ PA11, /*reset=*/ PA10);

uint8_t z = 127;	// start value
float graph[128];

unsigned ReadReg( uint8_t a ) {
  
  unsigned val = 0;
  
  SWire.begin();
  SWire.beginTransmission( ADDR );
  SWire.write( a );
  SWire.endTransmission( false );

  SWire.requestFrom( ADDR, (uint8_t)3 );
  val = SWire.read();
  val |= (SWire.read() << 8);

  delay(50);
  return val;
}

void updateGraph( float temp ) {
  float tmp[128];
  memcpy( tmp, &graph[1], sizeof(graph)-sizeof(float) );
  tmp[127] = temp;
  memcpy( graph, tmp, sizeof(graph) );
}

void setup(void)
{
  delay(1000);
  memset( graph, 0, sizeof(graph) );
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  //ucg.setRotate180();
  ucg.setColor(0, 0, 0, 0);
  ucg.clearScreen();
}

void loop(void)
{
  // get a random value between 0 and 255
  float temp = (ReadReg( TOBJ1 )* 0.02f) - 273.15;
  updateGraph( temp );
  ucg_int_t y = 30;
  ucg_int_t h = 14;

  y += h;
  ucg.setFontMode(UCG_FONT_MODE_TRANSPARENT);
  ucg.setPrintPos(14,y);
  ucg.setFont(ucg_font_amstrad_cpc_8r);
  ucg.print("Temperature:");
  y += h;
  ucg.setColor( 0, 0, 0, 0 );
  ucg.drawBox( 0, y-h, 128, y);
  ucg.setFontMode(UCG_FONT_MODE_SOLID);
  ucg.setFont(ucg_font_amstrad_cpc_8r);
  ucg.setColor(0, 255, 255, 255);
  ucg.setPrintPos(40,y);
  ucg.print(temp);
  ucg.setPrintPos(86,y);
  ucg.print("C");

  ucg.setColor( 0, 0, 0, 0 );
  ucg.drawBox( 0, 80, 128, 128);
  ucg.setColor(0, 255, 255, 255);
  for( uint8_t i = 0; i < 128; ++i ) {
    ucg.drawPixel(i, 100-graph[i]);
  }

  //delay(100);  
}
