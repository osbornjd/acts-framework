#ifndef ACTS_DD4HEPDETECTOR_H
#define ACTS_DD4HEPDETECTOR 1

// STL include(s)
#include <memory>

// ACTS include(s)
namespace Acts {
class TrackingGeometry;

}
namespace DD4hepExample {
    std::unique_ptr<const Acts::TrackingGeometry> trackingGeometry();
}

#endif  // ACTS_DD4HEPDETECTOR_H
