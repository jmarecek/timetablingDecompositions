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


#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>

#include "loader.h"

using namespace Udine;

void checkFile(const char *path) {
  try {
    std::ifstream input(path);
    input.seekg(0, std::ios::end);
    if (input.tellg() < 1) {
      std::cerr << "Loader: The data file " << path << " has zero lenght" << std::endl; 
      std::abort();
    }
    input.close();
  } catch (...) { 
    std::cerr << "Loader: There was an error opening the data file " << path << std::endl;
    std::abort(); 
  }
}


std::istream& operator>>(std::istream &is, Course &c) {
  return is >> c.name >> c.teacher >> c.lectures >> c.minWorkingDays >> c.students;
}


void Instance::load(const char *filename) {
  try {
    std::ifstream file;
    file.open(filename, std::ifstream::in);    

    std::string forget; int i;
    std::map< std::string, int, std::less<std::string> > cnames;

    std::string iname;
    int courseCnt, eventCnt, roomCnt, dayCnt, curriculumCnt, constraintCnt;
    
    // load the header
    file >> forget >> iname; 
    file >> forget >> courseCnt; eventCnt = 0;
    file >> forget >> roomCnt;
    file >> forget >> dayCnt;
    file >> forget >> periodsPerDay;
    file >> forget >> curriculumCnt;
    file >> forget >> constraintCnt;

    periods = dayCnt * periodsPerDay;
    days = dayCnt;
    checks = periodsPerDay;

    std::cout << "Loader: Instance " << iname << " (" << filename << ")"<< std::endl; 

    // courses
    file >> forget;
    for (i = 0; i < courseCnt; i++) {
      Course c;
      assert(file.good());
      file >> c;
      cnames[c.name] = i;
      eventCnt += c.lectures;
      courses.push_back(c);
    }
    
    // rooms and their capacities
    file >> forget;
    for (i = 0; i < roomCnt; i++) {
      Room room;
      assert(file.good());
      file >> room.name >> room.capacity;
      rooms.push_back(room);
    }
    RoomCapacityLess capacityLess;
    std::sort(rooms.begin(), rooms.end(), capacityLess);
    
    genMRoomsAggregates();

    // groups of conflicting courses
    file >> forget;
    for (i = 0; i < curriculumCnt; i++) {
      Curriculum u; int ccount;
      file >> u.name >> ccount;
      for (int j = 0; j < ccount; j++) {
        std::string s;
        assert(file.good());
        file >> s;
        assert(cnames.find(s) != cnames.end());
        u.courseIds.push_back(cnames.find(s)->second);
      }
      curricula.push_back(u);
    }
    origCurricula = curricula.size();

    // restrictions on times when teachers are available
    file >> forget;
    for (i = 0; i < constraintCnt; i++) {
      std::string cname; int day; int period;
      assert(file.good());
      file >> cname >> day >> period;
      Restriction r;
      assert(cnames.find(cname) != cnames.end());
      r.courseId = cnames.find(cname)->second;
      r.period = day * periodsPerDay + period;
      restrict.push_back(r);
    }

    // make an overview of who teaches what
    typedef std::map< std::string, std::vector<int>, std::less<std::string> > s2veci;
    s2veci tnames;
    std::vector<Course>::iterator it = courses.begin();
    for (i = 0; it != courses.end(); it++, i++) {
      if (tnames.find(it->teacher) == tnames.end()) {
        std::vector<int> teaches;
        teaches.push_back(i);
        tnames[it->teacher] = teaches;
      } else {
        tnames.find(it->teacher)->second.push_back(i);
      }
    }
    // look for teachers teaching more than a single course 
    s2veci::iterator mapit = tnames.begin();
    for (; mapit != tnames.end(); mapit++) {
      if (mapit->second.size() > 1) {
        // create artificial curricula out of this
        Curriculum c;
        c.courseIds = mapit->second;
        c.name = mapit->first;
        curricula.push_back(c);
      }
    }

    if (file.bad()) {
      std::cout << "Loader: Instance incomplete!" << std::endl; 
      abort();
    } else {
      std::cout << "Loader: " << courseCnt << " courses, ";
      std::cout << eventCnt << " events, and ";
      std::cout << curriculumCnt << " curricula" << std::endl; 
    }

    file.close();

    genPatterns(periodsPerDay);
    
  } catch (std::exception &e) {
    std::cerr << "Loader: There was an error reading the instance!" << std::endl;
    std::cerr << "Exception says: " << e.what() << std::endl;
    abort();
  }
}  // END of Instance::load



