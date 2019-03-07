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


#include "cuts.h"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <ilcplex/ilocplex.h>

#include "solver_config.h"

using namespace Udine;

IloCplex::Callback Udine::CutManager(IloEnv env, Udine::Model& s, Udine::Config &config) {
  return (IloCplex::Callback(new (env) Udine::CutManagerI(env, s, config)));
}

void CutManagerI::main() {
  int thisTime = 0;

  if (totalCalls >= 2 * (getNnodes() + 1)) return;
  
  std::cout << "Mycuts: Running separation routines ... " << std::endl;

  if (config.getParam(UseHeuristicCompactnessAtSurface)
      && (totalCalls <= 5 * (getNnodes() + 1)))
    thisTime += genCutsFromPatternsWithHeuristicAtSurface();

  if (thisTime > 0)
    std::cout << "Mycuts: Added " << thisTime << " cuts from patterns (node " << getNnodes() << ", call " << totalCalls << ")" << std::endl;

  if (active || totalCalls % config.getFrequency(CutsFromPregeneratedCliques) == 1)
    thisTime += genCutsFromCliquePool();

  if (config.getParam(UseObjectiveComponents)) 
    thisTime += genCutsFromComponents();

  /*
  if ((active || totalCalls % config.getFrequency(CutsFromPregeneratedPatterns) == 1)
      && !config.getParam(UseHeuristicCompactnessAtSurface))
    thisTime += genCutsFromPatterns();
  */

  // if (config.getFrequency(CutsFromTriangles))
  //  thisTime += genCutsFromTriangles(); 

  if (thisTime > 0)
    std::cout << "Mycuts: Added " << thisTime << " cuts in total (node " << getNnodes() << ", call " << totalCalls << ")" << std::endl;

  active = (thisTime > 0);
  totalCalls += 1;
}


int CutManagerI::genCutsFromCliquePool() {

  int cuts = 0;

  int p, clique, ci;  // period, clique and index within
  std::vector< std::vector<int> > &cs = solver.conflictGraph.cliques;
  std::vector<Vertex> &vs = solver.conflictGraph.vs;
  std::vector<Edge> &es = solver.conflictGraph.es;

  float value = 0;       // sum for the clique in the present LP relaxation
  for(p = 0; p < solver.instance.getPeriodCount(); p++)
    for(clique = 0; clique < cs.size(); clique++) {
      IloExpr sum(solver.env);
      for(ci = 0; ci < cs.at(clique).size(); ci++)
        sum += solver.vars.coursePeriods[cs[clique][ci]][p];
      // Do you want to add the cut?
      if (getValue(sum) <= 1) { sum.end(); continue; }
      CliqueCutIdentifier id(p, clique);
      if (cliquePool.find(id) != cliquePool.end()) { sum.end(); continue; }
      IloConstraint cut(sum <= 1);
      addLocal(cut);
      cuts += 1;
      cliquePool[id] = true;
    }

  return cuts;
}


int CutManagerI::genCutsFromComponents() {
  
  int cuts = 0;

  if (getValue(solver.vars.penaltyRoomStability)
    - std::floor(getValue(solver.vars.penaltyRoomStability)) > 0.1) {
      addLocal(solver.vars.penaltyRoomStability >= std::ceil(getValue(solver.vars.penaltyRoomStability)));
      cuts += 1;
  }

  if (getValue(solver.vars.penaltyRoomCapacity)
    - std::floor(getValue(solver.vars.penaltyRoomCapacity)) > 0.1) {
      addLocal(solver.vars.penaltyRoomCapacity >= std::ceil(getValue(solver.vars.penaltyRoomCapacity)));
      cuts += 1;
  }

  if (getValue(solver.vars.penaltyPeriodSingletons)
    - std::floor(getValue(solver.vars.penaltyPeriodSingletons)) > 0.1) {
      addLocal(solver.vars.penaltyPeriodSingletons >= std::ceil(getValue(solver.vars.penaltyPeriodSingletons)));
      cuts += 1;
  }

  if (getValue(solver.vars.penaltyPeriodSpread)
    - std::floor(getValue(solver.vars.penaltyPeriodSpread)) > 0.1) {
      addLocal(solver.vars.penaltyPeriodSpread >= std::ceil(getValue(solver.vars.penaltyPeriodSpread)));
      cuts += 1;
  }

  return cuts;
}


