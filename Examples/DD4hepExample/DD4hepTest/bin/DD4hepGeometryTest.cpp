#include "ACTFW/Examples/DD4hepExample/DD4hepDetector.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"


int main() {
    // create the tracking geometry from DD4hep xml input
    auto trackingGeometry = DD4hepExample::trackingGeometry("file:Examples/DD4hepExample/DD4hepDetector/compact/FCCTracker.xml",Acts::Logging::VERBOSE);
    // extrapolate trough the tracking geometry
    DD4hepExample::extrapolation(trackingGeometry,Acts::Logging::INFO);
}
