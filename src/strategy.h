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


#ifndef UDINE_STRATEGY
#define UDINE_STRATEGY

#include <cmath>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include <ilcplex/ilocplex.h>

#include "neighbourhood.h"
#include "model.h"

ILOSTLBEGIN

namespace Udine {

class StrategyI : public IloCplex::IncumbentCallbackI {

protected:
  IloEnv env;
  Config &config;
  Model &model;
  // NOTE: Model includes "definition"

public:
  StrategyI(IloEnv &environment, Model& s, Config &configuration)
  : IloCplex::IncumbentCallbackI(environment), env(environment), model(s), config(configuration) {}

  virtual int sumMissingCourseDays();
  virtual int sumSingletonChecks();
  virtual int sumMissingSeats();
  virtual int sumExtraRoomsUsed();

  virtual void logNeighbrouhood(Neighbourhood def);
  virtual Neighbourhood getFixDayNeighbourhood();
  virtual Neighbourhood getFixPeriodNeighbourhood();
  virtual void runDiver(Neighbourhood definition);
  virtual void main() = 0;
  virtual void finishOff() = 0;

};


/*** Anytime Strategy *************************************/

class AnytimeStrategyI : public StrategyI {
public:
  ILOCOMMONCALLBACKSTUFF(AnytimeStrategy)
  AnytimeStrategyI(IloEnv &environment, Model& s, Config &configuration)
  : StrategyI(environment, s, configuration) { 
  }
  virtual void main();
  virtual void finishOff();
};
IloCplex::Callback AnytimeStrategy(IloEnv &env, Model& s, Config &config);


/*** Contract Strategy ************************************/

class ContractStrategyI : public StrategyI {
protected:
  Neighbourhoods fixDayNeighbs;
  Neighbourhoods fixPeriodNeighbs;
public:
  ILOCOMMONCALLBACKSTUFF(ContractStrategy)
  ContractStrategyI(IloEnv &environment, Model& s, Config &configuration)
  : StrategyI(environment, s, configuration) { 
  }
  virtual void main();
  virtual void finishOff();
};
IloCplex::Callback ContractStrategy(IloEnv &env, Model& s, Config &config);


/*** Solution Saving Strategy ************************************/

class SolutionSavingStrategyI : public StrategyI {
public:
  ILOCOMMONCALLBACKSTUFF(SolutionSavingStrategy)
  SolutionSavingStrategyI(IloEnv &environment, Model& s, Config &configuration)
  : StrategyI(environment, s, configuration) { 
  }
  virtual void main();
  virtual void finishOff();
};
IloCplex::Callback SolutionSavingStrategy(IloEnv &env, Model& s, Config &config);

/*** Solution Polishing Strategy ************************************/

class SolutionPolishingStrategyI : public StrategyI {
protected:
  Neighbourhoods fixPeriodNeighbs;
public:
  ILOCOMMONCALLBACKSTUFF(SolutionPolishingStrategy)
  SolutionPolishingStrategyI(IloEnv &environment, Model& s, Config &configuration)
  : StrategyI(environment, s, configuration) { 
  }
  virtual void main();
  virtual void finishOff();
};
IloCplex::Callback SolutionPolishingStrategy(IloEnv &env, Model& s, Config &config);

} // END of namespace

#endif // UDINE STRATEGY