int CutManagerI::genCutsFromTriangles() {

  int cuts = 0;

  std::cout << "Mycuts: Starting cut generation. (Call " << totalCalls << ")" << std::endl;

  std::vector< std::vector<int> > &cs = solver.conflictGraph.cliques;
  std::vector<Vertex> &vs = solver.conflictGraph.vs;
  std::vector<Edge> &es = solver.conflictGraph.es;

  for (int u = 0; u < vs.size(); u++)
    for (std::set<int>::iterator vi = vs[u].adj.begin(); vi != vs[u].adj.end(); vi++)
      if (u < (*vi)) {
        for(int p = 0; p < solver.instance.getPeriodCount(); p++)
          if (getValue(solver.vars.coursePeriods[u][p] + solver.vars.coursePeriods[*vi][p]) >  1.0) {
            std::cout << "Violated for two courses" << std::endl;
          }
        for (std::set<int>::iterator wi = vs[*vi].adj.begin(); wi != vs[*vi].adj.end(); wi++)
          if ((*vi < (*wi)) && (vs[(*wi)].adj.find(u) != vs[(*wi)].adj.end()))
            for(int p = 0; p < solver.instance.getPeriodCount(); p++) {
              IloExpr sum(solver.env);
              sum += solver.vars.coursePeriods[u][p] + 
                     solver.vars.coursePeriods[*vi][p] + 
                     solver.vars.coursePeriods[*wi][p];
              if (getValue(sum) >  1.0 && genCutsFromTriangleNeighbourhood(u, *vi, *wi, p)) continue;
              if (getValue(sum) <= 1.1) continue;  // TODO: improve upon this
              IloConstraint cut(sum <= 1);
              addLocal(cut);
              cuts += 1;
              sum.end();
            }
      }

  return cuts;
}


int CutManagerI::genCutsFromTriangleNeighbourhood(int u, int v, int w, int p) {
  int cuts = 0;

  std::vector< std::vector<int> > &cs = solver.conflictGraph.cliques;
  std::vector<Vertex> &vs = solver.conflictGraph.vs;

  std::vector<int> clique;
  clique.push_back(u);
  clique.push_back(v); 
  clique.push_back(w); 

  std::cout << "Mycuts: Growing from " << u << " " << v  << " " << w << std::endl;
 
  std::vector<int> cliqueUpdates;

  while (1) {
    cliqueUpdates.clear();
    
    // For each element of the clique
    std::vector<int>::iterator cliqueElement = clique.begin();
    for (; cliqueElement != clique.end(); cliqueElement++) {
      // Go through its neighbourhood
      std::set<int>::iterator it = vs[*cliqueElement].adj.begin();
      for (; it != vs[*cliqueElement].adj.end(); it++) {
        if (std::find(clique.begin(), clique.end(), *it) != clique.end()) {
          bool add = true;
  //        for (int ci2 = 0; add && ci2 < clique.size(); ci2++) 
  //          if (vs[clique[ci2]].adj.find(*it) == vs[clique[ci2]].adj.end())
  //            add = false;
          if (add) cliqueUpdates.push_back(*it);
        }
      }
    }

    if (cliqueUpdates.size() == 0) break;
    std::copy(cliqueUpdates.begin(), cliqueUpdates.end(), std::back_inserter(clique));
    std::sort(clique.begin(), clique.end());
  }

  if (clique.size() <= 3) return false;
  std::sort(clique.begin(), clique.end());
  if (std::find(cs.begin(), cs.end(), clique) != cs.end()) return false;
  else cs.push_back(clique);

  IloExpr sum(solver.env);
  for (std::vector<int>::iterator cliqueElement = clique.begin(); cliqueElement != clique.end(); cliqueElement++) 
    sum += solver.vars.coursePeriods[*cliqueElement][p];
  IloConstraint cut(sum <= 1);
  addLocal(cut);
  totalCutsAdded += 1;
  sum.end();

  std::cout << "Mycuts: Found a cut dynamically from a triangle and added it to the pool (from call " << totalCalls << ")" << std::endl;
}


