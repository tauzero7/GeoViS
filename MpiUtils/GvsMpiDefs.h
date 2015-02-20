// ---------------------------------------------------------------------
//  Copyright (c) 2013-2014, Universitaet Stuttgart, VISUS, Thomas Mueller
//
//  This file is part of GeoViS.
//
//  GeoViS is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  GeoViS is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with GeoViS.  If not, see <http://www.gnu.org/licenses/>.
// ---------------------------------------------------------------------
#ifndef GVS_MPI_DEFS_H
#define GVS_MPI_DEFS_H

#include "Dev/GvsDevice.h"

#define TAG_START_TASK          1
#define TAG_RESULT              2
#define TAG_REGION_BUFFER       3
#define TAG_DATA_BUFFER         4

enum TaskStatus { TASK_WAITING, TASK_RUNNING, TASK_FINISHED };

class GvsMpiTask {
public:
    GvsMpiTask() : status(TASK_WAITING) {};

    TaskStatus status;
    int x1;
    int y1;
    int x2;
    int y2;
    int size;
    GvsDevice device;

    int imageNr;
};


typedef struct _MPIMsgPt {
    int continueCalc;
    int node;
    int task;
    long bytes;
    long numPixels;
} MPIMsgPt;

#endif
