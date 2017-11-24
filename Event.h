#pragma once

#include "RecEventFile.h"
#include "RecEvent.h"
#include "SDEvent.h"
#include "DetectorGeometry.h"

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/handle.hpp>
#include <TF1.h>

using namespace std;
namespace np = boost::python::numpy;

class Event: boost::noncopyable
//class Event
{
    public:
        Event(char* inFileName);
        ~Event();

        unsigned int GetNEvents();
        void ReadEvent(unsigned int n);
        
        unsigned int GetEventID();
        unsigned int GetGPSSecond();
        
        double GetStationShowerAxisDistance(int id);

        /* Returns the value of the fitted LDF
         * at the given points x
         */
        np::ndarray GetLDF(np::ndarray x);

        /* Functions to get the different types of signals
         * All functions return numpy array of size (4,nSignals)
         * The for arrays are:
         * 1. distance to shower core
         * 2. error on distance to shower core
         * 3. signal in VEM
         * 4. error on signal
         */
        np::ndarray GetSignals();
        np::ndarray GetSaturatedSignals();
        np::ndarray GetRecoveredSignals();
        np::ndarray GetAccidentalSignals();
        np::ndarray GetBadSilentSignals();
        np::ndarray GetGoodSilentSignals();
    
    private:
        RecEventFile recEventFile;
        RecEvent* recEvent;

        DetectorGeometry* detectorGeometry;
        unsigned int nEvents;
};
