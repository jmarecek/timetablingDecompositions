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


#include "model.h"

using namespace Udine;


// (HARD) Generate all ``strictly necessary'' hard constraints
void Model::generateHardConstraints() {

  Instance &i = instance;
  Neighbourhood &def = definition;
  int p, r, c;     // periods, rooms, courses
  int u, ui;       // curricula, index within curriculum's list of courseIds
  int f;           // restrictions

  // (HARD) Allocate the right amount of events for each course
  for (c = 0; c < i.getCourseCount(); c++) {
    IloExpr sum(env);
    for (p = 0; p < i.getPeriodCount(); p++)
      for (r = 0; r < i.getRoomCount(def.type); r++)
        sum += vars.x[p][r][c];
    constraints.add(sum == i.getCourse(c).lectures);
    sum.end();
  }

  // (HARD) There is only a limited number of rooms available
  for (p = 0; p < i.getPeriodCount(); p++) {
    IloExpr sum(env);
    for (c = 0; c < i.getCourseCount(); c++)
      for (r = 0; r < i.getRoomCount(def.type); r++)
        sum += vars.x[p][r][c];
    constraints.add(sum <= i.getRoomTotalMultiplicity(def.type));
    sum.end();
  }

  // (HARD) No two lectures can take place in the same room in the same period
  for (p = 0; p < i.getPeriodCount(); p++)
    for (r = 0; r < i.getRoomCount(def.type); r++) {
      IloExpr sum(env);
      for (c = 0; c < i.getCourseCount(); c++)
        sum += vars.x[p][r][c];
      constraints.add(sum <= i.getRoomMultiplicity(def.type, r));
      sum.end();
    }

  // (HARD) Lectures in one curriculum must be scheduled at different times
  for (p = 0; p < i.getPeriodCount(); p++)
    for (u = 0; u < i.getCurriculumCount(); u++)
      if (config.getParam(UseSpecialOrderedSets)) {
        IloNumVarArray sum(env);
        for (ui = 0; ui < i.getCurriculum(u).courseIds.size(); ui++) {
          c = i.getCurriculum(u).courseIds.at(ui);
          for (r = 0; r < i.getRoomCount(def.type); r++)
            sum.add(vars.x[p][r][c]);
        }
        model.add(IloSOS1(env, sum));
        sum.end();
      } else {
        IloExpr sum(env);
        for (ui = 0; ui < i.getCurriculum(u).courseIds.size(); ui++) {
          c = i.getCurriculum(u).courseIds.at(ui);
          for (r = 0; r < i.getRoomCount(def.type); r++)
            sum += vars.x[p][r][c];
        }
        constraints.add(sum <= 1);
        sum.end();
      }

  // (HARD) Teachers might be not available during some periods
  for (f = 0; f < i.getRestrictionCount(); f++)
    for (r = 0; r < i.getRoomCount(def.type); r++)  
      constraints.add(vars.x[i.getRestriction(f).period][r][i.getRestriction(f).courseId] == 0);
}  // END TimetablingSolver::generateHardConstraints


