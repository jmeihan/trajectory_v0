#include <stdlib.h>
#include "postgres.h"

#include "math.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <fmgr.h>
#include <string.h>

#include "trajectory.h"
#include "liblwgeom.h"
#include "lwgeom_pg.h"
#include "funcapi.h"
#include "utils/geo_decls.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(trj_in);
Datum trj_in(PG_FUNCTION_ARGS)
{
	trajectory *traj = NULL;
	uint8_t *result = NULL;
	uint8_t *buf = NULL;
	char *text = PG_GETARG_CSTRING(0);
	uint32_t len = 0;
	
	traj = trj_trajectory_from_text(text, strlen(text));
	if(traj == NULL)
		PG_RETURN_NULL();
	
	buf  = trj_trajectory_serialize(traj);

	len = trj_trajectory_serialize_size(traj) + VARHDRSZ;
pfree(traj);
	result = (uint8_t*)palloc(len);
elog(NOTICE, "len:%d", len);
	//memset(result, 0, len);
	SET_VARSIZE(result, len );
	memcpy((result + VARHDRSZ), buf, len);

	free(buf);

	if (result == NULL)
		PG_RETURN_NULL();

	PG_RETURN_POINTER(result);
}




PG_FUNCTION_INFO_V1(trj_out);
Datum trj_out(PG_FUNCTION_ARGS)
{
	char *result = NULL;
	uint8_t *serialized_traj = NULL;
	uint32_t size = 0;

	trajectory *traj = NULL;
	
	if(PG_ARGISNULL(0))
		PG_RETURN_NULL();
	serialized_traj = (uint8_t *)PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	
	traj = trj_trajectory_deserialize(serialized_traj);
	if(traj == NULL)
		PG_RETURN_NULL();
	result = trj_trajectory_to_text(traj, &size);
	pfree(traj);
	PG_RETURN_CSTRING(result);
}


