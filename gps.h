#ifndef GPS_H
#define GPS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct GPS{
  //int parse(char *nmea);

  uint8_t hour;                                     ///< GMT hours
  uint8_t minute;                                   ///< GMT minutes
  uint8_t seconds;                                  ///< GMT seconds
  uint16_t milliseconds;                            ///< GMT milliseconds
  uint8_t year;                                     ///< GMT year
  uint8_t month;                                    ///< GMT month
  uint8_t day;                                      ///< GMT day

  float latitude;   ///< Floating point latitude value in degrees/minutes as received from the GPS (DDMM.MMMM)
  float longitude;  ///< Floating point longitude value in degrees/minutes as received from the GPS (DDDMM.MMMM)

  int32_t latitude_fixed;   ///< Fixed point latitude in decimal degrees
  int32_t longitude_fixed;  ///< Fixed point longitude in decimal degrees

  float latitudeDegrees;    ///< Latitude in decimal degrees
  float longitudeDegrees;   ///< Longitude in decimal degrees

  float altitude;           ///< Altitude in meters above MSL

  float HDOP;               ///< Horizontal Dilution of Precision - relative accuracy of horizontal position
  char lat;                 ///< N/S
  char lon;                 ///< E/W
  char mag;                 ///< Magnetic variation direction
  int fix;                  ///< Have a fix?
  uint8_t fixquality;       ///< Fix quality (0, 1, 2 = Invalid, GPS, DGPS)
  uint8_t satellites;       ///< Number of satellites in use

};

int parse(char* nmea, struct GPS* ptr);

#endif