// (SOFT) The lectures of each course should be spread onto a given minimum number of days
void Model::generateMinCourseDays() {

  Instance &i = instance;
  Neighbourhood &def = definition;
  int p, d, r, c;  // periods, days, rooms, courses

  if (def.type == Surface || (def.type == Monolithic && !config.getParam(UseAdditionalVariables))) {

    // Mark the days when the course has lectures in the schedule
    for (c = 0; c < i.getCourseCount(); c++)
      for (d = 0; d < i.getDayCount(); d++)
        for (p = d * i.getPeriodsPerDayCount(); p < (d + 1) * i.getPeriodsPerDayCount(); p++) {
          IloExpr sum(env);
          for (r = 0; r < i.getRoomCount(def.type); r++)
            sum += vars.x[p][r][c];
          constraints.add(sum - vars.courseDays[c][d] <= 0);
        }

    if (!config.getParam(UsePreprocessingFriendlyFormulation))
      for (c = 0; c < i.getCourseCount(); c++)
        for (d = 0; d < i.getDayCount(); d++) {
          IloExpr sum(env);
          for (p = d * i.getPeriodsPerDayCount(); p < (d + 1) * i.getPeriodsPerDayCount(); p++) 
            for (r = 0; r < i.getRoomCount(def.type); r++)
              sum += vars.x[p][r][c];
          constraints.add(sum - vars.courseDays[c][d] >= 0);
          sum.end();
        }

    // (CUT) how many events there can be on a day, given the number of violations?
    if (config.getParam(UseStaticImpliedBounds))
      for (c = 0; c < i.getCourseCount(); c++)
        for (d = 0; d < i.getDayCount(); d++) {
          IloExpr sum(env);
          for (p = d * i.getPeriodsPerDayCount(); p < (d + 1) * i.getPeriodsPerDayCount(); p++) 
            for (r = 0; r < i.getRoomCount(def.type); r++)
              sum += vars.x[p][r][c];
          constraints.add(sum + i.getCourse(c).minWorkingDays - i.getCourse(c).lectures - vars.courseMinDayViolations[c] <= 1);
          sum.end();
        }

  } // endif (def.type == Surface || (def.type == Monolithic && !config.getParam(UseAdditionalVariables)))

  if (def.type == Monolithic && config.getParam(UseAdditionalVariables)) {
    // (SOFT) The lectures of each course should be spread onto a given minimum number of days
    // Mark the days when the course has lectures in the schedule
    for (c = 0; c < i.getCourseCount(); c++)
      for (d = 0; d < i.getDayCount(); d++)
        for (p = d * i.getPeriodsPerDayCount(); p < (d + 1) * i.getPeriodsPerDayCount(); p++)
          constraints.add(vars.coursePeriods[c][p] - vars.courseDays[c][d] <= 0);
    if (!config.getParam(UsePreprocessingFriendlyFormulation))
      for (c = 0; c < i.getCourseCount(); c++)
        for (d = 0; d < i.getDayCount(); d++) {
          IloExpr sum(env);
          for (p = d * i.getPeriodsPerDayCount(); p < (d + 1) * i.getPeriodsPerDayCount(); p++) 
            sum += vars.coursePeriods[c][p];
          constraints.add(sum - vars.courseDays[c][d] >= 0);
          sum.end();
        }

    // (CUT) how many events there can be on a day, given the number of violations?
    if (config.getParam(UseStaticImpliedBounds))
      for (c = 0; c < i.getCourseCount(); c++)
        for (d = 0; d < i.getDayCount(); d++) {
          IloExpr sum(env);
          for (p = d * i.getPeriodsPerDayCount(); p < (d + 1) * i.getPeriodsPerDayCount(); p++) 
            sum += vars.coursePeriods[c][p];
          constraints.add(sum + i.getCourse(c).minWorkingDays - i.getCourse(c).lectures - vars.courseMinDayViolations[c] <= 1);
          sum.end();
        }
  }

  // Count the number of days the course has lectures.
  for (c = 0; c < i.getCourseCount(); c++) {
    IloExpr sum(env);
    for (d = 0; d < i.getDayCount(); d++)
      sum += vars.courseDays[c][d];
    constraints.add(sum + vars.courseMinDayViolations[c] - i.getCourse(c).minWorkingDays >= 0);
    sum.end();
  }

  // (CUT) lower bounding course-days
  if (config.getParam(UseStaticImpliedBounds))
    for (c = 0; c < i.getCourseCount(); c++) {
      IloExpr sum(env);
      for (d = 0; d < i.getDayCount(); d++)
        sum += vars.courseDays[c][d];
      constraints.add(sum >= 1);
      sum.end();
    }

  // (CUT) upper bounding the number of missing days
  if (config.getParam(UseStaticImpliedBounds))
    for (c = 0; c < i.getCourseCount(); c++)
      constraints.add(vars.courseMinDayViolations[c] <= i.getCourse(c).minWorkingDays - 1);

} // END of Model::generateMinCourseDays


