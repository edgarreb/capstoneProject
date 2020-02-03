#include "gps.h"

/* ---------------------------------------------------------------------------------

format of GPS data:
$GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*65

parse() function returns true(1) if all data is found and false(0) otherwise.
parse() also sets/initializes coordinates variables in GPS struct

-----------------------------------------------------------------------------------*/

int parse(char* nmea, struct GPS* ptr)
{

    int32_t degree;
    long minutes;
    char degreebuff[10];

    /* look for the begining of data */
    // found GGA
    char *p = nmea;
    // get time
    p = strchr(p, ',')+1;
    float timef = atof(p);
    uint32_t time = timef;
    ptr->hour = time / 10000;
    ptr->minute = (time % 10000) / 100;
    ptr->seconds = (time % 100);

    ptr->milliseconds = fmod(timef, 1.0) * 1000;

    /* parse out latitude */
    p = strchr(p, ',')+1;
    if (',' != *p){
        strncpy(degreebuff, p, 2);
        p += 2;
        degreebuff[2] = '\0';
        degree = atol(degreebuff) * 10000000;
        strncpy(degreebuff, p, 2); // minutes
        p += 3; // skip decimal point
        strncpy(degreebuff + 2, p, 4);
        degreebuff[6] = '\0';
        minutes = 50 * atol(degreebuff) / 3;
        ptr->latitude_fixed = degree + minutes;

        ptr->latitude = degree / 100000 + minutes * 0.000006F;
        ptr->latitudeDegrees = (ptr->latitude-100*(int)(ptr->latitude/100))/60.0;
        ptr->latitudeDegrees += (int)(ptr->latitude/100);
    }

    p = strchr(p, ',')+1;
    if (',' != *p){
        if (p[0] == 'S') ptr->latitudeDegrees *= -1.0;
        if (p[0] == 'N') ptr->lat = 'N';
        else if (p[0] == 'S') ptr->lat = 'S';
        else if (p[0] == ',') ptr->lat = 0;
        else return 0;
    }

  /* parse out longitude */
  p = strchr(p, ',')+1;
  if (',' != *p){
    strncpy(degreebuff, p, 3);
    p += 3;
    degreebuff[3] = '\0';
    degree = atol(degreebuff) * 10000000;
    strncpy(degreebuff, p, 2); // minutes
    p += 3; // skip decimal point
    strncpy(degreebuff + 2, p, 4);
    degreebuff[6] = '\0';
    minutes = 50 * atol(degreebuff) / 3;
    ptr->longitude_fixed = degree + minutes;
    ptr->longitude = degree / 100000 + minutes * 0.000006F;
    ptr->longitudeDegrees = (ptr->longitude-100*(int)(ptr->longitude/100))/60.0;
    ptr->longitudeDegrees += (int)(ptr->longitude/100);
  }

  p = strchr(p, ',')+1;
  if (',' != *p){
    if (p[0] == 'W') ptr->longitudeDegrees *= -1.0;
    if (p[0] == 'W') ptr->lon = 'W';
    else if (p[0] == 'E') ptr->lon = 'E';
    else if (p[0] == ',') ptr->lon = 0;
    else return 0;
  }

  p = strchr(p, ',')+1;
  p = strchr(p, ',')+1;
  if (',' != *p){
    ptr->satellites = atoi(p);
  }

  p = strchr(p, ',')+1;
  p = strchr(p, ',')+1;
  if (',' != *p)
  {
    ptr->altitude = atof(p);
  }

  return 1;

};
