/*!

\mainpage Memos Solvers: Decomposition, Reformulation, and Diving

a project of Jakub Marecek
supervised by Prof. Edmund K. Burke, Dr. Andrew J. Parkes, and Dr. Hana Rudova.

\section An Overview
In many real-life optimisation problems, there are multiple interacting components 
in a solution. For example, different components might specify assignments to different 
kinds of resource. Often, each component is associated with different sets of soft 
constraints, and so with different measures of soft constraint violation. The goal 
is then to minimise a linear combination of such measures.
 
In the multiphase exploitation of multiple objective-/value-restricted submodels 
(MEMOS) approach of Burke et al. (2009), only one computationally difficult component 
of a problem and the associated subset of objectives is considered at first. This 
produces partial solutions, which define interesting neighbourhoods in the search 
space of the complete problem. Often, it is possible to pick the initial component 
so that variable aggregation can be performed at the first stage, and the 
neighbourhoods to be explored next are guaranteed to contain feasible solutions. 
Using integer programming, it is then easy to implement heuristics producing 
solutions with bounds on their quality.

The sample solver is for university course timetabling problem used in the 2007 
International Timetabling Competition, also known as the Udine Course Timetabling 
Problem. The goal is to find an assignment of events to periods and rooms, so 
that the assignment of events to periods is a feasible bounded colouring of an 
associated conflict graph and the linear combination of the numbers of violations 
of four soft constraints is minimised. In the proposed heuristic, an objective-
-restricted neighbourhood generator produces assignments of periods to events, 
with decreasing numbers of violations of two period-related soft constraints. 
Those are relaxed into assignments of events to days, which define neighbourhoods 
that are easier to search with respect to all four soft constraints. Integer 
programming formulations for all subproblems are given and evaluated using ILOG 
CPLEX 11.

\section Links:
http://memos-solvers.sourceforge.net/
http://www.cs.nott.ac.uk/~jxm/timetabling/memos/
http://www.cs.nott.ac.uk/~jxm/timetabling/tr2008.pdf
http://dx.doi.org/10.1016/j.cor.2009.02.023

\section License:
The sample solver is licenced under the GNU Public License (GPL) version 3. It 
would be greatly appreciated, however, if users would cite the following paper 
in any work that uses it:

Edmund K. Burke, Jakub Marecek, Andrew J. Parkes, and Hana Rudova: 
Decomposition, Reformulation, and Diving in University Course Timetabling
Computers and Operations Research, DOI 10.1016/j.cor.2009.02.023.

*/