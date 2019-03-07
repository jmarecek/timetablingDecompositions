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


#ifndef UDINE_SOLVER_CONFIG
#define UDINE_SOLVER_CONFIG

#include <ctime>
#include <vector>
#include <iostream>

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

namespace Udine {

enum SolverType {
  SurfaceSolver = 0,
  DiveSolver = 1
};
const int SolverTypeLen = 2;

enum ModelType {
  Monolithic = 0,
  Surface = 1,
  FixPeriod = 2,
  FixDay = 3
};
const int ModelTypeLen = 4;
std::ostream & operator<< (std::ostream &out, ModelType &t);

enum Strategy {
  AnytimeAlgorithm = 0,
  ContractAlgorithm = 1
};
const int StrategyLen = 2;

enum FeatureUse {
  UseSpecialOrderedSets = 0,
  UseStaticCliqueCutsAtSurface = 1,
  UseStaticCliqueCutsInDives = 2,
  UseDisaggregation = 3,
  UseStaticImpliedBounds = 4,
  UseStaticPatternEnumeration = 5,
  UseHeuristicCompactnessAtSurface = 6,
  UseHeuristicCompactnessInDayDives = 7,
  UseNeighbourhoodLogging = 8,
  UseLowerBoundLogging = 9,
  UseSolutionLogging = 10,
  UseDynamicCutsAtSurface = 11,
  UseZeroRoomStability = 12,
  UsePreprocessingFriendlyFormulation = 13,
  UseAdditionalVariables = 14,
  UseLpFilesExport = 15,
  UseRoomSoftfixing = 16,
  UseMRoomAggegation = 17,
  UseSpreadOnlyNeighbourhoods = 18,
  UseObjectiveComponents = 19,
};
const int FeatureUseLen = 20;

enum FeatureUseFrequency {
  CutsFromPregeneratedCliques = 0,
  CutsFromPregeneratedPatterns = 1,
  CutsFromTriangles = 2,
};
const int FeatureUseFrequencyLen = 3;

enum FeatureUseCount {
  FixDayDiveFromSurface = 0,
  FixPeriodDiveFromSurface = 1,
  FixPeriodDiveFromFixDay = 2,
};
const int FeatureUseCountLen = 3;

struct ObjectiveWeights {
  float wRoomCapacity;
  float wMinDays;
  float wCompactness;
  float wRoomStability;
  ObjectiveWeights(float roomCapacity = 1, float minDays = 5, float compactness = 2, float roomStability = 1) {
      wRoomCapacity = roomCapacity;
      wMinDays = minDays;
      wCompactness = compactness;
      wRoomStability = roomStability;
  }
};


struct Config {

  // some details we want to carry around
  const char *path;
  const char *configId;
  time_t globalStart;

  // configuration for various instances of CPLEX
  std::vector<IloCplex::ParameterSet> params;
  int upperBounds[ModelTypeLen];

  // time that has to elapse before the given type of dives is run (in seconds)
  bool featureUse[FeatureUseLen];
  int featureUseFrequency[FeatureUseFrequencyLen];
  int featureUseCount[FeatureUseCountLen];

  Strategy strategy;

  // strategy specific bits
  float anytimeDivingOnset[ModelTypeLen];
  float contractTimelimit;

  // weights of the individual penalty functions at various types of models
  std::vector<ObjectiveWeights> weights;

public:
  Config(IloEnv &env, char *pathToInstance, const char *configIdentifier);

  // run-time
  time_t elapsed();

  // retrieval of some basics
  bool getParam(FeatureUse f);
  int getFrequency(FeatureUseFrequency f);
  int getCount(FeatureUseCount c);
  ObjectiveWeights getWeights(ModelType s);
  void addSolutionCost(ModelType s, int value);

  // application of params en masse
  void apply(IloCplex &cplex, ModelType instance);
};

}

#endif // UDINE SOLVER CONFIG
