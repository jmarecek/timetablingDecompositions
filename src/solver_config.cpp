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


#include "solver_config.h"

#include <ilcplex/ilocplex.h>

using namespace Udine;

Config::Config(IloEnv &env, char *pathToInstance, const char *configIdentifier)
  : path(pathToInstance),
    configId(configIdentifier) {

  globalStart = std::time(NULL);

  strategy = ContractAlgorithm;
  contractTimelimit = 3600;

  for (int m = 0; m < ModelTypeLen; m++) {
    params.push_back(IloCplex::ParameterSet(env));
    weights.push_back(ObjectiveWeights(1, 5, 2, 1));
    anytimeDivingOnset[m] = 0;
  }    

  featureUse[UseSpecialOrderedSets] = false;
  featureUse[UseStaticCliqueCutsAtSurface] = true;
  featureUse[UseStaticCliqueCutsInDives] = true;
  featureUse[UseDisaggregation] = false;
  featureUse[UseStaticImpliedBounds] = true;
  featureUse[UseStaticPatternEnumeration] = false;
  featureUse[UseHeuristicCompactnessAtSurface] = false;
  featureUse[UseHeuristicCompactnessInDayDives] = true;
  featureUse[UseNeighbourhoodLogging] = true;
  featureUse[UseLowerBoundLogging] = true;
  featureUse[UseSolutionLogging] = true;
  featureUse[UseDynamicCutsAtSurface] = false;
  featureUse[UseZeroRoomStability] = false;
  featureUse[UsePreprocessingFriendlyFormulation] = false;
  featureUse[UseAdditionalVariables] = true;
  featureUse[UseLpFilesExport] = false;
  featureUse[UseRoomSoftfixing] = false;
  featureUse[UseMRoomAggegation] = false;
  featureUse[UseSpreadOnlyNeighbourhoods] = false;
  featureUse[UseObjectiveComponents] = true;

  featureUseFrequency[CutsFromPregeneratedCliques] = 5;
  featureUseFrequency[CutsFromPregeneratedPatterns] = 5;
  featureUseFrequency[CutsFromTriangles] = 5;

  featureUseCount[FixDayDiveFromSurface] = 3;
  featureUseCount[FixPeriodDiveFromSurface]= 3;
  featureUseCount[FixPeriodDiveFromFixDay] = 3;

  for (int p = 0; p < ModelTypeLen; p++) {
    params[p].setParam(IloCplex::ObjDif, 1);
    params[p].setParam(IloCplex::WorkMem, 512);
    upperBounds[p] = -1;
  }
}

bool Config::getParam(FeatureUse f) {
  assert(f >= 0 && f < FeatureUseLen);
  return featureUse[f];
}

int Config::getFrequency(FeatureUseFrequency f) {
  assert(f >= 0 && f < FeatureUseFrequencyLen);
  return featureUseFrequency[f];
}

int Config::getCount(FeatureUseCount c) {
  assert(c >= 0 && c < FeatureUseCountLen);
  return featureUseCount[c];
}

ObjectiveWeights Config::getWeights(ModelType t) {
  assert(t >= 0 && t < ModelTypeLen);
  return weights[t];
}

time_t Config::elapsed() {
  return (std::time(NULL) - globalStart);
}

void Config::addSolutionCost(ModelType t, int v) {
  if ((upperBounds[t] < 0) || (v == std::min(upperBounds[t], v))) {
    upperBounds[t] = v;
    params[t].setParam(IloCplex::CutUp, v - 0.5);
    if (t == FixPeriod && (upperBounds[FixDay] < 0) || (v == std::min(upperBounds[FixDay], v))) {
      upperBounds[FixDay] = v;
      params[FixDay].setParam(IloCplex::CutUp, v - 0.5);
    }
  }
}

void Config::apply(IloCplex &cplex, ModelType t) {
  assert(t >= 0 && t < ModelTypeLen);
  cplex.setParameterSet(params[t]);
}

std::ostream & Udine::operator<< (std::ostream &out, ModelType &type) {
  assert(type >= 0 && type < ModelTypeLen);
  switch (type) {
    case Monolithic: out << "Monolithic"; break;
    case Surface: out << "Surface"; break;
    case FixPeriod: out << "FixPeriod"; break;
    case FixDay: out << "FixDay"; break;
  }
  return out;
}
