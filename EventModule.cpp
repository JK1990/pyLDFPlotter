#include "Event.h"

#include <boost/python/module.hpp>
#include <boost/python/numpy.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(EventModule)
{
    boost::python::numpy::initialize();
    class_<Event,boost::noncopyable>("Event",init<char*>())
        .def("GetNEvents",&Event::GetNEvents)
        .def("ReadEvent",&Event::ReadEvent)
        .def("GetEventID",&Event::GetEventID)
        .def("GetGPSSecond",&Event::GetGPSSecond)
        .def("GetStationShowerAxisDistance",&Event::GetStationShowerAxisDistance)
        .def("GetLDF",&Event::GetLDF)
        .def("GetSignals",&Event::GetSignals)
        .def("GetSaturatedSignals",&Event::GetSaturatedSignals)
        .def("GetRecoveredSignals",&Event::GetRecoveredSignals)
        .def("GetAccidentalSignals",&Event::GetAccidentalSignals)
        .def("GetSilentSignals",&Event::GetSilentSignals)
    ;
}
