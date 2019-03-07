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


#ifndef UDINE_CONFLICT_GRAPH
#define UDINE_CONFLICT_GRAPH

#include <set>
#include <vector>
#include <utility>

#include "loader.h"

namespace Udine {

class Vertex {
public:
  std::set<int> adj;        // ajacent vertices
  int c;                       // color
  Vertex() { c = -1; }         // constructor
};

typedef std::pair<int, int> Edge;


class Graph {
  public:
  std::vector<Vertex> vs;
  std::vector<Edge> es;
  std::vector< std::vector<int> > cliques;
  public:
  virtual void generateConflictGraph(Instance &i);
  virtual void generateCliques();
  virtual ~Graph() {};
};

}

#endif // UDINE_CONFLICT_GRAPH
