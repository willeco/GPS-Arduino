
#define __ASSERT_USE_STDERR

//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <TinyGPS.h>
#include <assert.h>
#include "gps.h"


const double a_wgs84 = 6378137;           // WGS-84 Earth semimajor axis (m)
const double b_wgs84 = 6356752.3142;      // WGS-84 Earth semiminor axis (m)

//-----------------------------------------------------------------------------
// Prototype
//-----------------------------------------------------------------------------

static void gps_print_float(float val, float invalid, int len, int prec);
static void gps_print_int(unsigned long val, unsigned long invalid, int len);
static void gps_print_date(TinyGPS &gps);
static void gps_print_str(const char *str, int len);


                   

//-----------------------------------------------------------------------------
// Variable
//-----------------------------------------------------------------------------
TinyGPS gps;

//-----------------------------------------------------------------------------
// Function : gps_get_position
//-----------------------------------------------------------------------------
boolean GPS_get_position(float *flat, float *flon, float *falt )
{
  unsigned long   age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  boolean        valid_position;
  
  // Get GPS Position
  gps.f_get_position(flat, flon, &age);
  *falt = gps.f_altitude();

  // Is it a valid position
  if ( *flat            == TinyGPS::GPS_INVALID_F_ANGLE ||
       *flon            == TinyGPS::GPS_INVALID_F_ANGLE ||
       gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES )
  {
    valid_position = false;
  } else {
    valid_position = true;
  }
  
  // Print position of the serial port
  gps.stats(        &chars, &sentences, &failed);  
  gps_print_int(    gps.satellites(),   TinyGPS::GPS_INVALID_SATELLITES, 5); 
  gps_print_float(  *flat,               TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  gps_print_float(  *flon,               TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  gps_print_date(   gps);
  gps_print_float(  gps.f_altitude(),   TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2);
  gps_print_int(    failed,    0xFFFFFFFF, 9);
  Serial.println();

  return(valid_position);
}
  
//-----------------------------------------------------------------------------
// smartdelay
//-----------------------------------------------------------------------------
void GPS_smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while ( Serial1.available() > 0  )
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}


//-----------------------------------------------------------------------------
// Function : Print Flotant
//-----------------------------------------------------------------------------
static void gps_print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  } else {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  GPS_smartdelay(0);
}

//-----------------------------------------------------------------------------
// Function : Print int
//-----------------------------------------------------------------------------
static void gps_print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  
  GPS_smartdelay(0);
}

//-----------------------------------------------------------------------------
// Function : print date
//-----------------------------------------------------------------------------
static void gps_print_date(TinyGPS &gps)
{
  int           year;
  byte          month, day, hour, minute, second, hundredths;
  unsigned long age;
  
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  gps_print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  GPS_smartdelay(0);
}

//-----------------------------------------------------------------------------
// Function : print string
//-----------------------------------------------------------------------------
static void gps_print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  GPS_smartdelay(0);
}

//-----------------------------------------------------------------------------
// Degrees to Radians
//-----------------------------------------------------------------------------
float gps_degrees_to_radians(float x)
{
  const float pi = 3.14159265359;
  return( x/ 180.0 * pi);
}

//-----------------------------------------------------------------------------
// Compute ECEF Coordinates
//-----------------------------------------------------------------------------
void GPS_geodetic_to_ecef(double lat, double lon, double h,
                    double *x,  double *y,  double *z)
{

    double f;           // Ellipsoid Flatness
    double e_sq;        // Square of Eccentricity
    double lambda;
    double phi;
    double s;
    double N;

    double sin_lambda;
    double cos_lambda;
    double cos_phi;
    double sin_phi;

    f           = ( a_wgs84 - b_wgs84 )/a_wgs84;           // Ellipsoid Flatness
    e_sq        = f * (2 - f);                             // Square of Eccentricity
        
    lambda      = gps_degrees_to_radians(lat);
    phi         = gps_degrees_to_radians(lon);
    s           = sin(lambda);
    N           = a_wgs84 / sqrt(1 - e_sq * s * s);

    sin_lambda  = sin(lambda);
    cos_lambda  = cos(lambda);
    cos_phi     = cos(phi);
    sin_phi     = sin(phi);

    *x          = (h + N) * cos_lambda * cos_phi;
    *y          = (h + N) * cos_lambda * sin_phi;
    *z          = (h + (1 - e_sq) * N) * sin_lambda;
}

