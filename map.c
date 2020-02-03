#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "map.h"
#include "lcd.h"
#define PI 3.141592654
#define EXIT_FAILURE 1
#define KM_TO_FEET 3280.84

// calculate the distance to next point
double distanceNext(struct GPS* current, struct Point* pt){
    return calcDistance(current, pt->next->gps);
}



// calculate the distance to the previous point
double distancePrev(struct GPS* current, struct Point* pt){
    return calcDistance(current, pt->prev->gps);
}



// convert angle to randian/degrees
double radDegrees(double rad){return (180/3.141593)*rad;}
double degRadians(double deg){return deg*(3.141593/180);}



// calculate longitudinal distance
double calcDistance_long(struct GPS* current, struct GPS* gps){
  int R = 6371; // Radius of the Earth
  double dLat = 0;
  double dLon = degRadians(gps->longitudeDegrees - current->longitudeDegrees);
  double a = sin(dLat/2)*sin(dLat/2) + cos(degRadians(gps->latitudeDegrees)) *
    cos(degRadians(current->latitudeDegrees)) * sin(dLon/2) *
    sin(dLon/2);
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double d = R * c * 1000; //distance in miles
  return d;
}



// calculate latitude distance
double calcDistance_lat(struct GPS* current, struct GPS* gps){
  int R = 6371; // Radius of the Earth
  double dLat = degRadians(gps->latitudeDegrees - current->latitudeDegrees);
  double dLon = 0;
  double a = sin(dLat/2)*sin(dLat/2) + cos(degRadians(gps->latitudeDegrees)) *
    cos(degRadians(current->latitudeDegrees)) * sin(dLon/2) *
    sin(dLon/2);
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double d = R * c * 1000; //distance in miles
  return d;
}



// calclate direction to the next point
char* directionTo(struct GPS* current, struct GPS* to){
    char* dir = "";
    double long_dist = calcDistance_long(current, to);
    double lat_dist = calcDistance_lat(current, to);

    if(long_dist > lat_dist){
        if(to->longitude > current->longitude){
            dir = "W";
        }else{
            dir = "E";
        }
    }else{
        if(to->latitude > current->latitude){
            dir = "N";
        }else{
            dir = "S";
        }
    }

    return dir;
}



// calculate direction to next point
char* directionNext(struct GPS* current, struct Point* pt){
    return directionTo(current, pt->next->gps);
}



// calculate direction to previous point
char* directionPrev(struct GPS* current, struct Point* pt){
    return directionTo(current, pt->prev->gps);
}



// calculate distance between points
double calcDistance(struct GPS* current, struct GPS* gps){
  int R = 6371; // Radius of the Earth
  double dLat = degRadians(gps->latitudeDegrees - current->latitudeDegrees);
  double dLon = degRadians(gps->longitudeDegrees - current->longitudeDegrees);
  double a = sin(dLat/2)*sin(dLat/2) + cos(degRadians(gps->latitudeDegrees)) *
    cos(degRadians(current->latitudeDegrees)) * sin(dLon/2) *
    sin(dLon/2);
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double d = R * c * 1000; //distance in miles
  return d;
}



// update previous location for return to start
void updatePrev(struct GPS* current, struct Map* map){
    if(!(map->curr->prev)){
      lcd_yellowon();
      return;
    }
    double dist = calcDistance(current,map->curr->next->gps);
    if(dist < 15){
        map->index = map->index - 1;
        map->curr = map->curr->prev;
        lcd_yellowon();
        return;
    }
    else if (!(/*calcDistance(current, map->curr->gps) < 50 || */dist < 50)){
        lcd_redon();
    }
    else{
      lcd_redoff();
      lcd_yellowoff();
      return;
    }
}



// update next location
void updateNext(struct GPS* current, struct Map* map){
    if(!(map->curr->next)){
      lcd_yellowon();
      return;
    }
    double dist = calcDistance(current,map->curr->next->gps);
    if(dist < 15){
        map->distRemaining = map->distRemaining - calcDistance(map->curr->gps, map->curr->prev->gps);
        map->index = map->index + 1;
        map->curr = map->curr->next;
        lcd_yellowon();
        lcd_redon();
        return;
    }
    else{
      lcd_redoff();
      lcd_yellowoff();
      return;
    }

}



// initialize map
void init_map(struct Map* map_ptr, float points[][2], int numPoints){
    map_ptr->totalPoints = numPoints;
    map_ptr->index = 1;
    map_ptr->head = NULL;
    map_ptr->curr = NULL;

    struct Point* head = (struct Point*)malloc(sizeof(struct Point));
    struct GPS * head_gps = (struct GPS*)malloc(sizeof(struct GPS));

    head_gps->latitudeDegrees  = points[0][0];
    head_gps->longitudeDegrees = points[0][1];
    head->gps = head_gps;
    head->prev = NULL;
    head->next = NULL;

    map_ptr->head = head;
    map_ptr->curr = head;
    struct Point* prev = head;
    int j;
    double totalDist = 0;
    for(j = 1; j < numPoints; j++){
        struct Point* new_point = (struct Point*)malloc(sizeof(struct Point));
        struct GPS * new_gps = (struct GPS*)malloc(sizeof(struct GPS));
        new_gps->latitudeDegrees = points[j][0];
        new_gps->longitudeDegrees = points[j][1];
        new_point->gps = new_gps;
        new_point->prev = prev;
        new_point->next = NULL;
        totalDist += calcDistance(prev->gps, new_gps);
        prev->next = new_point;
        prev = new_point;
    }
    map_ptr->totalDist = totalDist;
    map_ptr->distRemaining = totalDist;
}
