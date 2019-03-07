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


#ifndef UDINE_LOADER
#define UDINE_LOADER

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <utility>

#include "solver_config.h"


namespace Udine {

/* This loader is based on the code of Andrea Schaerf, although
   there doesn't seem to be a single line copied in verbatim.
 */

struct Course {
  std::string name, teacher; 
  int lectures, minWorkingDays, students;
};
typedef std::vector<Course> Courses;

struct Room {
  std::string name; 
  int capacity;
};
typedef std::vector<Room> Rooms;
struct RoomCapacityLess { bool operator()(const Room &x, const Room &y ); };

struct MRoom {
  std::vector<Room> rooms;
  int multiplicity;
  int perEventCapacity;
  MRoom() { multiplicity = 0; perEventCapacity = 0; }
};
typedef std::vector<MRoom> MRooms;
typedef std::vector<MRooms> MRoomsVersions;

typedef std::vector<int> CourseIds;
struct Curriculum {
  std::string name; 
  CourseIds courseIds;
};
typedef std::vector<Curriculum> Curricula;

struct Restriction {
  int courseId; 
  int period;
};
typedef std::vector<Restriction> Restrictions;

// Patterns to penalise, with "rhs" containing the number of "hits" minus one
struct Pattern { std::vector<int> coefs; int penalty; int rhs; };
typedef std::vector<Pattern> PatternDB;

class Instance {
protected:
  int periods, periodsPerDay, days, checks;
  Courses courses;
  Rooms rooms;
  MRoomsVersions mRooms;      // aggregated rooms, varying from modeltype to modeltype
  int origCurricula;          // the number of the original curricula, without auxiliaries
  Curricula curricula;
  Restrictions restrict;
  PatternDB patterns;

  // generation of patterns to penalise
  void genPatterns(int periodsPerDay, int rhs = -1, std::vector<int> soFar = std::vector<int>());
  void genMRoomsAggregates();

public:
  Instance() : mRooms(MRoomsVersions(ModelTypeLen)) {}

  void load(const char *filename);

  int getPeriodCount() { return periods; }
  int getDayCount() { return days; }
  int getPeriodsPerDayCount() { return periodsPerDay; }
  int getCheckCount() { return checks; }
  int getCourseCount() { return courses.size(); }
  int getProperCurriculumCount() { return origCurricula; }
  int getCurriculumCount() { return curricula.size(); }
  int getRestrictionCount() { return restrict.size(); }

  const Course & getCourse(int i) { 
    assert(i >= 0 && i < courses.size());
    return courses.at(i); 
  }

  const Curriculum & getCurriculum(int u) { 
    assert(u >= 0 && u < curricula.size());  
    return curricula.at(u); 
  }

  const Restriction & getRestriction(int i) { 
    assert(i >= 0 && i < restrict.size());
    return restrict.at(i); 
  }

  int getRoomCount(ModelType t) { 
    assert(t >= 0 && t < ModelTypeLen); 
    return mRooms.at(t).size(); 
  }

  int getRoomTotalMultiplicity(ModelType t) {
    assert(t >= 0 && t < ModelTypeLen); 
    int multiplicity = 0;
    for(int r = 0; r < mRooms.at(t).size(); r++)
      multiplicity += mRooms.at(t).at(r).multiplicity;
    return multiplicity;
  }

  const std::string getRoomName(int r) {
    assert(r >= 0 && r < rooms.size()); 
    return rooms.at(r).name;
  }

  int getRoomMultiplicity(ModelType t, int r) { 
    assert(t >= 0 && t < ModelTypeLen); 
    assert(r >= 0 && r < mRooms.at(t).size()); 
    return mRooms.at(t).at(r).multiplicity;
  }

  int getRoomPerEventCapacity(ModelType t, int r) { 
    assert(t >= 0 && t < ModelTypeLen); 
    assert(r >= 0 && r < mRooms.at(t).size());
    return mRooms.at(t).at(r).perEventCapacity;
  }

  PatternDB & getPatterns() { 
    return patterns; 
  }
};

}

#endif // UDINE LOADER
