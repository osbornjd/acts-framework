#pragma once

#include <vector>
#include "TVector3.h"


struct SpacePoint {
    int       plane;
    int       module;
    int       sensor;
    int       row;
    double    charge;
    TVector3  globalPos;

    SpacePoint()
        : plane     {-1}
        , module    {-1}
        , sensor    {-1}
        , row       {-1}
        , charge    {-1.}
        , globalPos {0., 0., 0.}
    {
    }

    //SpacePoint(FaserSpacePoint * sp)
    //    : plane     {sp->Plane()}
    //    , module    {sp->Module()}
    //    , sensor    {sp->Sensor()}
    //    , row       {sp->Row()}
    //    , charge    {sp->Charge()}
    //    , globalPos {sp->GlobalPos()}
    //{
    //}
};

struct Event {
    long eventNumber;
    std::vector<SpacePoint> spacePoints;

    Event()
        : eventNumber {-1}
    {
    }

    //void set(long eventNumber_, std::vector<FaserSpacePoint*> faserSpacePoints) {
    //    eventNumber = eventNumber_;
    //    spacePoints.clear();
    //    for (FaserSpacePoint * sp : faserSpacePoints) {
    //        spacePoints.push_back(SpacePoint {sp});
    //    }
    //}

    void clear() { spacePoints.clear(); }
};

