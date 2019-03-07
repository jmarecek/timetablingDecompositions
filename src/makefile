SYSTEM = x86_rhel4.0_3.4
LIBFORMAT = static_pic

CCC = g++

# System/lib-format specific compile/link flags
CSYSFLAGS  = -fPIC -DIL_STD 
LDSYSFLAGS = 

# Compilation and linking flags enabling Multi-threading
CMTFLAGS = -DILOUSEMT -D_REENTRANT
LDMTFLAGS = -lpthread

# Code optimization/debugging options
DEBUG = -O3 -DNDEBUG
#DEBUG = -ggdb -O0

CONCERTDIR=/usr/local/concert22
CPLEXDIR=/usr/local/cplex100

#CONCERTDIR=/home/jxm/concert25
#CPLEXDIR=/home/jxm/cplex110

CFLAGS = $(CSYSFLAGS) $(DEBUG) -I$(CONCERTDIR)/include -I$(CPLEXDIR)/include \
	-I$./boost $(OPTIONS)  

LDFLAGS = \
	-L$(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT) \
		-lilocplex -lcplex \
	-L$(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT) \
		-lconcert -ldl -lpthread \
	$(LDSYSFLAGS) \
	$(LDOPTIONS)

#---------------------------------------------------------
# FILES
#---------------------------------------------------------

TARGET = udine 

execute: $(TARGET)
	./udine ./data/trivial.ctt

build: $(TARGET)

clean:
	/bin/rm -rf *.o
	/bin/rm -rf $(TARGET)
	touch out.out
	/bin/rm -rf *.out

loader.o: ./loader.cpp
	$(CCC) $(CFLAGS) ./loader.cpp -c
solver.o: ./solver.cpp
	$(CCC) $(CFLAGS) ./solver.cpp -c
solver_config.o: ./solver_config.cpp
	$(CCC) $(CFLAGS) ./solver_config.cpp -c
conflicts.o: ./conflicts.cpp
	$(CCC) $(CFLAGS) ./conflicts.cpp -c
strategy.o: ./strategy.cpp
	$(CCC) $(CFLAGS) ./strategy.cpp -c
model.o: ./model.cpp
	$(CCC) $(CFLAGS) ./model.cpp -c
cuts.o: ./cuts.cpp
	$(CCC) $(CFLAGS) ./cuts.cpp -c
saver_bound.o: ./saver_bound.cpp
	$(CCC) $(CFLAGS) ./saver_bound.cpp -c
test01.o: ./test01.cpp
	$(CCC) $(CFLAGS) ./test01.cpp -c
udine: conflicts.o loader.o strategy.o solver_config.o model.o cuts.o saver_bound.o solver.o test01.o
	$(CCC) $(CFLAGS) conflicts.o loader.o strategy.o solver_config.o model.o cuts.o saver_bound.o solver.o test01.o -o udine $(LDFLAGS)