PG_FUNCTION_INFO_V1(trj_trajectory_makeEmpty);
Datum trj_trajectory_makeEmpty(PG_FUNCTION_ARGS){
	trajectory *traj = NULL;
	uint8_t *result = NULL;
	uint8_t *buf = NULL;
	uint32_t len = 0;
	int32_t srid = -1;
	uint32_t nvolume = 0;
	uint16_t hasaccuracy = 0, hasaltitude = 0, hasbearing = 0, hasspeed = 0;
	
	if(PG_NARGS() < 6){
		elog(ERROR, "trj_trajectory_makeEmpty requires 6 arguments");
		PG_RETURN_NULL();
	}

	if(!PG_ARGISNULL(0))
		srid = PG_GETARG_INT32(0);

	if(!PG_ARGISNULL(1))
		nvolume = PG_GETARG_UINT32(1);
	if(!PG_ARGISNULL(2))
		hasaccuracy = PG_GETARG_UINT16(2);
	if(!PG_ARGISNULL(3))
		hasaltitude = PG_GETARG_UINT16(3);
	if(!PG_ARGISNULL(4))
		hasbearing = PG_GETARG_UINT16(4);
	if(!PG_ARGISNULL(5))
		hasspeed = PG_GETARG_UINT16(5);


	traj = trj_trajectory_new(srid, nvolume, hasaccuracy, hasaltitude, hasbearing, hasspeed);

	buf  = trj_trajectory_serialize(traj);

	pfree(traj);

	len = trj_trajectory_serialize_size(traj) + VARHDRSZ;

	result = (uint8_t*)palloc(len);
	memset(result, 0, len);
	SET_VARSIZE(result, len);
	memcpy(result + VARHDRSZ, buf, len);
	free(buf);

elog(NOTICE, "srid:%d", traj->srid );

	if (result == NULL)
		PG_RETURN_NULL();

	PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(trj_location_makeEmpty);
Datum trj_location_makeEmpty(PG_FUNCTION_ARGS){
	uint8_t *ret = NULL;
	PG_RETURN_POINTER(ret);
}


PG_FUNCTION_INFO_V1(trj_get_srid);
Datum trj_get_srid(PG_FUNCTION_ARGS)
{
	//char *result = NULL;
	int32_t result = 0;
	uint8_t *serialized_traj = NULL;
	trajectory *traj = NULL;
	serialized_traj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	traj = trj_trajectory_deserialize(serialized_traj);
	
	result = traj->srid;
	PG_RETURN_INT32(result);
}


PG_FUNCTION_INFO_V1(trj_get_nvolume);
Datum trj_get_nvolume(PG_FUNCTION_ARGS){
	uint32_t result = 0;
	uint8_t *serialized_traj = NULL;
	trajectory *traj = NULL;
	serialized_traj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	traj = trj_trajectory_deserialize(serialized_traj);
	
	result = traj->nvolume;
	PG_RETURN_INT32(result);
}


PG_FUNCTION_INFO_V1(trj_get_nlocation);
Datum trj_get_nlocation(PG_FUNCTION_ARGS){
	uint32_t result = 0;
	uint8_t *serialized_trj = NULL;
	trajectory *trj = NULL;
	serialized_trj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	trj = trj_trajectory_deserialize(serialized_trj);
	
	result = trj->nlocation;
	PG_RETURN_INT32(result);
}


PG_FUNCTION_INFO_V1(trj_get_extent);
Datum trj_get_extent(PG_FUNCTION_ARGS){
	BOX3D *result = NULL;
	uint8_t *serialized_trj = NULL;
	trajectory *trj = NULL;
	serialized_trj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	trj = trj_trajectory_deserialize(serialized_trj);
	
	result = (BOX3D *)palloc(sizeof(BOX3D));
	result->xmin = trj->minx;
	result->ymin = trj->miny;
	result->xmax = trj->maxx;
	result->ymax = trj->maxy;
	result->zmin = 0.0;
	result->zmax = 0.0;

	PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(trj_get_starttime);
Datum trj_get_starttime(PG_FUNCTION_ARGS){
	char *result = NULL;
	uint8_t *serialized_trj = NULL;
	trajectory *trj = NULL;
	serialized_trj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	trj = trj_trajectory_deserialize(serialized_trj);

	result = trj_time_to_text(trj->starttime);
	PG_RETURN_CSTRING(result);
}


PG_FUNCTION_INFO_V1(trj_get_endtime);
Datum trj_get_endtime(PG_FUNCTION_ARGS){
	char *result = NULL;
	uint8_t *serialized_trj = NULL;
	trajectory *trj = NULL;
	serialized_trj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	trj = trj_trajectory_deserialize(serialized_trj);

	result = trj_time_to_text(trj->endtime);
	PG_RETURN_CSTRING(result);
}


PG_FUNCTION_INFO_V1(trj_get_hasaccuracy);
Datum trj_get_hasaccuracy(PG_FUNCTION_ARGS){
	uint16_t result = 0;
	uint8_t *serialized_trj = NULL;

	trajectory *trj = NULL;
	serialized_trj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	trj = trj_trajectory_deserialize(serialized_trj);
	
	
	result = trj->hasaccuracy;
	PG_RETURN_INT32(result);
}

PG_FUNCTION_INFO_V1(trj_get_hasaltitude);
Datum trj_get_hasaltitude(PG_FUNCTION_ARGS){
	uint16_t result = 0;
	uint8_t *serialized_trj = NULL;

	trajectory *trj = NULL;
	serialized_trj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	trj = trj_trajectory_deserialize(serialized_trj);
	
	
	result = trj->hasaltitude;
	PG_RETURN_INT32(result);
}

PG_FUNCTION_INFO_V1(trj_get_hasbearing);
Datum trj_get_hasbearing(PG_FUNCTION_ARGS){
	uint16_t result = 0;
	uint8_t *serialized_trj = NULL;

	trajectory *trj = NULL;
	serialized_trj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	trj = trj_trajectory_deserialize(serialized_trj);
	
	
	result = trj->hasbearing;
	PG_RETURN_INT32(result);
}

PG_FUNCTION_INFO_V1(trj_get_hasspeed);
Datum trj_get_hasspeed(PG_FUNCTION_ARGS)
{
	//char *result = NULL;
	uint16_t result = 0;
	uint8_t *serialized_trj = NULL;

	trajectory *trj = NULL;
	serialized_trj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	trj = trj_trajectory_deserialize(serialized_trj);
	
	
	result = trj->hasspeed;
	PG_RETURN_INT32(result);
}


PG_FUNCTION_INFO_V1(trj_get_location_n);
Datum trj_get_location_n(PG_FUNCTION_ARGS)
{
	//char *result = NULL;
	double result;
	location *loc = NULL;
	uint8_t *serialized_trj = NULL;

	trajectory *trj = NULL;
	serialized_trj = (uint8_t *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	trj = trj_trajectory_deserialize(serialized_trj);


	
	loc = trj->locations[1];
	result = (double)loc->time;
	//snprintf(result, 100, "%g", loc->x);
	PG_RETURN_FLOAT8(result);
}






