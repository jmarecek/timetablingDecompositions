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


#include "solver.h"

#include "loader.h"
#include "conflicts.h"
#include "model.h"
#include "cuts.h"
#include "strategy.h"
#include "saver_bound.h"

using namespace Udine;

Solver::Solver(IloEnv &env, Config &config) {   
  try {
    env.out() << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
    env.out() << "<log";
    env.out() << " data='" << config.path << "' ";
    env.out() << " config='" << config.configId << "' ";
    env.out() << ">" << std::endl;
    env.out() << "<cplex>";

    IloModel model(env);
    IloCplex cplex(model);
    config.apply(cplex, Surface);

    Instance instance;
    instance.load(config.path);
    Graph conflictGraph;
    conflictGraph.generateConflictGraph(instance);
    conflictGraph.generateCliques();
    Neighbourhood definition;
    definition.type = Surface;
    definition.cost = 0;
    Model surface(model, config, instance, conflictGraph, definition);

    if (config.getParam(UseLpFilesExport)) {
      env.out() << std::endl << "Exporting the surface model ..." << std::endl;
      std::stringstream filename;
      filename << config.path << ".surface.lp";
      cplex.exportModel(filename.str().c_str());
    }

    StrategyI *strategy = NULL;
    switch (config.strategy) {
      case ContractAlgorithm:
        strategy = new (env) ContractStrategyI(env, surface, config);
        break;
      case AnytimeAlgorithm: 
      default:
        strategy = new (env) AnytimeStrategyI(env, surface, config);
        break;
    }
    cplex.use(IloCplex::Callback(strategy));

    if (config.getParam(UseLowerBoundLogging))
      cplex.use(BoundSaver(env, config, "globalLB"));

    if (config.getParam(UseDynamicCutsAtSurface))
      cplex.use(CutManager(env, surface, config));

    env.out() << std::endl << "Solver: Running ..." << std::endl;

#if CPX_VERSION >= 1100
    cplex.populate();
#else
    cplex.solve();
#endif

    if (cplex.getStatus() != IloAlgorithm::Feasible &&
        cplex.getStatus() != IloAlgorithm::Optimal) {
      env.out() << "Solver: Not a single neighbourhood has been found. Please check the time limit and feasibility of the instance." << std::endl;
    }

    env.out() << "</cplex>";
    env.out() << "<timestamp end='" << config.elapsed() << "'/>";

    strategy->finishOff();

    env.out() << "</log>" << std::endl;
  }
  catch (IloException& e) { env.error() << "Solver: Concert exception caught: " << e << std::endl; }
  catch (...) { env.error() << "Solver: Unknown exception caught" << std::endl; }
}
