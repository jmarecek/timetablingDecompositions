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


#include "strategy.h"

#include "solver_config.h"
#include "neighbourhood.h"
#include "model.h"
#include "cuts.h"

using namespace Udine;

inline int roundProperly(double x) { return int(std::floor(x + 0.5f)); }

int StrategyI::sumMissingCourseDays() {
  if (model.definition.type == FixDay || model.definition.type == FixPeriod)
    return model.definition.penaltyMinCourseDays;

  int i = 0;
  for (int c = 0; c < model.instance.getCourseCount(); c++)
    try { i += roundProperly(getValue(model.vars.courseMinDayViolations[c])); } 
    catch (IloAlgorithm::NotExtractedException e)  { std::cerr << "Error retrieving " << model.vars.courseMinDayViolations[c].getName() << "." << std::endl; }
    catch (...)  { std::cerr << "Unknown error retrieving " << model.vars.courseMinDayViolations[c].getName() << "." << std::endl; throw; }
  return i;
}

int StrategyI::sumSingletonChecks() {
  if (model.definition.type == FixPeriod)
    return model.definition.penaltyCompactness;

  int i = 0;
  // TODO: if (config.getParam(UseSpreadOnlyNeighbourhoods)) return i;
  int limit = model.instance.getCheckCount();
  if ((model.definition.type == Surface && config.getParam(UseHeuristicCompactnessAtSurface))
      || (model.definition.type == FixDay && config.getParam(UseHeuristicCompactnessInDayDives)))
    limit = 1;

  for (int u = 0; u < model.instance.getProperCurriculumCount(); u++)
    for (int d = 0; d < model.instance.getDayCount(); d++)
      for (int s = 0; s < limit; s++)
        try { i += roundProperly(getValue(model.vars.singletonChecks[u][d][s])); }
        catch (IloAlgorithm::NotExtractedException e)  { std::cerr << "Error retrieving " << model.vars.singletonChecks[u][d][s].getName() << std::endl; }
        catch (...) { std::cerr << "Error retrieving " << model.vars.singletonChecks[u][d][s].getName() << std::endl; throw; }
  return i;
}

int StrategyI::sumMissingSeats() {
  int i = 0;
  for (int r = 0; r < model.instance.getRoomCount(model.definition.type); r++)
    for (int c = 0; c < model.instance.getCourseCount(); c++)
      if (model.instance.getCourse(c).students > model.instance.getRoomPerEventCapacity(model.definition.type, r))
        for (int p = 0; p < model.instance.getPeriodCount(); p++)
          try { 
            if (roundProperly(getValue(model.vars.x[p][r][c])) == 1) 
              i += model.instance.getCourse(c).students - model.instance.getRoomPerEventCapacity(model.definition.type, r);
          }
          catch (IloAlgorithm::NotExtractedException e)  { std::cerr << "Error retrieving " << model.vars.x[p][r][c].getName() << std::endl; }
          catch (...) { std::cerr << "Error retrieving " << model.vars.x[p][r][c].getName() << std::endl; throw; }
  return i;
}

int StrategyI::sumExtraRoomsUsed() {
  int i = 0;
  for (int c = 0; c < model.instance.getCourseCount(); c++)
    for (int r = 0; r < model.instance.getRoomCount(model.definition.type); r++)
      // TODO: Why this could fail
      try { i += roundProperly(getValue(model.vars.courseRooms[c][r])); } 
      catch (IloAlgorithm::NotExtractedException e)  { std::cerr << "Error retrieving " << model.vars.courseRooms[c][r].getName() << std::endl; }
      catch (...) { std::cerr << "Error retrieving " << model.vars.courseRooms[c][r].getName() << std::endl; throw; }
  i -= model.instance.getCourseCount();
  return i;
}

