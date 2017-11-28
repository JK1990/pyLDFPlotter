#include "Event.h"
#include "utl.h"

#include "RecEventFile.h"
#include "RecEvent.h"
#include "SDEvent.h"
#include "DetectorGeometry.h"
#include "LDF.h"

#include <iostream>

#include <TF1.h>
#include <TVector3.h>

#include <boost/python/numpy.hpp>

using namespace std;
namespace np = boost::python::numpy;
namespace p = boost::python;

inline double StationSignalError(double signal) {return 1.06*sqrt(signal);}

Event::Event(char* inFileName) : 
    recEventFile(inFileName),
    recEvent(0),
    detectorGeometry(0),
    nEvents(0)
{
    recEventFile.SetBuffers(&recEvent);
    detectorGeometry = new DetectorGeometry();

    if(recEventFile.ReadDetectorGeometry(*detectorGeometry) != RecEventFile::eSuccess)exit(EXIT_FAILURE);

    nEvents = recEventFile.GetNEvents();
}

Event::~Event(){
    recEventFile.Close();
    delete detectorGeometry;
}

unsigned int Event::GetNEvents(){
    return nEvents;
}

void Event::ReadEvent(unsigned int n){
    recEventFile.ReadEvent(n);
}

unsigned int Event::GetEventID(){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    return sdEvent.GetEventId();
}

unsigned int Event::GetGPSSecond(){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    return sdEvent.GetGPSSecond();
}

double Event::GetStationShowerAxisDistance(int id){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    const SdRecShower& sdRecShower = sdEvent.GetSdRecShower();
    const TVector3& showerCore = sdRecShower.GetCoreSiteCS();
    const TVector3& showerAxis = sdRecShower.GetAxisSiteCS();

    return detectorGeometry->GetStationAxisDistance(id,showerAxis,showerCore);
}

np::ndarray Event::GetLDF(np::ndarray x){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    const SdRecShower& sdRecShower = sdEvent.GetSdRecShower();
    const LDF& ldf = sdRecShower.GetLDF();

    p::tuple shape = p::make_tuple(2,x.shape(0));
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray result = np::zeros(shape,dtype);

    for (int i = 0;i<x.shape(0);i++) {
        const double funcval = ldf.Evaluate(p::extract<double>(x[i]),eNKG);
        result[0][i] = funcval;
        result[1][i] = sdRecShower.SignalUncertainty()*sqrt(funcval);
    }

    return result;
}

np::ndarray Event::GetSignals(){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    const SdRecShower& sdRecShower = sdEvent.GetSdRecShower();
    const LDF& ldf = sdRecShower.GetLDF();
    const vector<SdRecStation>& stations = sdEvent.GetStationVector();

    vector<double> signal;
    vector<double> signalErr;
    vector<double> distance;
    vector<double> distanceErr;

    for ( unsigned int iS = 0; iS<stations.size(); iS++){
        if ( stations[iS].IsCandidate() && !stations[iS].IsLowGainSaturated() && stations[iS].GetLDFResidual()!=0) {
            distance.push_back(stations[iS].GetSPDistance());
            distanceErr.push_back(stations[iS].GetSPDistanceError());

            signal.push_back(stations[iS].GetTotalSignal());
            signalErr.push_back(stations[iS].GetTotalSignalError() ?
                                stations[iS].GetTotalSignalError() : StationSignalError(stations[iS].GetTotalSignal()));
        }
    }

    p::tuple shape = p::make_tuple(4,signal.size());
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray result = np::zeros(shape,dtype);
    
    result[0] = vector_to_ndarray(distance);
    result[1] = vector_to_ndarray(distanceErr);
    result[2] = vector_to_ndarray(signal);
    result[3] = vector_to_ndarray(signalErr);

    return result;
}

np::ndarray Event::GetSaturatedSignals(){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    const SdRecShower& sdRecShower = sdEvent.GetSdRecShower();
    const LDF& ldf = sdRecShower.GetLDF();
    const vector<SdRecStation>& stations = sdEvent.GetStationVector();

    vector<double> signal;
    vector<double> signalErr;
    vector<double> distance;
    vector<double> distanceErr;

    for ( unsigned int iS = 0; iS<stations.size(); iS++){
        if ( stations[iS].IsCandidate() && stations[iS].IsLowGainSaturated()) {
            distance.push_back(stations[iS].GetSPDistance());
            distanceErr.push_back(stations[iS].GetSPDistanceError());

            signal.push_back(stations[iS].GetTotalSignal());
            signalErr.push_back(stations[iS].GetTotalSignalError() ?
                                stations[iS].GetTotalSignalError() : StationSignalError(stations[iS].GetTotalSignal()));
        }
    }

    p::tuple shape = p::make_tuple(4,signal.size());
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray result = np::zeros(shape,dtype);
    
    result[0] = vector_to_ndarray(distance);
    result[1] = vector_to_ndarray(distanceErr);
    result[2] = vector_to_ndarray(signal);
    result[3] = vector_to_ndarray(signalErr);

    return result;
}