//-----------------------------------------------------------------------------
// Converts the Earth-Centered Earth-Fixed (ECEF) coordinates (x, y, z) to 
// East-North-Up coordinates in a Local Tangent Plane that is centered at the 
// (WGS-84) Geodetic point (lat0, lon0, h0).
//-----------------------------------------------------------------------------
void GPS_ecef_to_enu
(double x, double y, double z,
               double lat0, double lon0, double h0,
               double *xEast, double *yNorth, double *zUp)
{
    double f;           // Ellipsoid Flatness
    double e_sq;        // Square of Eccentricity      
    double lambda;
    double phi;
    double s;
    double N;
    double sin_lambda;
    double cos_lambda;
    double cos_phi;
    double sin_phi;
    double xd, yd, zd;
    double x0, y0, z0;
      
    // Convert to radians in notation consistent with the paper:
    lambda      = gps_degrees_to_radians(lat0);
    phi         = gps_degrees_to_radians(lon0);

    f           = ( a_wgs84 - b_wgs84 )/a_wgs84;           // Ellipsoid Flatness
    e_sq        = f * (2 - f);                             // Square of Eccentricity
        
    s           = sin(lambda);
    N           = a_wgs84 / sqrt(1 - e_sq * s * s);

    sin_lambda  = sin(lambda);
    cos_lambda  = cos(lambda);
    cos_phi     = cos(phi);
    sin_phi     = sin(phi);
    
    x0          = (h0 + N) * cos_lambda * cos_phi;
    y0          = (h0 + N) * cos_lambda * sin_phi;
    z0          = (h0 + (1 - e_sq) * N) * sin_lambda;
    
    
    xd          = x - x0; 
    yd          = y - y0; 
    zd          = z - z0;

    // This is the matrix multiplication
    *xEast     = -sin_phi                  * xd   + cos_phi              * yd;
    *yNorth    = -cos_phi    * sin_lambda  * xd   - sin_lambda * sin_phi * yd   + cos_lambda * zd; 
    *zUp       =  cos_lambda * cos_phi     * xd   + cos_lambda * sin_phi * yd   + sin_lambda * zd;
}

//-----------------------------------------------------------------------------        
// Converts the geodetic WGS-84 coordinated (lat, lon, h) to 
// East-North-Up coordinates in a Local Tangent Plane that is centered at the 
// (WGS-84) Geodetic point (lat0, lon0, h0).
//-----------------------------------------------------------------------------
void GPS_geodetic_to_enu
(
double lat,    double lon,     double h,
double lat0,   double lon0,    double h0,
double *xEast, double *yNorth, double *zUp
)
{
    double x, y, z;
    GPS_geodetic_to_ecef(lat, lon, h, &x, &y, &z);
    GPS_ecef_to_enu(x, y, z, lat0, lon0, h0, xEast, yNorth, zUp);
}

//-----------------------------------------------------------------------------
// To check if two values are close
//-----------------------------------------------------------------------------
boolean  gps_are_close(double x0, double x1)
{
    double d = x1-x0 ;
    return (d * d) < 1e16;
}

//-----------------------------------------------------------------------------
// handle diagnostic informations given by assertion and abort program execution
//-----------------------------------------------------------------------------
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
    // transmit diagnostic informations through serial link. 
    Serial.println(__func);
    Serial.println(__file);
    Serial.println(__lineno, DEC);
    Serial.println(__sexp);
    Serial.flush();
    // abort program execution.
    abort();
}

//-----------------------------------------------------------------------------
// Just check that we get the same values as the paper for the main calculations.
//-----------------------------------------------------------------------------
void GPS_test_geo(void)
{
    double latLA = 34.00000048;
    double lonLA = -117.3335693;
    double hLA = 251.702;
    double x0, y0, z0;
    double x, y, z;
    double xEast, yNorth, zUp;
        
    GPS_geodetic_to_ecef(latLA, lonLA, hLA, &x0, &y0, &z0);

    assert(gps_are_close(-2430601.8, x0));
    assert(gps_are_close(-4702442.7, y0));
    assert(gps_are_close( 3546587.4, z0));

    // Checks to read out the matrix entries, to compare with the paper
    // First column
    x = x0 + 1;
    y = y0;
    z = z0;
    GPS_ecef_to_enu(x, y, z, latLA, lonLA, hLA, &xEast, &yNorth, &zUp);
    assert(gps_are_close(0.88834836, xEast));
    assert(gps_are_close(0.25676467, yNorth));
    assert(gps_are_close(-0.38066927, zUp));

    x = x0;
    y = y0 + 1;
    z = z0;
    GPS_ecef_to_enu(x, y, z, latLA, lonLA, hLA, &xEast, &yNorth, &zUp);
    assert(gps_are_close(-0.45917011, xEast));
    assert(gps_are_close(0.49675810, yNorth));
    assert(gps_are_close(-0.73647416, zUp));

    x = x0;
    y = y0;
    z = z0 + 1;
    GPS_ecef_to_enu(x, y, z, latLA, lonLA, hLA, &xEast, &yNorth, &zUp);
    assert(gps_are_close(0.00000000, xEast));
    assert(gps_are_close(0.82903757, yNorth));
    assert(gps_are_close(0.55919291, zUp));

}
