#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include "gps.h"

#define RANGE 20

//struct to hold points of map
struct Point{
	struct GPS * gps;
	struct Point *prev;
	struct Point *next;
};
//creates a link list of points + additional details to map trail
struct Map{
	int totalPoints;
	struct Point* head;
	struct Point* curr;
	int index;
	double totalDist;
	double distRemaining;
};

void init_map(struct Map* map_ptr, float points[][2], int numPoints);
double radDegrees(double rad);

void updateNext(struct GPS* current, struct Map* map);
double distanceNext(struct GPS* current, struct Point* pt);
void updatePrev(struct GPS* current, struct Map* map);
double distancePrev(struct GPS* current, struct Point* pt);
char* directionTo(struct GPS* current, struct GPS* to);
char* directionNext(struct GPS* current, struct Point* pt);
char* directionPrev(struct GPS* current, struct Point* pt);
double calcDistance(struct GPS* current, struct GPS* gps);

#endif
