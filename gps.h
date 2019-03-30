
/* File gps.h */


void GPS_test_geo(void);

void GPS_smartdelay(unsigned long ms);

void GPS_geodetic_to_ecef(double lat, double lon, double h, double *x,  double *y,  double *z);

void GPS_geodetic_to_enu(double lat,    double lon,     double h,
                   double lat0,   double lon0,    double h0,
                   double *xEast, double *yNorth, double *zUp);

void GPS_ecef_to_enu
(double x, double y, double z,
               double lat0, double lon0, double h0,
               double *xEast, double *yNorth, double *zUp);
                                  
boolean GPS_get_position(float *flat, float *flon, float *falt );
                  