void Model::generateCompactness() {

  Instance &i = instance;
  Neighbourhood &def = definition;
  int p, d, r, c;  // periods, days, rooms, courses
  int u, ui;       // curricula, index within curriculum's list of courseIds

  // (SOFT) Penalise isolated lectures in timetables of individual curricula
  // First check if there is an isolated lecture during the first or the last period
  for (u = 0; u < i.getProperCurriculumCount(); u++)
    for (d = 0; d < i.getDayCount(); d++) {
      IloExpr sumMorning(env);
      IloExpr sumEvening(env);
      for (ui = 0; ui < i.getCurriculum(u).courseIds.size(); ui++) {
        c = i.getCurriculum(u).courseIds.at(ui);
        p = d * i.getPeriodsPerDayCount();
        if (def.type != Surface && config.getParam(UseAdditionalVariables))
          sumMorning += vars.coursePeriods[c][p] - vars.coursePeriods[c][p + 1];
        else for (r = 0; r < i.getRoomCount(def.type); r++)  
          sumMorning += vars.x[p][r][c] - vars.x[p + 1][r][c];
        p = (d + 1) * i.getPeriodsPerDayCount() - 1;
        if (def.type != Surface && config.getParam(UseAdditionalVariables))
          sumEvening += vars.coursePeriods[c][p] - vars.coursePeriods[c][p - 1];
        else for (r = 0; r < i.getRoomCount(def.type); r++)
          sumEvening += vars.x[p][r][c] - vars.x[p - 1][r][c];
        }
      constraints.add(sumMorning - vars.singletonChecks[u][d][0] <= 0);
      if ((def.type == Surface && config.getParam(UseHeuristicCompactnessAtSurface))
          || (def.type == FixDay && config.getParam(UseHeuristicCompactnessInDayDives)))
        constraints.add(sumEvening - vars.singletonChecks[u][d][0] <= 0);
      else 
        constraints.add(sumEvening - vars.singletonChecks[u][d][1] <= 0);
      sumMorning.end();
      sumEvening.end();
    }
  // Then check the remaining periods
  for (u = 0; u < i.getProperCurriculumCount(); u++)
    for (d = 0; d < i.getDayCount(); d++)
      for (p = d * i.getPeriodsPerDayCount() + 1;
          p < (d + 1) * i.getPeriodsPerDayCount() - 1; p++) {
        IloExpr sumInbetween(env);
        for (ui = 0; ui < i.getCurriculum(u).courseIds.size(); ui++) {
          c = i.getCurriculum(u).courseIds.at(ui);
          if (def.type != Surface && config.getParam(UseAdditionalVariables))
            sumInbetween += vars.coursePeriods[c][p] - vars.coursePeriods[c][p + 1] - vars.coursePeriods[c][p - 1];
          else for (r = 0; r < i.getRoomCount(def.type); r++)
            sumInbetween += vars.x[p][r][c] - vars.x[p + 1][r][c] - vars.x[p - 1][r][c];
          }
        int s = p - d * i.getPeriodsPerDayCount() + 1;
      if ((def.type == Surface && config.getParam(UseHeuristicCompactnessAtSurface))
          || (def.type == FixDay && config.getParam(UseHeuristicCompactnessInDayDives)))
          constraints.add(sumInbetween - vars.singletonChecks[u][d][0] <= 0);
        else 
          constraints.add(sumInbetween - vars.singletonChecks[u][d][s] <= 0);
        sumInbetween.end();
      }
}  // END Compactnesss


void Model::generateRoomStability() {

  Instance &i = instance;
  Neighbourhood &def = definition;
  int p, r, c;  // periods, rooms, courses

  // (SOFT) The lectures of each course should be held all in a single room
  // Mark the rooms where the lectures of the course are held
  for (c = 0; c < i.getCourseCount(); c++)
    for (r = 0; r < i.getRoomCount(def.type); r++)
      for (p = 0; p < i.getPeriodCount(); p++)
        constraints.add(vars.courseRooms[c][r] - vars.x[p][r][c] >= 0);
  if (!config.getParam(UsePreprocessingFriendlyFormulation))
    for (c = 0; c < i.getCourseCount(); c++)
      for (r = 0; r < i.getRoomCount(def.type); r++) {
        IloExpr sum(env);
        for (p = 0; p < i.getPeriodCount(); p++)
          sum += vars.x[p][r][c];
        // TODO: Add the number of lectures into the inequality?
        constraints.add(sum - vars.courseRooms[c][r] >= 0);
        sum.end();
      }

  if (config.getParam(Udine::UseStaticImpliedBounds)) {
    for (c = 0; c < i.getCourseCount(); c++) {
      IloExpr sum(env);
      for (r = 0; r < i.getRoomCount(def.type); r++)
        sum += vars.courseRooms[c][r];      
      constraints.add(sum >= 1);
      sum.end();
    }
  }

  if (config.getParam(UseZeroRoomStability)) {
    for (c = 0; c < i.getCourseCount(); c++)
      for (r = 0; r < i.getRoomCount(def.type); r++) {
        IloExpr sum(env);
        for (p = 0; p < i.getPeriodCount(); p++)
          sum += vars.x[p][r][c];
        sum -= i.getCourse(c).lectures * vars.courseRooms[c][r];
        constraints.add(sum == 0);
        sum.end();
      }
    for (c = 0; c < i.getCourseCount(); c++) {
      IloExpr sum(env);
      for (r = 0; r < i.getRoomCount(def.type); r++)
        sum += vars.courseRooms[c][r];      
      constraints.add(sum == 1);
      sum.end();
    }
  }  // END of zero room stability
}    // END of room stability