np::ndarray Event::GetRecoveredSignals(){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    const SdRecShower& sdRecShower = sdEvent.GetSdRecShower();
    const LDF& ldf = sdRecShower.GetLDF();
    const vector<SdRecStation>& stations = sdEvent.GetStationVector();

    vector<double> signal;
    vector<double> signalErr;
    vector<double> distance;
    vector<double> distanceErr;

    for ( unsigned int iS = 0; iS<stations.size(); iS++){
        if ( stations[iS].IsCandidate() && stations[iS].IsLowGainSaturated() && stations[iS].GetRecoveredSignal()!=0) {
            distance.push_back(stations[iS].GetSPDistance());
            distanceErr.push_back(stations[iS].GetSPDistanceError());

            signal.push_back(stations[iS].GetRecoveredSignal());
            signalErr.push_back(stations[iS].GetRecoveredSignalError());
        }
    }

    p::tuple shape = p::make_tuple(4,signal.size());
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray result = np::zeros(shape,dtype);
    
    result[0] = vector_to_ndarray(distance);
    result[1] = vector_to_ndarray(distanceErr);
    result[2] = vector_to_ndarray(signal);
    result[3] = vector_to_ndarray(signalErr);

    return result;
}

np::ndarray Event::GetAccidentalSignals(){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    const SdRecShower& sdRecShower = sdEvent.GetSdRecShower();
    const LDF& ldf = sdRecShower.GetLDF();
    const vector<SdRecStation>& stations = sdEvent.GetStationVector();

    vector<double> signal;
    vector<double> signalErr;
    vector<double> distance;
    vector<double> distanceErr;

    for ( unsigned int iS = 0; iS<stations.size(); iS++){
        const double dist = stations[iS].GetSPDistance();
        if ( stations[iS].IsAccidental() && 0 < dist && dist < 5e3 && !stations[iS].IsDense()) {
            distance.push_back(dist);
            distanceErr.push_back(0.);

            signal.push_back(stations[iS].GetTotalSignal());
            signalErr.push_back(stations[iS].GetTotalSignalError() ?
                                stations[iS].GetTotalSignalError() : StationSignalError(stations[iS].GetTotalSignal()));
        }
    }

    p::tuple shape = p::make_tuple(4,signal.size());
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray result = np::zeros(shape,dtype);
    
    result[0] = vector_to_ndarray(distance);
    result[1] = vector_to_ndarray(distanceErr);
    result[2] = vector_to_ndarray(signal);
    result[3] = vector_to_ndarray(signalErr);

    return result;
}

np::ndarray Event::GetBadSilentSignals(){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    const Detector& det = recEvent->GetDetector();
    const vector<SdBadStation>& badStations = sdEvent.GetBadStationVector();
    TBits array = det.GetActiveStations();

    vector<double> signal;
    vector<double> signalErr;
    vector<double> distance;
    vector<double> distanceErr;

    for (size_t i=0;i<badStations.size();++i) {
        if ( badStations[i].GetReason() != eBadSilent )
            array[badStations[i].GetId()] = false;
    }

    for (DetectorGeometry::StationPosMapConstIterator iStation = detectorGeometry->GetStationsBegin();
            iStation!=detectorGeometry->GetStationsEnd();
            ++iStation){
        if ( array.TestBitNumber(iStation->first)){
            const int id = iStation->first;
            const double dist = GetStationShowerAxisDistance(id);
            if ( dist < 10000. && !sdEvent.HasStation(id)){
                if (sdEvent.HasBadStation(id) && (
                            sdEvent.GetBadStationById(id)->GetReason() == eBadSilent ||
                            sdEvent.GetBadStationById(id)->GetReason() == eNotAliveT2 ||
                            sdEvent.GetBadStationById(id)->GetReason() == eNotAliveT120)){
                    signal.push_back(3.);
                    signalErr.push_back(0.);
                    distance.push_back(dist);
                    distanceErr.push_back(0.);
                }
            }
        }
    }

    p::tuple shape = p::make_tuple(4,signal.size());
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray result = np::zeros(shape,dtype);
    
    result[0] = vector_to_ndarray(distance);
    result[1] = vector_to_ndarray(distanceErr);
    result[2] = vector_to_ndarray(signal);
    result[3] = vector_to_ndarray(signalErr);

    return result;
}

np::ndarray Event::GetGoodSilentSignals(){
    const SDEvent& sdEvent = recEvent->GetSDEvent();
    const Detector& det = recEvent->GetDetector();
    const vector<SdBadStation>& badStations = sdEvent.GetBadStationVector();
    TBits array = det.GetActiveStations();

    vector<double> signal;
    vector<double> signalErr;
    vector<double> distance;
    vector<double> distanceErr;

    for (size_t i=0;i<badStations.size();++i) {
        if ( badStations[i].GetReason() != eBadSilent )
            array[badStations[i].GetId()] = false;
    }

    for (DetectorGeometry::StationPosMapConstIterator iStation = detectorGeometry->GetStationsBegin();
            iStation!=detectorGeometry->GetStationsEnd();
            ++iStation){
        if ( array.TestBitNumber(iStation->first)){
            const int id = iStation->first;
            const double dist = GetStationShowerAxisDistance(id);
            if ( dist < 10000. && !sdEvent.HasStation(id)){
                if (sdEvent.HasBadStation(id) && (
                            sdEvent.GetBadStationById(id)->GetReason() == eBadSilent ||
                            sdEvent.GetBadStationById(id)->GetReason() == eNotAliveT2 ||
                            sdEvent.GetBadStationById(id)->GetReason() == eNotAliveT120)){
                    continue;
                } else {
                    signal.push_back(3.);
                    signalErr.push_back(0.);
                    distance.push_back(dist);
                    distanceErr.push_back(0.);
                }
            }
        }
    }

    p::tuple shape = p::make_tuple(4,signal.size());
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray result = np::zeros(shape,dtype);
    
    result[0] = vector_to_ndarray(distance);
    result[1] = vector_to_ndarray(distanceErr);
    result[2] = vector_to_ndarray(signal);
    result[3] = vector_to_ndarray(signalErr);

    return result;
}
