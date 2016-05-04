#include <stdlib.h>
#include "postgres.h"

#include "math.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>


#include "trajectory.h"
#include "liblwgeom.h"
#include "lwgeom_pg.h"





uint32_t trj_sizeof_location(trajectory *traj){
	uint32_t size = 0;
	size += traj->nvolume * (sizeof(double) * 7);
	return size;
}

uint32_t trj_sizeof_trajectory(){
	uint32_t size = 0;
	size += sizeof(int32_t) + sizeof(double) * 6 + sizeof(uint32_t) * 2 + sizeof(uint16_t) * 4 + sizeof(location);
	return size;
}


char * trj_time_to_text(double time){
	struct tm *tptr = NULL;
	char * result = NULL;
	char *p = NULL;
	uint32_t size;
	time_t t = (time_t)time;
	
	int count;

	size = strlen("xxxx-xx-xx xx:xx:xx");
	tptr = gmtime(&t);
	result = (char*)palloc(size);

	p = result;
	
	count = sprintf(p, "%d-", tptr->tm_year);
	p += count;

	if(tptr->tm_mon < 10){
		count = sprintf(p, "0%d-", tptr->tm_mon);
	}else{
		count = sprintf(p, "%d-", tptr->tm_mon);
	}
	p += count;
//elog(NOTICE, "tm_mday:%d",tptr->tm_mday);
	if(tptr->tm_mday < 10){
		count = sprintf(p, "0%d ", tptr->tm_mday);
	}else{
		count = sprintf(p, "%d ", tptr->tm_mday);
	}
	p += count;

	if(tptr->tm_hour < 10){
		count = sprintf(p, "0%d:", tptr->tm_hour);
	}else{
		count = sprintf(p, "%d:", tptr->tm_hour);
	}
	p += count;

	if(tptr->tm_min < 10){
		count = sprintf(p, "0%d:", tptr->tm_min);
	}else{
		count = sprintf(p, "%d:", tptr->tm_min);
	}
	p += count;

	if(tptr->tm_sec < 10){
		count = sprintf(p, "0%d", tptr->tm_sec);
	}else{
		count = sprintf(p, "%d", tptr->tm_sec);
	}
	p += count;
	
	
	return result;
	
}

double trj_text_to_time(char *text){
	struct tm tptr;
	double result = 0;
	time_t time;
	char *p = NULL;
	
	
	p = text;
	
	tptr.tm_year = strtol(p, &p, 10);

	if(tptr.tm_year < 1970 || tptr.tm_year > 2014){
		elog(ERROR, "Time input Format ERROR!");
	}
	p ++;
	
	if(strncmp(p, "0", 1) == 0){
		p++;
		tptr.tm_mon = strtol(p, &p, 10);
	}else{
		tptr.tm_mon = strtol(p, &p, 10);
	}

	p++;
	
	if(strncmp(p, "0", 1) == 0){
		p++;
		tptr.tm_mday = strtol(p, &p, 10);
	}else{
		tptr.tm_mday = strtol(p, &p, 10);
	}
	p++;

	if(strncmp(p, "0", 1) == 0){
		p++;
		tptr.tm_hour = strtol(p, &p, 10);
	}else{
		tptr.tm_hour = strtol(p, &p, 10);
	}
	p++;

	if(strncmp(p, "0", 1) == 0){
		p++;
		tptr.tm_min = strtol(p, &p, 10);
	}else{
		tptr.tm_min = strtol(p, &p, 10);
	}
	p++;

	if(strncmp(p, "0", 1) == 0){
		p++;
		tptr.tm_sec = strtol(p, &p, 10);
	}else{
		tptr.tm_sec = strtol(p, &p, 10);
	}
	p++;

	time = mktime(&tptr);
	result = (double)time;
	
	return result;
}


