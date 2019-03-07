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


#ifndef UDINE_MODEL
#define UDINE_MODEL

#include <vector>
#include <set>
#include <map>
#include <utility>

#include <ilcplex/ilocplex.h>

#include "solver_config.h"
#include "loader.h"
#include "conflicts.h"
#include "neighbourhood.h"
#include "variables.h"

ILOSTLBEGIN

namespace Udine {

class Model {

protected:
  Instance &instance;
  IloEnv env;
  IloModel model;
  Variables vars;
  IloRangeArray constraints;
  IloExpr obj;
  Graph conflictGraph;
  Neighbourhood &definition;
  Config &config;

  // Helpers
  virtual void generateHardConstraints();
  virtual void generateMinCourseDays();
  virtual void generateCompactness();
  virtual void generateRoomStability();
  virtual void generateCoursePeriods();
  virtual void generateNeighbourhood();
  virtual void generateObjectiveComponents();
  virtual void generateObjective();

public:
  Model(IloModel &modelToGenerate, Config &c, Instance &i, Graph &g, Neighbourhood &def) 
    : vars(modelToGenerate.getEnv(), c, i, def), constraints(modelToGenerate.getEnv()), obj(modelToGenerate.getEnv()),
      env(modelToGenerate.getEnv()), model(modelToGenerate), instance(i), conflictGraph(g), definition(def), config(c) {

      // (HARD) Add all hard constraints
      generateHardConstraints();

      // (SOFT) Add all three soft constraints
      generateRoomStability();
      if (def.type != FixPeriod)
        generateCompactness();
      if (def.type == Surface || def.type == Monolithic)
        generateMinCourseDays();

      // (AUX) Add some bells & whistles
      if (def.type != Surface && config.getParam(UseAdditionalVariables))
        generateCoursePeriods();

      // (NEI) Outline the neighbourhood
      if (def.type == FixPeriod || def.type == FixDay)
        generateNeighbourhood();
      
      if (config.getParam(UseObjectiveComponents)) generateObjectiveComponents();
      else generateObjective();

      model.add(constraints);
      model.add(IloMinimize(env, obj));
  }
  virtual ~Model() {}

  friend class StrategyI;
  friend class CutManagerI;
  friend class SolutionSavingStrategyI;
  friend class SolutionPolishingStrategyI;
};

}

#endif // UDINE MODEL
