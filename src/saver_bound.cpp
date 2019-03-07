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


#include "saver_bound.h"

using namespace Udine;

IloCplex::Callback Udine::BoundSaver(IloEnv &env, Udine::Config &configuration, const char *tagToCreate) {
  return (IloCplex::Callback(new (env) Udine::BoundSaverI(env, configuration, tagToCreate)));
}

void BoundSaverI::main() {
  if (fabs(getBestObjValue() - previousBest) > 0.01) {
    env.out() << "</cplex>" << std::endl;
    env.out() << "<" << tag << " value='" << getBestObjValue() << "' discovered='" << config.elapsed() << "' />";
    env.out() << "<cplex>" << std::endl;
    previousBest = getBestObjValue();
  }
}