trajectory * trj_trajectory_from_text(char *text, uint32_t size){
	trajectory *traj = NULL;
	
	char *p = NULL;
	//uint32_t nlocation = 0;
	//double start; 
	//double end = 0;
	BOX3D *box = (BOX3D *)palloc(sizeof(BOX3D));
	uint32_t i;
	
	
	p = text;

	traj = (trajectory*)palloc(trj_sizeof_trajectory());
	memset(traj, 0, trj_sizeof_trajectory());
	CHOP_LEFT(p);
	if(strncmp(p, "SRID", strlen("SRID")) != 0){
		elog(ERROR, "Input Format ERROR!");
	}
	p += strlen("SRID");
	CHOP_LEFT(p);
	p++;
	CHOP_LEFT(p);
	traj->srid = strtol(p, &p, 10);

	CHOP_LEFT(p);
	if(strncmp(p, "NVOLUME", strlen("NVOLUME")) != 0){
		elog(ERROR, "Input Format ERROR!");
	}
	p += strlen("NVOLUME");
	CHOP_LEFT(p);
	p++;
	CHOP_LEFT(p);
	traj->nvolume = strtol(p, &p, 10);

	CHOP_LEFT(p);
	if(strncmp(p, "NLOCATION", strlen("NLOCATION")) != 0){
		elog(ERROR, "Input Format ERROR!");
	}
	p += strlen("NLOCATION");
	CHOP_LEFT(p);
	p++;
	CHOP_LEFT(p);
	traj->nlocation = strtol(p, &p, 10);
	
	CHOP_LEFT(p);
	if(strncmp(p, "HASACCURACY", strlen("HASACCURACY")) != 0){
		elog(ERROR, "Input Format ERROR!");
	}
	p += strlen("HASACCURACY");
	CHOP_LEFT(p);
	p++;
	CHOP_LEFT(p);
	traj->hasaccuracy = strtol(p, &p, 10);

	CHOP_LEFT(p);
	if(strncmp(p, "HASALTITUDE", strlen("HASALTITUDE")) != 0){
		elog(ERROR, "Input Format ERROR!");
	}
	p += strlen("HASALTITUDE");
	CHOP_LEFT(p);
	p++;
	CHOP_LEFT(p);
	traj->hasaltitude = strtol(p, &p, 10);

	CHOP_LEFT(p);
	if(strncmp(p, "HASBEARING", strlen("HASBEARING")) != 0){
		elog(ERROR, "Input Format ERROR!");
	}
	p += strlen("HASBEARING");
	CHOP_LEFT(p);
	p++;
	CHOP_LEFT(p);
	traj->hasbearing = strtol(p, &p, 10);

	CHOP_LEFT(p);
	if(strncmp(p, "HASSPEED", strlen("HASSPEED")) != 0){
		elog(ERROR, "Input Format ERROR!");
	}
	p += strlen("HASSPEED");
	CHOP_LEFT(p);
	p++;
	CHOP_LEFT(p);
	traj->hasspeed = strtol(p, &p, 10);

	CHOP_LEFT(p)
	if(strncmp(p, "LOCATIONS", strlen("LOCATIONS")) != 0){
		elog(ERROR, "Input Format ERROR!");
	}
	p += strlen("LOCATIONS");
	CHOP_LEFT(p);
	p++;
	CHOP_LEFT(p);

	
	if( traj->nlocation == 0){
		traj->locations = NULL;
		return traj;
	}

	traj->locations = (location **)palloc((sizeof(struct location))* traj->nlocation);

	if(!traj->locations){
		elog(ERROR,"Out of memory allocating locations for trajectory!");
	}

	if(traj->nlocation > 0){
		for(i = 0; i<traj->nlocation; i++){
		
			location *loc = NULL;
			loc = (location*)palloc(sizeof(location));

			loc->x = strtod(p, &p);
			CHOP_LEFT(p);
		
			loc->y = strtod(p, &p);
			CHOP_LEFT(p);	
		
			loc->z = strtod(p, &p);
			CHOP_LEFT(p);
	
			loc->time = trj_text_to_time(p);
			p += 19;
			CHOP_LEFT(p);

			loc->accuracy = strtod(p, &p);
			CHOP_LEFT(p);

			loc->bearing = strtod(p, &p);
			CHOP_LEFT(p);

			loc->speed = strtod(p, &p);
			CHOP_LEFT(p);


			if(i == 0){
				traj->starttime = loc->time;
				box->xmin = loc->x;
				box->ymin = loc->y;
				box->zmin = 0.0;
				box->xmax = loc->x;
				box->ymax = loc->y;
				box->zmax = 0.0;
			}
			else{
				box = trj_trajectory_extent_calculate(box, loc->x, loc->y, 0.0);
			}

			if(i == traj->nlocation-1){
				traj->endtime = loc->time;
			}
		
			traj->locations[i] = loc;
		}

		traj->maxx = box->xmax;
		traj->maxy = box->ymax;
		traj->minx = box->xmin;
		traj->miny = box->ymin;

	}
	return traj;
}