void StrategyI::logNeighbrouhood(Neighbourhood def) {
  try {      
    std::stringstream log;
    log << "<neighbourhood ";
    log << "data='" << config.path << "' ";
    log << "discovered='" << config.elapsed() << "' ";
    log << "cost='" << def.cost << "' ";
    log << "penaltyCompactness='" << def.penaltyCompactness << "' ";
    log << "penaltyMinCourseDays='" << def.penaltyMinCourseDays << "' ";
    log << "LB='" << def.lowerBound << "' ";
    log << "'><definition>" << std::endl;

    std::vector<CoursePeriodPair>::iterator itFixPeriod;
    for (itFixPeriod = def.fixPeriod.begin(); 
         itFixPeriod != def.fixPeriod.end(); itFixPeriod++) {
      log << "<session course='" << itFixPeriod->course << "' ";
      log << " period='" << itFixPeriod->period << "'/>";
    }

    std::vector<CourseDayNumberTriple>::iterator itFixDay;
    for (itFixDay = def.fixDay.begin(); 
         itFixDay != def.fixDay.end(); itFixDay++) {
      log << "<session course='" << itFixDay->course << "' ";
      log << " day='" << itFixDay->day << "' ";
      log << " events='" << itFixDay->events << "'/>";
    }

    log << "</definition></neighbourhood>";

    if (model.config.getParam(UseNeighbourhoodLogging)) {
      env.out() << "</cplex>\n" << log.str() << "<cplex>" << std::endl;
    }

    std::stringstream filename;
    filename << config.path << ".nei" << (int)def.cost << ".xml";
    std::ofstream neiFile(filename.str().c_str(), ios::app);
    neiFile << log.str() << std::endl;
    neiFile.close();
  }
  catch (...) { std::cerr << "There was an error saving the neighbourhood." << std::endl; return; }
}


Neighbourhood StrategyI::getFixPeriodNeighbourhood() {
  Neighbourhood next;
  next.type = FixPeriod;
  next.cost = getObjValue();
  next.lowerBound = getBestObjValue();
  next.penaltyMinCourseDays = sumMissingCourseDays();
  next.penaltyCompactness = sumSingletonChecks();

  int c, p, r;  // course, period
  for(c = 0; c < model.instance.getCourseCount(); c++)
    for (p = 0; p < model.instance.getPeriodCount(); p++) {
      CoursePeriodPair pair;
      pair.period = p;
      pair.course = c;
      try { 
        IloExpr sum(env);
        for (r = 0; r < model.instance.getRoomCount(model.definition.type); r++)
          sum += model.vars.x[p][r][c];
        assert(std::fabs(1 - getValue(sum)) < 0.0001 || std::fabs(getValue(sum)) < 0.0001);
        if (roundProperly(getValue(sum)) == 1)
          next.fixPeriod.push_back(pair);
        else 
          next.preprocessAway.push_back(pair);
        if (model.definition.type == FixDay)
          assert(roundProperly(getValue(sum)) == roundProperly(getValue(model.vars.coursePeriods[c][p])));
      } catch (IloAlgorithm::NotExtractedException e)  { 
        std::cerr << "Error retrieving " << model.vars.x[p][r][c].getName() << "." << std::endl; 
      } catch (...)  { 
        std::cerr << "Unknown error retrieving " << model.vars.x[p][r][c].getName() << "." << std::endl; 
        throw; 
      }
    }
  return next;
}


Neighbourhood StrategyI::getFixDayNeighbourhood() {
  Neighbourhood next;
  next.type = FixDay;
  next.cost = getObjValue();
  next.lowerBound = getBestObjValue();
  next.penaltyMinCourseDays = sumMissingCourseDays();
  next.penaltyCompactness = sumSingletonChecks();
  int c, d, p, r;  // course, day, period, room

  for(c = 0; c < model.instance.getCourseCount(); c++) {
    CoursePeriodPair pair;
    pair.course = c;
    CourseDayNumberTriple triple;
    triple.course = c;

    for (d = 0; d < model.instance.getDayCount(); d++) {
      triple.day = d;
      triple.events = 0;
      bool ok = true;

      for (p = d * model.instance.getPeriodsPerDayCount(); 
           p < (d + 1) * model.instance.getPeriodsPerDayCount(); p++)
        try {
          IloExpr sum(env);
          for (r = 0; r < model.instance.getRoomCount(model.definition.type); r++)
            sum += model.vars.x[p][r][c];
          triple.events += roundProperly(getValue(sum));
        } catch (IloAlgorithm::NotExtractedException e)  { 
          std::cerr << "Error retrieving value of x for c = " << c << " and d = " << d << "." << std::endl;
          ok = false; 
        } catch (...) { 
          std::cerr << "Unknown error retrieving value of x for c = " << c << " and d = " << d << "." << std::endl; 
          throw; 
        }

      if (triple.events > 0) { 
        next.fixDay.push_back(triple);
      } else if (ok)
        for (p = d * model.instance.getPeriodsPerDayCount(); 
             p < (d + 1) * model.instance.getPeriodsPerDayCount(); 
             p++) {
        pair.period = p;
        next.preprocessAway.push_back(pair);
      }
    }
  }

  return next;
}


