#ifndef _TRJ_API_H
#define _TRJ_API_H


#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include "time.h"
#include "liblwgeom.h"
#include "lwgeom_pg.h"

//typedef struct location_t* location;
//typedef struct trajectory_t* trajectory;

typedef struct location{
	double x;
	double y;
	double z;
	double time;
	double accuracy;
	double bearing;
	double speed;
}location;

typedef struct trajectory{
	int32_t srid;
	double starttime;
	double endtime;
	double maxx;
	double maxy;
	double minx;
	double miny;
	uint32_t nvolume;
	uint32_t nlocation;
	uint16_t hasaccuracy;
	uint16_t hasaltitude;
	uint16_t hasbearing;
	uint16_t hasspeed;
	location **locations;
}trajectory;

typedef struct{
	uint32_t size;
	void *data;
}tserialized;

#define CHOP_LEFT(p)	while((p != NULL) && ((*p == ' ') || (*p == '\t') || (*p == '\r') || (*p == '\n') || (*p == ','))) p++;

#define TRJ_DATA(trj)		(trj->data)
#define TRJ_LOC_N_NDIM(trj,n,dim)	(*(double*)(trj->data + n * TRJ_DIMENSION(trj) * sizeof(double) + dim * sizeof(double)))
#define TRJ_LOC_N_X(trj,n)		TRJ_LOC_N_NDIM(trj,n,0)
#define TRJ_LOC_N_Y(trj,n)		TRJ_LOC_N_NDIM(trj,n,1)
#define TRJ_LOC_N_Z(trj,n)		TRJ_LOC_N_NDIM(trj,n,2)


uint32_t trj_sizeof_trajectory();

uint32_t trj_sizeof_location(trajectory *traj);

char * trj_time_to_text(double time);
double trj_text_to_time(char *text);

trajectory * trj_trajectory_from_text(char* text, uint32_t size);
location * trj_location_from_text(char* text);

uint32_t trj_trajectory_text_size(trajectory *traj);

char * trj_trajectory_to_text(trajectory *traj, uint32_t *size);
char * trj_location_to_text(location *loc);


uint32_t trj_trajectory_serialize_size(trajectory *traj);
uint8_t* trj_trajectory_serialize(trajectory * traj);

trajectory * trj_trajectory_deserialize(uint8_t* serialized);

BOX3D* trj_trajectory_extent_calculate(BOX3D *box, double x, double y, double z);

trajectory * trj_trajectory_new(int32_t srid, uint32_t nvolume, uint16_t hasaccuracy, uint16_t hasaltitude, uint16_t hasbearing, uint16_t hasspeed);

location * trj_location_new(double x, double y, double z, double time, double accuracy, double bearing, double speed);

#endif