/*
location * trj_location_from_text(char* text){
	char *p = NULL;
	location *loc = NULL;
	
	loc = (location*)palloc(sizeof(location));
	p = text;

	loc->x = strtod(p, &p);
	CHOP_LEFT(p);
	

	loc->y = strtod(p, &p);
	CHOP_LEFT(p);	
	
	loc->z = strtod(p, &p);
	CHOP_LEFT(p);

	loc->time = strtod(p, &p);
	CHOP_LEFT(p);

	loc->accuracy = strtod(p, &p);
	CHOP_LEFT(p);

	loc->bearing = strtod(p, &p);
	CHOP_LEFT(p);

	loc->speed = strtod(p, &p);
	CHOP_LEFT(p);
	
	return loc;
}
*/


uint32_t trj_trajectory_text_size(trajectory *traj){
	uint32_t size = 0;
	size = strlen("SRID:") + 1 + strlen("STARTTIME:") + 1 + strlen("ENDTIME:") + 1 + strlen("NLOCATION:") + 1 + strlen("MAXX:") + 1 + strlen("MAXY:") + 1 + strlen("MINX:") + 1 + strlen("MINY:") + 1 + strlen("HASACCURACY:") + 1 + strlen("HASALTITUDE:") + 1 + strlen("HASBEARING:") + 1  + strlen("HASSPEED:") + 1 + strlen("LOCATIONS:");
	size += sizeof(int32_t) + sizeof(double) * 6 + sizeof(uint32_t) * 2 + sizeof(uint16_t) * 4 + traj->nlocation * (sizeof(double) * 6 + 6 * 4 + 20);
	return size;
}



char * trj_trajectory_to_text(trajectory *traj, uint32_t *size){
	char *p = NULL;
	char *text = NULL;
	int count = 0;
	uint32_t i;
	//char *tmp = NULL;
	//uint32_t size = 0;

	*size = trj_trajectory_text_size(traj);
	text = (char *)palloc(*size);
	memset(text, 0, *size);
	p = text;
	
	count = sprintf(p, "SRID:%lu\t", traj->srid);
	p += count;

	count = sprintf(p, "NLOCATION:%lu\t", traj->nlocation);
	p += count;

	
	count = sprintf(p, "STARTTIME:%s\t", trj_time_to_text(traj->starttime));
	p += count;

	count = sprintf(p, "ENDTIME:%s\n", trj_time_to_text(traj->endtime));
	p += count;

	count = sprintf(p, "MAXX:%f\t", traj->maxx);
	p += count;

	count = sprintf(p, "MAXY:%f\t", traj->maxy);
	p += count;

	count = sprintf(p, "MINX:%f\t", traj->minx);
	p += count;

	count = sprintf(p, "MINY:%f\n", traj->miny);
	p += count;

	count = sprintf(p, "HASACCURACY:%d\t", traj->hasaccuracy);
	p += count;

	count = sprintf(p, "HASALTITUDE:%d\t", traj->hasaltitude);
	p += count;

	count = sprintf(p, "HASBEARING:%d\t", traj->hasbearing);
	p += count;

	count = sprintf(p, "HASSPEED:%d\t", traj->hasspeed);
	p += count;

	count = sprintf(p, "LOCATIONS:");
	p += count;
	if(traj->nlocation > 0){
		for (i = 0; i < traj->nlocation; i++){
			location *loc = traj->locations[i];
			count = sprintf(p, "%f\t", loc->x);
			p += count;
			count = sprintf(p, "%f\t", loc->y);
			p += count;
			count = sprintf(p, "%f\t", loc->z);
			p += count;
			count = sprintf(p, "%s\t", trj_time_to_text(loc->time));
			p += count;
			count = sprintf(p, "%f\t", loc->accuracy);
			p += count;
			count = sprintf(p, "%f\t", loc->bearing);
			p += count;
			count = sprintf(p, "%f,\t", loc->speed);
			p += count;

		}
	}
	return text;
}

/*
char * trj_location_to_text(location *loc){
	char *text = NULL;
	char *p = NULL;
	uint32_t count = 0;
	
	text = (char *)palloc(sizeof(location)+7*5);
	//memset(text, 0, (sizeof(location)+7));
	p = text;

	count = sprintf(p, "%g\t", loc->x);
	p += count;
	count = sprintf(p, "%g\t", loc->y);
	p += count;
	count = sprintf(p, "%g\t", loc->z);
	p += count;
	count = sprintf(p, "%g\t", loc->time);
	p += count;
	count = sprintf(p, "%g\t", loc->accuracy);
	p += count;
	count = sprintf(p, "%g\t", loc->bearing);
	p += count;
	count = sprintf(p, "%g\t", loc->speed);
	p += count;
	
	return text;
}
*/