void StrategyI::runDiver(Neighbourhood next) {

  try {  

    IloModel subMIPModel(env);
    Model subMIP(subMIPModel, model.config, model.instance, model.conflictGraph, next);
    IloCplex cplex(subMIPModel);

    if (model.config.getParam(UseLpFilesExport)) {
      std::stringstream filename;
      filename << config.path << "." << next.type << next.cost << ".lp";
      cplex.exportModel(filename.str().c_str());
    }

    env.out() << std::endl << "Diving into the " << next.type << " neighbouhood from ";
    env.out() << model.definition.type << " ..." << std::endl;
    
    StrategyI *strategy = NULL;
    if (next.type == FixDay)
      strategy = new (env) SolutionPolishingStrategyI(env, subMIP, config);
    else // NOTE: assumes model.config.getParam(UseSolutionLogging)
      strategy = new (env) SolutionSavingStrategyI(env, subMIP, config);
    cplex.use(IloCplex::Callback(strategy));
    
    config.apply(cplex, next.type);
    cplex.solve();
    strategy->finishOff();
  }
  catch (IloException& e) { std::cerr << "Solver (Strategy): Concert exception caught: " << e << std::endl; }
  catch (...) { std::cerr << "Solver (Strategy): Unknown exception caught." << std::endl; }
}


/*** Anytime Strategy *************************************/

IloCplex::Callback AnytimeStrategy(IloEnv &env, Model& s, Config &configuration) {
  return (IloCplex::Callback(new (env) AnytimeStrategyI(env, s, configuration)));
}


void AnytimeStrategyI::main() {
  if (   config.anytimeDivingOnset[FixPeriod] >= 0
      && config.elapsed() > config.anytimeDivingOnset[FixPeriod]) {
      Neighbourhood def = getFixPeriodNeighbourhood();
      logNeighbrouhood(def);
      runDiver(def);
    }
  if (   config.anytimeDivingOnset[FixDay] >= 0
      && config.elapsed() > config.anytimeDivingOnset[FixDay]) {
      Neighbourhood def = getFixDayNeighbourhood();
      logNeighbrouhood(def);
      runDiver(def);
    }
}

void AnytimeStrategyI::finishOff() {
}


/*** Contract Strategy ************************************/

IloCplex::Callback ContractStrategy(IloEnv &env, Model& s, Config &configuration) {
  return (IloCplex::Callback(new (env) ContractStrategyI(env, s, configuration)));
}


void ContractStrategyI::main() {
  fixDayNeighbs.push_back(getFixDayNeighbourhood());
  fixPeriodNeighbs.push_back(getFixPeriodNeighbourhood());
  logNeighbrouhood(fixDayNeighbs.back());
}

void ContractStrategyI::finishOff() {
  int counter = 0;
  while  (config.elapsed() < config.contractTimelimit 
       && !fixPeriodNeighbs.empty()
       && counter < config.getCount(FixPeriodDiveFromSurface))
    try {
      runDiver(fixPeriodNeighbs.back());
      fixPeriodNeighbs.pop_back();
      counter += 1;
    } catch (std::exception &e) { std::cerr << "Solver (FixPeriod Dives): " << e.what() << std::endl; }
  counter = 0;
  while   (config.elapsed() < config.contractTimelimit 
        && !fixDayNeighbs.empty()
        && counter < config.getCount(FixDayDiveFromSurface)) {
    try {
      runDiver(fixDayNeighbs.back());
      fixDayNeighbs.pop_back();
      counter += 1;
    } catch (std::exception &e) { std::cerr << "Solver (FixDay Dives): " << e.what() << std::endl; }
  }
}

/*** Solution Saving Strategy ************************************/

IloCplex::Callback SolutionSavingStrategy(IloEnv &env, Model& s, Config &configuration) {
  return (IloCplex::Callback(new (env) SolutionSavingStrategyI(env, s, configuration)));
}