void Model::generateCoursePeriods() {

  Instance &i = instance;
  Neighbourhood &def = definition;
  int c, p, r;     // courses, periods, rooms
  int u, ui;       // curricula, index within curriculum's list of courseIds

  // (AUX) Synchronisation with coursePeriods
  for (c = 0; c < i.getCourseCount(); c++) {
    IloExpr sum(env);
    for (p = 0; p < i.getPeriodCount(); p++)
      sum += vars.coursePeriods[c][p];
    constraints.add(sum == i.getCourse(c).lectures);
    sum.end();
  }

  // (AUX) Synchronisation with coursePeriods
  for (p = 0; p < i.getPeriodCount(); p++)
    for (c = 0; c < i.getCourseCount(); c++) {
      IloExpr sum(env);
      for (r = 0; r < i.getRoomCount(def.type); r++)
        sum += vars.x[p][r][c];
      constraints.add(vars.coursePeriods[c][p] - sum == 0);
      constraints.add(i.getCourse(c).lectures * vars.coursePeriods[c][p] - sum >= 0);
      sum.end();
    }

  // (AUX) Room multiplicity in coursePeriods
  for (p = 0; p < i.getPeriodCount(); p++) {
    IloExpr sum(env);
    for (c = 0; c < i.getCourseCount(); c++)
      sum += vars.coursePeriods[c][p];
    constraints.add(sum <= i.getRoomTotalMultiplicity(def.type));
    sum.end();
  }

  // (AUX) Curricula in coursePeriods
  for (p = 0; p < i.getPeriodCount(); p++)
    for (u = 0; u < i.getCurriculumCount(); u++) {
      IloExpr sum(env);
      for (ui = 0; ui < i.getCurriculum(u).courseIds.size(); ui++) {
        IloInt c = i.getCurriculum(u).courseIds.at(ui);
        sum += vars.coursePeriods[c][p];
      }
      constraints.add(sum <= 1);
      sum.end();
    }

  // (AUX) Curricula in coursePeriods via SOS
  if (config.getParam(UseSpecialOrderedSets)) {
    IloNumVarArray sum(env);
    for (ui = 0; ui < i.getCurriculum(u).courseIds.size(); ui++) {
      c = i.getCurriculum(u).courseIds.at(ui);
      sum.add(vars.coursePeriods[i.getCurriculum(u).courseIds.at(ui)][p]);
    }
    model.add(IloSOS1(env, sum));
    sum.end();
  }

  // (CUT/AUX) Clique cuts added statically with additional variables
  int clique, ci;  // clique and index within
  if (config.getParam(UseStaticCliqueCutsInDives))
    for (p = 0; p < i.getPeriodCount(); p++)
      for (clique = 0; clique < conflictGraph.cliques.size(); clique++)
        if (config.getParam(UseSpecialOrderedSets)) {
          IloNumVarArray sum(env);
          for(ci = 0; ci < conflictGraph.cliques.at(clique).size(); ci++)
            sum.add(vars.coursePeriods[conflictGraph.cliques[clique][ci]][p]);
          model.add(IloSOS1(env, sum));
        } else {
          IloExpr sum(env);
          for(ci = 0; ci < conflictGraph.cliques.at(clique).size(); ci++)
            sum += vars.coursePeriods[conflictGraph.cliques[clique][ci]][p];
          constraints.add(sum <= 1);
        }
}


