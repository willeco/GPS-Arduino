

#include <LiquidCrystal.h>

#include "gps.h"

//-----------------------------------------------------------------------------
// GPS connected to the arduino
//-----------------------------------------------------------------------------
// initialize the library with the numbers of the interface pins
// arduino Mega
// RS   = pin 8 arduino
// EN   = pin 9 arduino
// DB4 =  pin 10 arduino
// DB5 =  pin 11 arduino
// DB6 =  pin 12 arduino
// DB7 =  pin 13 arduino

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);


//-----------------------------------------------------------------------------
// Reference coordinates
//-----------------------------------------------------------------------------
const double lat_ref = 48.0;
const double lng_ref = -1.6;

         
//-----------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------
void setup() {
  
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  lcd.print("Essai GPS ");

  // Open serial port to communicate with the GPS, sets data rate to 9600 bps  
  Serial1.begin(9600);     
  Serial.begin(9600);

  GPS_test_geo();
  
  // Opens serial port to display info to the user
  Serial.begin(19200);
  //Serial.println(TinyGPS::library_version());
  Serial.println();
  Serial.println("Sats Latitude  Longitude  Date       Time     Date Alt    Course Speed Card  Checksum");
  Serial.println("     (deg)     (deg)                          Age  (m)                               ");
  Serial.println("-------------------------------------------------------------------------------------");
}

//-----------------------------------------------------------------------------
// Boucle
//-----------------------------------------------------------------------------
void loop()
{
  float   flat, flon, falt;
  double  x,y,z;
  double  xEast, yNorth, zUp;
  boolean valid_position;
  
  // --------------------------------------------------------------------------
  // Get GPS position and transform it into ENU coordinates
  // --------------------------------------------------------------------------
  valid_position = GPS_get_position(     &flat, &flon, &falt);
  if (valid_position == true)
  {
    GPS_geodetic_to_ecef(  flat,  flon,  falt, &x,  &y,  &z);  
    GPS_ecef_to_enu(x, y,z, lat_ref, lng_ref, falt, &xEast, &yNorth, &zUp);  
  
    // --------------------------------------------------------------------------    
    // Display x and y on LCD displau
    // --------------------------------------------------------------------------  
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("E: "); lcd.print(xEast);   lcd.print(" m");
    lcd.setCursor(0,1); lcd.print("N: "); lcd.print(yNorth);  lcd.print(" m");    
  } else { 
     lcd.clear();
     lcd.setCursor(0,0); lcd.print("No GPS fix ");         
  }
  // Toutes les secondes
  GPS_smartdelay(1000);
}



        
        
        
