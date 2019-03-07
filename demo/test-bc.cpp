#include <iostream>
#include <exception>

#include <ilcplex/ilocplex.h>

#include "loader.h"
#include "conflicts.h"
#include "model.h"
#include "cuts.h"
#include "solver_config.h"

ILOSTLBEGIN

using namespace Udine;

int main (int argc, char **argv) {

  if (argc != 2) { 
    std::cerr << "Usage: " << argv[0] << " <data> " << std::endl;
    std::cerr << "where: <data> is a path to an instance of Udine Timetabling " << std::endl;
    exit(-1); 
  }

  IloEnv env;
  
  Udine::Config config(env, argv[1], "Branch-and-cut");
  config.params[Monolithic].setParam(IloCplex::TiLim, 30*60);
  // config.params[Monolithic].setParam(IloCplex::MIPEmphasis, 1);
  config.params[Monolithic].setParam(IloCplex::RootAlg, 1);
  config.params[Monolithic].setParam(IloCplex::Symmetry, 2);
#if CPX_VERSION >= 1100
  config.params[Monolithic].setParam(IloCplex::Symmetry, 3);
#endif
  config.params[Monolithic].setParam(IloCplex::MIPDisplay, 4); // Display LP details at root node
  config.params[Monolithic].setParam(IloCplex::MIPInterval, 1); 
  config.featureUse[UseLpFilesExport] = true;
  config.featureUse[UseZeroRoomStability] = false;
  config.featureUse[UseAdditionalVariables] = true;
  config.featureUse[UseRoomSoftfixing] = false;
  config.featureUse[UseMRoomAggegation] = true;

  try {
    IloModel model(env);
    IloCplex cplex(model);
    config.apply(cplex, Monolithic);

    Instance instance;
    instance.load(config.path);
    Graph conflictGraph;
    conflictGraph.generateConflictGraph(instance);
    conflictGraph.generateCliques();
    Neighbourhood definition;
    definition.type = Monolithic;
    definition.cost = 0;
    Model generator(model, config, instance, conflictGraph, definition);
    // cplex.use(CutManager(env, generator, config));
    cplex.solve();

  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  } catch (IloException &e) { 
    std::cerr << "Concert error: " << e << std::endl;
  } catch (...) {
    std::cerr << "Unknown error." << std::endl;
  }

#if _WIN32
  int i; std::cin >> i;
#endif

  env.end(); 
  return 0;
}