void Model::generateNeighbourhood() {

  Instance &i = instance;
  Neighbourhood &def = definition;
  int p, r;  // periods, days, rooms, courses

  // (NEIGHBOURHOOD) Fix the assignment to periods
  std::vector<CoursePeriodPair>::iterator itFixPeriod;
  for (itFixPeriod = def.fixPeriod.begin(); itFixPeriod != def.fixPeriod.end(); itFixPeriod++) {
    IloExpr sum(env);
    for (r = 0; r < i.getRoomCount(def.type); r++)
      sum += vars.x[itFixPeriod->period][r][itFixPeriod->course];
    constraints.add(sum == 1);
    if (def.type != Surface && config.getParam(UseAdditionalVariables))
      constraints.add(vars.coursePeriods[itFixPeriod->course][itFixPeriod->period] == 1);
    sum.end();
  }
  
  // (NEIGHBOURHOOD) Fix the assignment to days
  std::vector<CourseDayNumberTriple>::iterator itFixDay;
  for (itFixDay = def.fixDay.begin(); itFixDay != def.fixDay.end(); itFixDay++) {
    IloExpr sumXes(env);
    for (p = itFixDay->day * i.getPeriodsPerDayCount(); p < (itFixDay->day + 1) * i.getPeriodsPerDayCount(); p++)
      for (r = 0; r < i.getRoomCount(def.type); r++)
        sumXes += vars.x[p][r][itFixDay->course];
    constraints.add(sumXes == itFixDay->events);
    sumXes.end();
  }

  // (AUX) Synchronise with coursePeriod
  if (def.type != Surface && config.getParam(UseAdditionalVariables)) 
    for (itFixDay = def.fixDay.begin(); itFixDay != def.fixDay.end(); itFixDay++) {
      IloExpr sumPeriods(env);
      for (p = itFixDay->day * i.getPeriodsPerDayCount(); 
           p < (itFixDay->day + 1) * i.getPeriodsPerDayCount(); p++)
        sumPeriods += vars.coursePeriods[itFixDay->course][p];
      constraints.add(sumPeriods == itFixDay->events);
      sumPeriods.end();
    }

  // (NEIGHBOURHOOD) Preprocess things away
  std::vector<CoursePeriodPair>::iterator it3;
  for (it3 = def.preprocessAway.begin(); it3 != def.preprocessAway.end(); it3++) {
    for (r = 0; r < i.getRoomCount(def.type); r++)
      constraints.add(vars.x[(*it3).period][r][(*it3).course] == 0);
    if (def.type != Surface && config.getParam(UseAdditionalVariables))
      constraints.add(vars.coursePeriods[(*it3).course][(*it3).period] == 0);
  }
}


