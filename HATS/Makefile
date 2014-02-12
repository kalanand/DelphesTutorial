DEL           = $(shell echo $(DELPHES))
CXX           = g++
CXXFLAGS      = -Wno-deprecated $(shell root-config --cflags) -I../$(DEL)/external/ -I../$(DEL) -std=c++0x 
LDFLAGS       = 
LD            = g++

LIBS = $(shell root-config --glibs)
LIBS += -L../$(DEL) -lDelphes


PROGRAM       = DelHATS
OBJS          = $(PROGRAM).o

ifneq (, $(findstring cmslpc, $(shell hostname)))
  CPPFLAGS += -I/uscmst1/prod/sw/cms/slc5_amd64_gcc462/external/boost/1.50.0-cms2/include
endif

all:            $(PROGRAM)


%.o : %.C $(HEADERS)


%.o : %.cc $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<


$(PROGRAM):     $(OBJS)
	@echo "Linking $(PROGRAM) ..."
	@$(LD) $(OBJS) $(LIBS) -o $(PROGRAM)
	@echo "done"


clean:;         @rm -f $(OBJS) $(PROGRAM) core 

