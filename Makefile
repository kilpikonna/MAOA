#------------------------------------------------------------
#
# Please define an environnement variable PATHTUTOMIP
# equal to the XXX directory where Lemon and Graphviz have been installed
# In order to these:
# edit ~/.bashrc add "export PATHTUTOMIP=XXX"
# close your xterm and star new ones
#
#------------------------------------------------------------

LEMONDIR	= $(PATHTUTOMIP)/lemon-1.3.1

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

LEMONLIBDIR	= $(LEMONDIR)/lib
CCLNDIRS  = -L$(LEMONLIBDIR)

LEMONINCDIR   = $(LEMONDIR)/include
CCFLAGS = -I$(LEMONINCDIR)

CCLNFLAGS = -lemon
   # Joke: as the librairy lemon is named libemon,
   # linking lemon librairy is just-lemon

PLNE_partie1: Graph.o PLNE_partie1.o
	g++  $(CCFLAGS) $(CCLNDIRS) PLNE_partie1.o Graph.o -o PLNE_partie1 $(CCLNFLAGS)  

PLNE_partie1.o : PLNE_partie1.cpp
	g++ -c $(CCFLAGS) $(CCLNDIRS) PLNE_partie1.cpp 

Graph.o: Graph.cpp Graph.h
	g++ -c $(CCFLAGS) $(CCLNDIRS) Graph.cpp

clean:
	rm -f *.o 
