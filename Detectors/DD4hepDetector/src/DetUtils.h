#ifndef DETCOMMON_DETUTILS_H
#define DETCOMMON_DETUTILS_H


// DD4hep
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Segmentations.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DDSegmentation/CartesianGridXY.h"
#include "DDSegmentation/PolarGridRPhi.h"



/** Given a xml element with several daughters with the same name, e.g.
 <detector> <layer name="1" /> <layer name="2"> </detector>
 this method returns the first daughter of type nodeName whose attribute has a given value
 e.g. returns <layer name="2"/> when called with (detector, "layer", "name", "1") */
namespace det {
namespace utils {
dd4hep::xml::Component getNodeByStrAttr(const dd4hep::xml::Handle_t& mother, const std::string& nodeName, const std::string& attrName,
                                        const std::string& attrValue);

/// try to get attribute with double value, return defaultValue if attribute not found
double getAttrValueWithFallback(const dd4hep::xml::Component& node, const std::string& attrName, const double& defaultValue);

}
}
#endif /* DETCOMMON_DETUTILS_H */