void SolutionSavingStrategyI::main() {
  try {
    std::stringstream log;
    std::stringstream sol;

    int pRoomCapacity = sumMissingSeats();
    int pMinDays = sumMissingCourseDays();
    int pCompactness = sumSingletonChecks();
    int pRoomStability = sumExtraRoomsUsed();
    int pTotal = 
      config.getWeights(model.definition.type).wMinDays * pMinDays + 
      config.getWeights(model.definition.type).wCompactness * pCompactness + 
      config.getWeights(model.definition.type).wRoomCapacity * pRoomCapacity + 
      config.getWeights(model.definition.type).wRoomStability * pRoomStability;

    log << "<solution submodelCost='" << getObjValue() << "' ";
    log << "cost='" << pTotal << "' ";
    log << "data='" << config.path << "' ";
    log << "type='" << model.definition.type << "' ";
    log << "discovered='" << config.elapsed() << "' ";
    log << "neighbourhoodLB='" << getBestObjValue() << "' ";
    log << "penaltyRoomCapacity='" << pRoomCapacity << "' ";
    log << "penaltyMinCourseDays='" << pMinDays << "' ";
    log << "penaltyCompactness='" << pCompactness << "' ";
    log << "penaltyRoomStability='" << pRoomStability << "' ";

    log << ">\n";

    int c, p, r;  // course, period, room
    for(c = 0; c < model.instance.getCourseCount(); c++)
      for(p = 0; p < model.instance.getPeriodCount(); p++) {
        int day = (int)std::floor(float(p / model.instance.getPeriodsPerDayCount()));
        int periodWithin = p % model.instance.getPeriodsPerDayCount();
        for(r = 0; r < model.instance.getRoomCount(model.definition.type); r++)
          try {
            if (roundProperly(getValue(model.vars.x[p][r][c])) == 1) {
              log << "<session course='" << model.instance.getCourse(c).name;
              log << "' room='" << model.instance.getRoomName(r);
              log << "' period='" << p;
              log << "' day='" << day;
              log << "' periodWithin='" << periodWithin;
              log << "'/> ";
              sol << model.instance.getCourse(c).name << " ";
              sol << model.instance.getRoomName(r) << " ";
              sol << day << " " << periodWithin << "\n";
            }
          } catch (IloAlgorithm::NotExtractedException e)  { std::cerr << "Error retrieving " << model.vars.x[p][r][c].getName() << "." << std::endl; }
            catch (...)  { std::cerr << "Unknown error retrieving " << model.vars.x[p][r][c].getName() << "." << std::endl; throw; }
          
      }

    log << "</solution>\n";

    env.out() << "</cplex>\n" << log.str();
    env.out() << "\n<cplex>" << std::endl;

    std::stringstream xmlname;
    xmlname << config.path << ".sol" << pTotal << ".xml";
    std::ofstream solFile(xmlname.str().c_str(), ios::out);
    solFile << log.str() << std::endl;
    solFile.close();

    std::stringstream solname;
    solname << config.path << ".sol" << pTotal << ".out";
    solFile.open(solname.str().c_str(), ios::out);
    solFile << sol.str() << std::endl;
    solFile.close();

    config.addSolutionCost(model.definition.type, pTotal);
  }
  catch (IloException& e) { env.error() << "Concert error: " << e << std::endl; }
  catch (...) { env.error() << "Unknown error: " << std::endl; throw; }
}

void SolutionSavingStrategyI::finishOff() {
}


/*** Solution Polishing Strategy ************************************/

IloCplex::Callback SolutionPolishingStrategy(IloEnv &env, Model& s, Config &configuration) {
  return (IloCplex::Callback(new (env) SolutionPolishingStrategyI(env, s, configuration)));
}

void SolutionPolishingStrategyI::main() {
  config.addSolutionCost(model.definition.type, getObjValue());
  fixPeriodNeighbs.push_back(getFixPeriodNeighbourhood());
}

void SolutionPolishingStrategyI::finishOff() {
  int counter = 0;
  while (   !fixPeriodNeighbs.empty() 
         && counter < config.getCount(FixPeriodDiveFromFixDay)) {
    runDiver(fixPeriodNeighbs.back());
    fixPeriodNeighbs.pop_back();
    counter += 1;
  }
}