int CutManagerI::genCutsFromPatternsWithHeuristicAtSurface() {

  int cuts = 0;

  int u, ui, d, pd;  // curriculum and index within, day, period within
  int pati;          // pati for index within patterns
  for(u = 0; u < solver.instance.getProperCurriculumCount(); u++)
    for(d = 0; d < solver.instance.getDayCount(); d++) {

      try {
        // Get the current penalties, which are fixed across all patterns
        float rhs = getValue(solver.vars.singletonChecks[u][d][0]);

        // Compute the LHS for each pattern
        for (pati = 0; pati < solver.instance.getPatterns().size(); pati++) {
          float lhs = 0;
          for (pd = 0; pd < solver.instance.getPeriodsPerDayCount(); pd++) {
            IloInt p = d * solver.instance.getPeriodsPerDayCount() + pd;
            float factor = 0;
            for (ui = 0; ui < solver.instance.getCurriculum(u).courseIds.size(); ui++)
              factor += getValue(solver.vars.coursePeriods[solver.instance.getCurriculum(u).courseIds.at(ui)][p]);
            lhs += solver.instance.getPatterns()[pati].coefs[pd] * factor;
          }
          lhs -= solver.instance.getPatterns()[pati].rhs;
          lhs *= solver.instance.getPatterns()[pati].penalty;
          // If the constraint is violated, add the cut
          if (lhs - rhs > 0) { 
            IloExpr lhsExpr(solver.env);
            for (ui = 0; ui < solver.instance.getCurriculum(u).courseIds.size(); ui++) {
              IloInt c = solver.instance.getCurriculum(u).courseIds.at(ui);
              for (pd = 0; pd < solver.instance.getPeriodsPerDayCount(); pd++) {
                lhsExpr += solver.instance.getPatterns()[pati].coefs[pd] * 
                  solver.vars.coursePeriods[c][d * solver.instance.getPeriodsPerDayCount() + pd];
              }
            }
            lhsExpr -= solver.instance.getPatterns()[pati].rhs;
            lhsExpr *= solver.instance.getPatterns()[pati].penalty;
            IloConstraint cut(lhsExpr - solver.vars.singletonChecks[u][d][0] <= 0);
            addLocal(cut);
            lhsExpr.end();
            cuts += 1; 
          }
        }      
        
      } catch (...) {
        std::cerr << "Solver: An exception intercepted." << std::endl;
      }
    }

  return cuts;
}


int CutManagerI::genCutsFromPatterns() {

  int cuts = 0;

  int u, ui, d, pd;  // curriculum and index within, day, period within
  int s, pati;       // s for check, pati for index within patterns
  for(u = 0; u < solver.instance.getProperCurriculumCount(); u++)
    for(d = 0; d < solver.instance.getDayCount(); d++) {
      IloExpr rhsExpr(solver.env);
      try {
        // Get the current penalties, which are fixed across all patterns
        for(s = 0; s < solver.instance.getCheckCount(); s++)
          rhsExpr += solver.vars.singletonChecks[u][d][s];
        float rhs = getValue(rhsExpr);

        // Compute the LHS for each pattern
        for (pati = 0; pati < solver.instance.getPatterns().size(); pati++) {
          float lhs = 0;
          for (pd = 0; pd < solver.instance.getPeriodsPerDayCount(); pd++) {
            IloInt p = d * solver.instance.getPeriodsPerDayCount() + pd;
            float factor = 0;
            for (ui = 0; ui < solver.instance.getCurriculum(u).courseIds.size(); ui++)
              factor += getValue(solver.vars.coursePeriods[solver.instance.getCurriculum(u).courseIds.at(ui)][p]);
            lhs += solver.instance.getPatterns()[pati].coefs[pd] * factor;
          }
          lhs -= solver.instance.getPatterns()[pati].rhs;
          lhs *= solver.instance.getPatterns()[pati].penalty;
          // If the constraint is violated, add the cut
          if (lhs - rhs > 0) { 
            IloExpr lhsExpr(solver.env);
            for (ui = 0; ui < solver.instance.getCurriculum(u).courseIds.size(); ui++) {
              IloInt c = solver.instance.getCurriculum(u).courseIds.at(ui);
              for (pd = 0; pd < solver.instance.getPeriodsPerDayCount(); pd++) {
                lhsExpr += solver.instance.getPatterns()[pati].coefs[pd] * 
                  solver.vars.coursePeriods[c][d * solver.instance.getPeriodsPerDayCount() + pd];
              }
            }
            lhsExpr -= solver.instance.getPatterns()[pati].rhs;
            lhsExpr *= solver.instance.getPatterns()[pati].penalty;
            IloConstraint cut(lhsExpr - rhsExpr <= 0);
            addLocal(cut);
            lhsExpr.end();
            cuts += 1; 
          }
        }      
        
      } catch (...) {
        std::cerr << "Solver: An exception intercepted." << std::endl;
      }
      rhsExpr.end();
    }

  return cuts;
}
