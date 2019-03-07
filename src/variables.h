/*  This file is part of the Solver for Udine Course Timetabling Problem based 
 *  on Multiphase Exploitation of Multiple Objective-/Value-restricted Submodels 
 *  (abbreviated MEMOS for Timetabling).
 *
 *  Copyright 2009 Jakub Marecek
 *  Copyright 2009 The University of Nottingham
 *  http://www.cs.nott.ac.uk/~jxm/timetabling/memos/
 *  http://memos-solvers.sourceforge.net/ 
 *
 *  MEMOS for Timetabling is free software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation, either version 3 of the License, or (at your 
 *  option) any later version. It would be greatly appreciated, however, if you
 *  could cite the following paper in any work that uses it:
 *    Edmund K. Burke, Jakub Marecek, Andrew J. Parkes, and Hana Rudova: 
 *    Decomposition, Reformulation, and Diving in University Course Timetabling.
 *    Computers and Operations Research. DOI 10.1016/j.cor.2009.02.023
 */


#ifndef UDINE_VARIABLES
#define UDINE_VARIABLES

#include <sstream>
#include <ilcplex/ilocplex.h>

#include "solver_config.h"
#include "loader.h"
#include "neighbourhood.h"

ILOSTLBEGIN

namespace Udine {

struct Variables {

  // The main array of decision variables
  IloArray< IloArray<IloIntVarArray> > x;
  // ... indexed first with periods, then rooms, then courses

  // Plus some auxiliary decision variables
  IloIntVarArray courseMinDayViolations;
  // ... indexed with courses
  IloArray<IloIntVarArray> courseRooms;
  // ... indexed with courses first, rooms second
  IloArray<IloIntVarArray> coursePeriods;
  // ... indexed with courses first, periods second
  IloArray<IloIntVarArray> courseDays;
  // ... indexed with courses first, days second
  IloArray< IloArray<IloIntVarArray> > singletonChecks;
  // ... indexed with curricula first, days second, index last

  // Plus some extras for the objective function
  IloIntVar penaltyRoomStability;
  IloIntVar penaltyRoomCapacity;
  IloIntVar penaltyPeriodSingletons;
  IloIntVar penaltyPeriodSpread;

  // Initialisation of it all
  Variables(IloEnv env, Config &config, Instance &i, Neighbourhood &def)
    : x(IloArray< IloArray<IloIntVarArray> >(env)),
      courseRooms(IloArray<IloIntVarArray>(env)),
      coursePeriods(IloArray<IloIntVarArray>(env)),
      courseDays(IloArray<IloIntVarArray>(env)),
      courseMinDayViolations(IloIntVarArray(env)),
      singletonChecks(IloArray< IloArray<IloIntVarArray> >(env)),
      penaltyRoomStability(env, 0, 214783647, "penaltyRoomStability"),
      penaltyRoomCapacity(env, 0, 214783647, "penaltyRoomCapacity"),
      penaltyPeriodSingletons(env, 0, 214783647, "penaltyPeriodSingletons"),
      penaltyPeriodSpread(env, 0, 214783647, "penaltyPeriodSpread")
  {

    int p, d, r, c, u, s;  // periods, days, rooms, courses, curricula, checks

    // Initialize the core decision variables
    for (p = 0; p < i.getPeriodCount(); p++) {
      IloArray<IloIntVarArray> forPeriod = IloArray<IloIntVarArray>(env);
      for (r = 0; r < i.getRoomCount(def.type); r++) {  
        IloIntVarArray forRoom(env);
        for (c = 0; c < i.getCourseCount(); c++) {  
          std::stringstream name;
          name << "x(" << p << "," << r << "," << c << ")";
          IloIntVar var(env, 0, 1, name.str().c_str());
          forRoom.add(var);
        }
        forPeriod.add(forRoom);
      }
      x.add(forPeriod);
    }

    // Initialize some auxiliary decision variables
    for (c = 0; c < i.getCourseCount(); c++) {
      IloIntVarArray forCourse(env); 
      for (r = 0; r < i.getRoomCount(def.type); r++) {
        std::stringstream name;
        name << "courseRoom(" << c << "," << r << ")";
        IloIntVar var(env, 0, 1, name.str().c_str());
        forCourse.add(var);
      }
      courseRooms.add(forCourse);
    }

    if (def.type != Surface) {
      if (config.getParam(UseAdditionalVariables))
        for (c = 0; c < i.getCourseCount(); c++) {
          IloIntVarArray forCourse(env); 
          for (p = 0; p < i.getPeriodCount(); p++) {
            std::stringstream name;
            name << "coursePeriod(" << c << "," << p << ")";
            IloIntVar var(env, 0, 1, name.str().c_str());
            forCourse.add(var);
          }
          coursePeriods.add(forCourse);
        }
    }  // endif (def.type != Surface)

    if (def.type != FixPeriod)  { // For FixDays or similar

      for (u = 0; u < i.getProperCurriculumCount(); u++) {
        IloArray<IloIntVarArray> forCurriculum(env);
        for (d = 0; d < i.getDayCount(); d++) {
          IloIntVarArray forDay(env);
          if ((def.type == Surface && config.getParam(UseHeuristicCompactnessAtSurface))
            || (def.type == FixDay && config.getParam(UseHeuristicCompactnessInDayDives))) {       
            std::stringstream name;
            name << "singletonCheck(" << u << "," << d << ")";
            IloIntVar var(env, 0, 1, name.str().c_str());
            forDay.add(var);
          } else {
            for (s = 0; s < i.getCheckCount(); s++) {
              std::stringstream name;
              name << "singletonCheck(" << u << "," << d << "," << s << ")";
              IloIntVar var(env, 0, 1, name.str().c_str());
              forDay.add(var);
            }
          }
          forCurriculum.add(forDay);
        }
        singletonChecks.add(forCurriculum);
      }
    }  // endif (def.type != FixPeriod)

    if (def.type == Monolithic || def.type == Surface)  {

      // Initialize the auxiliary "courseMinDayViolations"
      for (c = 0; c < i.getCourseCount(); c++) {  
        std::stringstream name;
        name << "minDayViol(" << c << ")";
        IloIntVar var(env, 0, 1, name.str().c_str());
        courseMinDayViolations.add(var);
      }

      // Initialize the auxiliary "courseDays"
      for (c = 0; c < i.getCourseCount(); c++) {  
        IloIntVarArray forCourse(env);
        for (d = 0; d < i.getDayCount(); d++) {
          std::stringstream name;
          name << "courseDays(" << c << "," << d << ")";
          IloIntVar var(env, 0, 1, name.str().c_str());
          forCourse.add(var);
        }
        courseDays.add(forCourse);
      }

    } // endif (def.type == Monolithic || def.type == Surface)
  }

}; // END of Variables

} // END of namespace

#endif // UDINE VARIABLES
