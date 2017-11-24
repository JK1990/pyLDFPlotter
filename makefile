
#location of the Python header files
 
PYTHON_VERSION = 2.7
PYTHON_INCLUDE = $(HOME)/Software/python/include/python$(PYTHON_VERSION)
PYTHON_LIB = $(HOME)/Software/python/lib/
 
TARGET = EventModule

all: $(TARGET).so

$(TARGET).so: $(TARGET).o Event.o utl.o
	g++ -shared -Wl,--export-dynamic $(TARGET).o Event.o utl.o $(shell auger-offline-config --ldflags) -lboost_python -lboost_numpy -L$(PYTHON_LIB) -lpython$(PYTHON_VERSION) -std=c++11 -o $(TARGET).so

Event.o: Event.cpp Event.h
	g++ -I$(PYTHON_INCLUDE) $(shell auger-offline-config --cppflags) -std=c++11 -c Event.cpp

$(TARGET).o: $(TARGET).cpp
	g++ -I$(PYTHON_INCLUDE) $(shell auger-offline-config --cppflags) -std=c++11 -c $(TARGET).cpp

utl.o: utl.cpp utl.h
	g++ -I$(PYTHON_INCLUDE) $(shell auger-offline-config --cppflags) -std=c++11 -c utl.cpp 
