#ifndef _TREJECTORY_H
#define _TRAJECTORY_H

#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <limits.h>
#include <math.h>

#include "liblwgeom.h"
#include "lwgeom_pg.h"
#include "trj_api.h"


Datum trj_in(PG_FUNCTION_ARGS);
Datum trj_out(PG_FUNCTION_ARGS);
Datum trj_trajectory_makeEmpty(PG_FUNCTION_ARGS);
Datum trj_location_makeEmpty(PG_FUNCTION_ARGS);

Datum trj_get_srid(PG_FUNCTION_ARGS);
Datum trj_get_nvolume(PG_FUNCTION_ARGS);
Datum trj_get_nlocation(PG_FUNCTION_ARGS);
Datum trj_get_extent(PG_FUNCTION_ARGS);
Datum trj_get_starttime(PG_FUNCTION_ARGS);
Datum trj_get_endtime(PG_FUNCTION_ARGS);
Datum trj_get_hasaccuracy(PG_FUNCTION_ARGS);
Datum trj_get_hasaltitude(PG_FUNCTION_ARGS);
Datum trj_get_hasbearing(PG_FUNCTION_ARGS);
Datum trj_get_hasspeed(PG_FUNCTION_ARGS);
Datum trj_get_location_n(PG_FUNCTION_ARGS);


#endif
