#include "provided.h"
#include <vector>

using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;

private:
    std::string findDir(double x);
    std::string findTurn(double x);
    void commandHelper(list<StreetSegment> streets, vector<DeliveryCommand>& commands, string item, GeoCoord depot);
    const StreetMap* m_streetMap;
};

//find the direction based on angle
string DeliveryPlannerImpl::findDir(double angle) {
    if (angle >= 0 && angle < 22.5)
        return "east";
    else if (angle >= 22.5 && angle < 67.5)
        return "northeast";
    else if (angle >= 67.5 && angle < 112.5)
        return "north";
    else if (angle >= 112.5 && angle < 157.5)
        return "northwest";
    else if (angle >= 157.5 && angle < 202.5)
        return "west";
    else if (angle >= 202.5 && angle < 247.5)
        return "southwest";
    else if (angle >= 247.5 && angle < 292.5)
        return "south";
    else if (angle >= 292.5 && angle < 337.5)
        return "southeast";
    else
        return "east";
}

//find the direction to turn based on angle x
string DeliveryPlannerImpl::findTurn(double x) {
    if (x >= 1 && x < 180)
        return "left";
    else
        return "right";
}

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_streetMap = sm;
    
}
void DeliveryPlannerImpl::commandHelper(list<StreetSegment> streets, vector<DeliveryCommand>& commands, string item, GeoCoord depot) {
   
    list<StreetSegment>::iterator curr = streets.begin();
    list<StreetSegment>::iterator next = streets.begin();
    if (next != streets.end())
        next++; //get the next thing in the list
    DeliveryCommand dc;
    for (; curr != streets.end(); curr++) {
        list<StreetSegment>::iterator og = curr;
        bool compile = false;
        double distance = 0.0;
        while (next != streets.end() && curr->name == next->name) {
            compile = true;
            if (curr != next && curr != streets.end())
                curr++;
            if (next != streets.end())
                next++;
        }
        if (curr == streets.end())
            break;
        distance = distanceEarthMiles(og->start, curr->end);
        if (compile) {
            dc.initAsProceedCommand(findDir(angleOfLine(*curr)), og->name, distance);
            commands.push_back(dc);
            compile = false;
            distance = 0.0;
        }
        else {
            distance = distanceEarthMiles(curr->start, curr->end);
            dc.initAsProceedCommand(findDir(angleOfLine(*curr)), curr->name, distance);
            commands.push_back(dc);
        }
        if (next != streets.end()) {
            dc.initAsTurnCommand(findTurn(angleBetween2Lines(*curr, *next)), next->name);
            commands.push_back(dc);
        }
    }
    curr = streets.end();
    curr--;
    if (curr->end != depot) {
        dc.initAsDeliverCommand(item);
        commands.push_back(dc);
    }
}
DeliveryPlannerImpl::~DeliveryPlannerImpl()
{

}



DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    //make vector called reqs and use it to optomize the order
    vector<DeliveryRequest> reqs = deliveries;
    DeliveryOptimizer delOp(m_streetMap);
    double oldCrow, newCrow;
    delOp.optimizeDeliveryOrder(depot, reqs, oldCrow, newCrow);
    totalDistanceTravelled = 0;
    vector<StreetSegment> segs;
    list<StreetSegment> all_segs;

    //if cannot find depo, return bad_coord
    if (m_streetMap->getSegmentsThatStartWith(depot, segs) == false)
        return BAD_COORD;

    PointToPointRouter router(m_streetMap); //create a pointtopoint router
    if (reqs.size() >= 1) { //only do this if there is 1 more or more deliveries
        DeliveryResult result = router.generatePointToPointRoute(depot, reqs[0].location, all_segs, totalDistanceTravelled);
        if (result == BAD_COORD)
            return BAD_COORD;
        else if (result == NO_ROUTE)
            return NO_ROUTE;

        //call the commandHelper function wiht first delivery and depo
        (const_cast<DeliveryPlannerImpl*>(this))->commandHelper(all_segs, commands, reqs[0].item, depot);
        //basically do the same thing for the rest of the delivieries
        for (int i = 1; i < reqs.size(); i++) {
            list<StreetSegment> tempSegs;
            double tempDistance;
            router.generatePointToPointRoute(reqs[i - 1].location, reqs[i].location, tempSegs, tempDistance);
            totalDistanceTravelled += tempDistance;
            (const_cast<DeliveryPlannerImpl*>(this))->commandHelper(tempSegs, commands, reqs[i].item, depot);
            all_segs.splice(all_segs.end(), tempSegs); //append the route for a specific delivery to route of all deliveries
        }
        list<StreetSegment> tempSegs;
        double tempDistance;
        //find the route from the last delivery back to depo
        router.generatePointToPointRoute(reqs[reqs.size() - 1].location, depot, tempSegs, tempDistance);
        totalDistanceTravelled += tempDistance;
        (const_cast<DeliveryPlannerImpl*>(this))->commandHelper(tempSegs, commands, reqs[reqs.size() - 1].item, depot);
        all_segs.splice(all_segs.end(), tempSegs);
    }
   
    return DELIVERY_SUCCESS;
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
