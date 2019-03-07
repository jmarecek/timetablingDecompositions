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


#include <ctime>
#include <iostream>
#include <fstream>

#include <ilcplex/ilocplex.h>

#include "solver_config.h"


ILOSTLBEGIN

namespace Udine {

class Solver {
public:
  Solver(IloEnv &env, Config &config);
  virtual ~Solver() { }
};

}
