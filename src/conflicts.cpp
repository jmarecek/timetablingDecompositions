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


#include <cassert>
#include <vector>
#include <set>
#include <algorithm>

#include "conflicts.h"

using namespace Udine;

void Graph::generateConflictGraph(Instance &instance) {
  vs.clear();
  es.clear();
  cliques.clear();

  int i;
  Vertex fresh;
  for (i = 0; i < instance.getCourseCount(); i++)
    vs.push_back(fresh);
  for (i = 0; i != instance.getCurriculumCount(); i++) {
    CourseIds::const_iterator it1 = instance.getCurriculum(i).courseIds.begin();
    for (; it1 != instance.getCurriculum(i).courseIds.end(); it1++) {
      CourseIds::const_iterator it2 = it1;
      std::advance(it2, 1);
      for (; it2 != instance.getCurriculum(i).courseIds.end(); it2++) {
        Edge e(*it1, *it2);
        es.push_back(e);
        vs.at(*it1).adj.insert(*it2);
        vs.at(*it2).adj.insert(*it1);
      }
    }
  }

  std::cout << "Graphs: Generated a conflict graph with " << vs.size();
  std::cout << " vertices and " << es.size() << " edges" << std::endl;
}  // END of Graph::generateConflictGraph


typedef std::pair< int, std::vector<int> > Candidate;
struct CandidateLess {
  bool operator()(const Candidate &x, const Candidate &y ) {
    return (x.second.size() > y.second.size());
  }
};


void Graph::generateCliques() {
  cliques.clear();

  // the size of the largest clique not to pre-generate
  // int minLimit = 3;
  int minLimit = 2;

  // for all vertices
  int u = 0;
  for (u = 0; u < vs.size(); u++) {

    if (vs[u].adj.size() < minLimit) continue;

    std::vector<Candidate> cands;
    for (std::set<int>::iterator vi = vs[u].adj.begin(); vi != vs[u].adj.end(); vi++) {
      std::vector<int> intersection;
      std::set_intersection(vs[u].adj.begin(), vs[u].adj.end(),
                            vs[*vi].adj.begin(), vs[*vi].adj.end(),
                            std::back_inserter(intersection));
      if (u > (*vi) && intersection.size() > minLimit) {
        Candidate cand(*vi, intersection);
        cands.push_back(cand);
      }
    }

    if (cands.size() < minLimit) continue;

    CandidateLess CandidateLessInstance;
    std::sort(cands.begin(), cands.end(), CandidateLessInstance);

    std::vector<int> clique;
    for(int candi = 0; candi < cands.size(); candi++)
      if (std::includes(cands[candi].second.begin(), cands[candi].second.end(), clique.begin(), clique.end()))
        clique.push_back(cands[candi].first);

    if (clique.size() < minLimit) continue;

    clique.push_back(u);
    cliques.push_back(clique);
  }

  std::cout << "Graphs: Pre-generated " << cliques.size() << " cliques" << std::endl;
} // END Graph::generateCliques