void Model::generateObjectiveComponents() {

  Instance &i = instance;
  Neighbourhood &def = definition;
  int p, d, r, c, u;  // periods, days, rooms, courses, curricula

  if (def.type == Monolithic || def.type == Surface) {
    IloExpr sumPeriodSpread(env);
    for (c = 0; c < i.getCourseCount(); c++)
      sumPeriodSpread += config.getWeights(def.type).wMinDays * vars.courseMinDayViolations[c];
    constraints.add(sumPeriodSpread - vars.penaltyPeriodSpread == 0);
    constraints.add(vars.penaltyPeriodSpread >= 0);
    obj += vars.penaltyPeriodSpread;
  }

  if (def.type != FixPeriod) {
    int s;  // check
    IloExpr sumPeriodSingletons(env);
    for (u = 0; u < i.getProperCurriculumCount(); u++)
      for (d = 0; d < i.getDayCount(); d++)
        if ((def.type == Surface && config.getParam(UseHeuristicCompactnessAtSurface))
            || (def.type == FixDay && config.getParam(UseHeuristicCompactnessInDayDives)))
          sumPeriodSingletons += config.getWeights(def.type).wCompactness * vars.singletonChecks[u][d][0];
        else for (s = 0; s < i.getCheckCount(); s++)
          sumPeriodSingletons += config.getWeights(def.type).wCompactness * vars.singletonChecks[u][d][s];
    constraints.add(sumPeriodSingletons - vars.penaltyPeriodSingletons == 0);
    constraints.add(vars.penaltyPeriodSingletons >= 0);
    obj += vars.penaltyPeriodSingletons;
  }

  // Sum up the the number of missing seats ...
  IloExpr sumRoomCapacity(env);
  for (r = 0; r < i.getRoomCount(def.type); r++)
    for (c = 0; c < i.getCourseCount(); c++)
      if (i.getCourse(c).students > i.getRoomPerEventCapacity(def.type, r))
        for (p = 0; p < i.getPeriodCount(); p++)
          sumRoomCapacity += config.getWeights(def.type).wRoomCapacity *
                 vars.x[p][r][c] * (i.getCourse(c).students - i.getRoomPerEventCapacity(def.type, r));
  constraints.add(sumRoomCapacity - vars.penaltyRoomCapacity == 0);
  constraints.add(vars.penaltyRoomCapacity >= 0);
  obj += vars.penaltyRoomCapacity;

  IloExpr sumRoomStability(env);
  // ... add the number of rooms used on the top of a single room per course
  if (!config.getParam(UseZeroRoomStability)) {
    for (c = 0; c < i.getCourseCount(); c++)
      for (r = 0; r < i.getRoomCount(def.type); r++)
        sumRoomStability += config.getWeights(def.type).wRoomStability * vars.courseRooms[c][r];
    sumRoomStability -= config.getWeights(def.type).wRoomStability * i.getCourseCount();
    constraints.add(sumRoomStability - vars.penaltyRoomStability == 0);
    constraints.add(vars.penaltyRoomStability >= 0);
    obj += vars.penaltyRoomStability;
  }

} // END TimetablingSolver::generateObjectiveComponents


void Model::generateObjective() {

  Instance &i = instance;
  Neighbourhood &def = definition;
  int p, d, r, c, u;  // periods, days, rooms, courses, curricula

  // Get the number of missing days of instruction
  if (def.type == FixPeriod || def.type == FixDay) {
    obj += config.getWeights(def.type).wMinDays * def.penaltyMinCourseDays;
  } else { // elif (def.type == Monolithic || def.type == Surface) {
    for (c = 0; c < i.getCourseCount(); c++)
      obj += config.getWeights(def.type).wMinDays 
             * vars.courseMinDayViolations[c];
  }

  // ... add the number of gaps in timetables for individual curricula
  if (def.type == FixPeriod) {
    obj += config.getWeights(def.type).wCompactness * def.penaltyCompactness;
  } else { // elif (def.type != FixPeriod)
    int s;  // check
    for (u = 0; u < i.getProperCurriculumCount(); u++)
      for (d = 0; d < i.getDayCount(); d++)
        if ((def.type == Surface && config.getParam(UseHeuristicCompactnessAtSurface))
            || (def.type == FixDay && config.getParam(UseHeuristicCompactnessInDayDives)))
          obj += config.getWeights(def.type).wCompactness * vars.singletonChecks[u][d][0];
        else for (s = 0; s < i.getCheckCount(); s++)
          obj += config.getWeights(def.type).wCompactness * vars.singletonChecks[u][d][s];
  }

  // Sum up the the number of missing seats ...
  for (r = 0; r < i.getRoomCount(def.type); r++)
    for (c = 0; c < i.getCourseCount(); c++)
      if (i.getCourse(c).students > i.getRoomPerEventCapacity(def.type, r))
        for (p = 0; p < i.getPeriodCount(); p++)
          obj += config.getWeights(def.type).wRoomCapacity *
                 vars.x[p][r][c] * (i.getCourse(c).students - i.getRoomPerEventCapacity(def.type, r));

  // ... add the number of rooms used on the top of a single room per course
  if (!config.getParam(UseZeroRoomStability)) {
    for (c = 0; c < i.getCourseCount(); c++)
      for (r = 0; r < i.getRoomCount(def.type); r++)
        obj += config.getWeights(def.type).wRoomStability * vars.courseRooms[c][r];
    obj -= config.getWeights(def.type).wRoomStability * i.getCourseCount();
  }

} // END TimetablingSolver::generateObjective