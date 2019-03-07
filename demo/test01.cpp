#include <iostream>
#include <exception>

#include <ilcplex/ilocplex.h>

#include "solver.h"
#include "solver_config.h"

ILOSTLBEGIN

int main (int argc, char **argv) {

  if (argc != 2) { 
    std::cerr << "Usage: " << argv[0] << " <data> " << std::endl;
    std::cerr << "where: <data> is a path to an instance of Udine Timetabling " << std::endl;
    exit(-1); 
  }

  IloEnv env0;
  Udine::Config config0(env0, argv[1], "Tuned-0-15-Bar-Feas-Fract-Sym");

  config0.contractTimelimit = 60*60;
  config0.anytimeDivingOnset[Udine::FixPeriod] = 0;
  config0.anytimeDivingOnset[Udine::FixDay] = 0;

  config0.featureUse[Udine::UseLpFilesExport] = true;
  config0.featureUse[Udine::UseHeuristicCompactnessInDayDives] = false;
  config0.featureUse[Udine::UseStaticCliqueCutsInDives] = true;
  config0.featureUse[Udine::UseZeroRoomStability] = false;
  config0.featureUse[Udine::UseAdditionalVariables] = true;
  config0.featureUse[Udine::UseRoomSoftfixing] = false;
  config0.featureUse[Udine::UseMRoomAggegation] = true;

  config0.params[Udine::Surface].setParam(IloCplex::TiLim, 5*60);
  config0.params[Udine::Surface].setParam(IloCplex::WorkMem, 1024);
  config0.params[Udine::Surface].setParam(IloCplex::RootAlg, 4);
  config0.params[Udine::Surface].setParam(IloCplex::Symmetry, 2);
  config0.params[Udine::Surface].setParam(IloCplex::MIPDisplay, 4); // Display LP details at root node
  config0.params[Udine::Surface].setParam(IloCplex::MIPInterval, 1);
  
  config0.params[Udine::FixPeriod].setParam(IloCplex::TiLim, 1*60);
  config0.params[Udine::FixPeriod].setParam(IloCplex::WorkMem, 1024);
  // config0.params[Udine::FixPeriod].setParam(IloCplex::EpGap, 0.02);
  config0.params[Udine::FixPeriod].setParam(IloCplex::MIPEmphasis, 1);
  config0.params[Udine::FixPeriod].setParam(IloCplex::Symmetry, 2);

  config0.params[Udine::FixDay].setParam(IloCplex::TiLim, 30*60);
  config0.params[Udine::FixDay].setParam(IloCplex::NodeLim, 500);
  config0.params[Udine::FixDay].setParam(IloCplex::WorkMem, 2048);
  // config0.params[Udine::FixDay].setParam(IloCplex::EpGap, 0.02);
  config0.params[Udine::FixDay].setParam(IloCplex::Symmetry, 2);
  config0.params[Udine::FixDay].setParam(IloCplex::MIPInterval, 1);
  config0.params[Udine::FixDay].setParam(IloCplex::MIPDisplay, 4);


#if CPX_VERSION >= 1100
  config0.params[Udine::Surface].setParam(IloCplex::Symmetry, 3);
  config0.params[Udine::FixPeriod].setParam(IloCplex::Symmetry, 3);
  config0.params[Udine::FixDay].setParam(IloCplex::Symmetry, 3);
#endif

  try {
    Udine::Solver solver0(env0, config0);
  } catch (...) {
  }
  env0.end(); 

  return 0;
}