// NOTE: Does not support the trivial cases of days of less than three periods
void Instance::genPatterns(int toAdd, int rhs, std::vector<int> pat) {

  const int minus = -1;
  const int plus = 1;

  // Recursion
  if (toAdd > 0) {
    pat.push_back(minus);
    genPatterns(toAdd - 1, rhs, pat);
    pat.pop_back();
    pat.push_back(plus);
    genPatterns(toAdd - 1, rhs + plus, pat);
    return;
  }

  // Evaluate the pattern, if complete 
  int penalty = 0;
  int last = pat.size() - 1;
  if (pat.size() < 3) return;

  if (pat[0] == plus && pat[1] == minus) penalty += 1;
  if (pat[last] == plus && pat[last - 1] == minus) penalty += 1;

  for (int i = 0; i < pat.size() - 2; i++) 
    if (pat[i] == minus && pat[i + 1] == plus && pat[i + 2] == minus)
      penalty += 1;

  // Save it if it has attracted any penalty
  if (penalty > 0) {
    Pattern p;
    p.coefs = pat;
    p.penalty = penalty;
    p.rhs = rhs;
    patterns.push_back(p);
  }
} // END of Instance::genPatters()


bool RoomCapacityLess::operator()(const Room &x, const Room &y ) {
  if (x.capacity != y.capacity) return (x.capacity > y.capacity);
  return (x.name > y.name);
}


// Take "Rooms rooms" sorted by capacity and generate "MRoomsVersions mRooms",
// ie. aggregate rooms, as appropriate for various types of models
void Instance::genMRoomsAggregates() {

  for (int type = 0; type < ModelTypeLen; type++) {

    switch(type) {

      case Monolithic: 
      case FixPeriod:
      {
        for (int i = 0; i < rooms.size(); i++) {
          MRoom room;
          room.multiplicity = 1;
          room.perEventCapacity = rooms.at(i).capacity;
          room.rooms.push_back(rooms.at(i));
          mRooms.at(type).push_back(room);
        }
      }
      break;

      case FixDay: 
      {
        MRoom mRoom;
        for (int i = 0; i < rooms.size() - 1; i++) {
          mRoom.multiplicity += 1;
          mRoom.perEventCapacity = std::max(mRoom.perEventCapacity, rooms.at(i).capacity);
          mRoom.rooms.push_back(rooms.at(i));
          if (rooms.at(i).capacity != rooms.at(i + 1).capacity) {
            mRooms.at(type).push_back(mRoom);
            mRoom.rooms.clear();
            mRoom.multiplicity = 0;
            mRoom.perEventCapacity = rooms.at(i + 1).capacity;
          }
        }
        mRoom.multiplicity += 1;
        mRoom.perEventCapacity = rooms.at(rooms.size() - 1).capacity;
        mRoom.rooms.push_back(rooms.at(rooms.size() - 1));
        mRooms.at(type).push_back(mRoom);
      }
      break;

      case Surface:
      {
        MRoom large, small;
        int i;

        std::vector<int> distinct;
        distinct.push_back(rooms.at(0).capacity);
        for (i = 1; i < rooms.size(); i++) {
          if (rooms.at(i - 1).capacity != rooms.at(i).capacity)
            distinct.push_back(rooms.at(i).capacity);
        }

        /*
        float cost = 0;
        for (i = 0; i < distinct.size() - 1; i++) {
          int now = abs(distinct.at(i) - distinct.at(i + 1)) / 
                    (0.2 * abs((int)(i - distinct.size())));
          std::cout << abs(distinct.at(i) - distinct.at(i + 1)) << " "
            << abs((int)(i - distinct.size() - 1)) << std::endl;
          if (now > cost) 
            splitAt = (distinct.at(i) + distinct.at(i + 1)) / 2;
        }
        */
        
        float splitAt = distinct.at(floor((float)(distinct.size() / 2))) + 1;

        for (i = 0; i < rooms.size(); i++) {
          if (rooms.at(i).capacity < splitAt) {
            small.multiplicity += 1;
            small.perEventCapacity = std::max(small.perEventCapacity, rooms.at(i).capacity);
            small.rooms.push_back(rooms.at(i));
          } else {
            large.multiplicity += 1;
            large.perEventCapacity = std::max(large.perEventCapacity, rooms.at(i).capacity);
            large.rooms.push_back(rooms.at(i));
          }
        }
        std::cout << "The surface mRooms have capacity " << small.perEventCapacity;
        std::cout << " and " << large.perEventCapacity << std::endl;
        mRooms.at(type).push_back(small);
        mRooms.at(type).push_back(large);
        
        /*
        for (i = 0; i < rooms.size(); i++) {
          small.multiplicity += 1;
          small.perEventCapacity = std::max(small.perEventCapacity, rooms.at(i).capacity);
          small.rooms.push_back(rooms.at(i));
        }
        mRooms.at(type).push_back(small);
        */
        
      }
      break;
    }
  }
}