uint32_t trj_trajectory_serialize_size(trajectory *traj){
	uint32_t size = 0;
	size += sizeof(int32_t) + sizeof(double) * 6 + sizeof(uint32_t) * 2 + sizeof(uint16_t) * 4 + sizeof(location ) + traj->nvolume * (sizeof(double) * 7);
	return size;
}





uint8_t* trj_trajectory_serialize(trajectory * traj){
	uint32_t size = 0;
	uint8_t* ret = NULL;
	uint8_t* ptr = NULL;
	uint16_t i = 0;
	double x, y, z, a, b, s;
	double time = 0;

	size = trj_trajectory_serialize_size(traj);
	
	ret = (uint8_t*) malloc(size);
	memset(ret, 0, size);
	ptr = ret;

	memcpy(ptr, traj, trj_sizeof_trajectory());

elog(NOTICE, "ssrid:%d", trj_sizeof_trajectory());

	if(traj->nlocation  > 0){
		ptr += trj_sizeof_trajectory();
		for(i = 0; i < traj->nlocation; i++){
			location *loc = traj->locations[i];
			x = loc->x;
			memcpy(ptr, &x, 8);

			ptr += 8;
		
			y = loc->y;
			memcpy(ptr, &y, 8);

			ptr += 8;

			z = loc->z;
			memcpy(ptr, &z, 8);
			ptr += 8;

			time = loc->time;
			memcpy(ptr, &time, 8);
elog(NOTICE, "sssssssrid%f", (*((double *)ptr)));
			ptr += 8;

			a = loc->accuracy;
			memcpy(ptr, &a, 8);
			ptr += 8;

			b = loc->bearing;
			memcpy(ptr, &b, 8);
			ptr += 8;

			s = loc->speed;
			memcpy(ptr, &s, 8);
			ptr += 8;
			

		}
	
	}


	return ret;
}



trajectory * trj_trajectory_deserialize(uint8_t* serialized){
	trajectory *traj = NULL;
	const uint8_t *ptr = NULL;
	uint32_t i;
	const uint8_t *head = (const uint8_t *)serialized;
	traj = (trajectory *)palloc(trj_sizeof_trajectory());
	memcpy(traj, serialized+VARHDRSZ, trj_sizeof_trajectory());

	traj->locations = (location**)palloc(traj->nlocation * sizeof(struct location));
	ptr = head;
	
	ptr += VARHDRSZ + trj_sizeof_trajectory();
	if(traj->nlocation > 0){
		for(i = 0; i < traj->nlocation; i++){
			location *loc = (location *)palloc(sizeof(struct location));
			//loc->x = (*((double *)ptr));
			//ptr += sizeof(double);
			
			 
			memcpy(loc, ptr, sizeof(struct location));
			traj->locations[i] = loc;
			ptr += sizeof(struct location);
		
		}
	}
	return traj;
}


trajectory * trj_trajectory_new(int32_t srid, uint32_t nvolume, uint16_t hasaccuracy, uint16_t hasaltitude, uint16_t hasbearing, uint16_t hasspeed){
	trajectory *traj = NULL;

	traj = (trajectory *)palloc(trj_sizeof_trajectory());
	memset(traj, 0, trj_sizeof_trajectory());	
	traj->srid = srid;
	traj->nvolume = nvolume;
	traj->hasaccuracy = hasaccuracy;
	traj->hasaltitude = hasaltitude;
	traj->hasbearing = hasbearing;
	traj->hasspeed = hasspeed;
	traj->nlocation = 0;
	traj->starttime = 0;
	traj->endtime = 0;
	traj->maxx = 0;
	traj->maxy = 0;
	traj->minx = 0;
	traj->miny = 0;
	traj->locations = NULL;
	
	return traj;
}


location * trj_location_new(double x, double y, double z, double time, double accuracy, double bearing, double speed){
	location *loc = NULL;
	loc = (location *)palloc(sizeof(struct location));
	
	loc->x = x;
	loc->y = y;
	loc->z = z;
	loc->time = time;
	loc->accuracy = accuracy;
	loc->bearing = bearing;
	loc->speed = speed;

	return loc;
}



BOX3D* trj_trajectory_extent_calculate(BOX3D *box, double x, double y, double z){
	
	if(box->xmin > x){
		box->xmin = x;
	}
	if(box->ymin > y){
		box->ymin = y;
	}
	if(box->xmax < x){
		box->xmax = x;
	}
	if(box->ymax < y){
		box->ymax = y;
	}
	return box;
}





