#include "provided.h"
#include "ExpandableHashMap.h"
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <algorithm>
#include <map>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;

private:
    const StreetMap* m_map; //will hold streetMap pointer passed in
};


PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_map = sm; //set m_map to passed in streetmap
} 

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
    const GeoCoord& start,
    const GeoCoord& end,
    list<StreetSegment>& route,
    double& totalDistanceTravelled) const
{
    totalDistanceTravelled = 0.0; //set totalDistance traveled to zero
    set<GeoCoord> visited;  //will hold all the GeoCoords we have already visited
    queue<GeoCoord> toDo;   //queue for DFS searchign
    if (start == end) {//navigating where we are standing
        route.clear(); //clear the list
        totalDistanceTravelled = 0.0;
        return DELIVERY_SUCCESS; //return Delivery_Sucess if made it to end geocoords
    }
    //burner vector
    vector<StreetSegment> a;
    if (m_map->getSegmentsThatStartWith(start, a) == false || m_map->getSegmentsThatStartWith(end, a) == false) {
        return BAD_COORD;
    } //if geocoords not into m_map return bad_coords
    //Here is the routeToRoute algorithm
    toDo.push(start);
    map<GeoCoord, GeoCoord> route_map;
    GeoCoord prev("0.0", "0.0");
    bool found = false;

    while (!toDo.empty()) { //while the queue is not empty
        GeoCoord curr = toDo.front();
        toDo.pop();
        if (visited.find(curr) != visited.end()) { //check if you already visited the curr
            continue; //already been here continue to next 
        }
        visited.insert(curr);
        if (curr == end) { //if you are at the destination return true
            found = true;
            break;
        }
        vector<StreetSegment> segs;
        m_map->getSegmentsThatStartWith(curr, segs);
        //sort segements to find the shortestest one
        int  min;
        for (int i = 0; i < segs.size() - 1; i++) {
            min = i;
            for (int j = i + 1; j < segs.size(); j++) {
                if (distanceEarthMiles(curr, segs[j].end) < distanceEarthMiles(curr, segs[min].end))
                    min = j;
                swap(segs[min], segs[i]);
            }
       }
        //push the end geocoords into the queue
        for (int i = 0; i < segs.size(); i++) {
            if (visited.find(segs[i].end) == visited.end()) {
                route_map[segs[i].end] = curr;
                toDo.push(segs[i].end);
            }
        }

    }

    //if found need to push populate route
    if (found) {
        GeoCoord currCoord = end;
        GeoCoord prevCoord = route_map[currCoord];
        //working backwards from end coord to initial start coords
        while (currCoord != start){
            vector<StreetSegment> streets;
            m_map->getSegmentsThatStartWith(prevCoord, streets);
            for (int i = 0; i < streets.size(); i++) {
                if (streets[i].end == currCoord)
                    route.push_front(streets[i]); //find prev coord or curr node and push_front ot route
            }
            currCoord = prevCoord;
            prevCoord = route_map[prevCoord];
        }
        
        //go through route and find totalDistance travelled
        list<StreetSegment>::iterator iter = route.begin();
        for (; iter != route.end(); iter++) {
            totalDistanceTravelled += distanceEarthMiles(iter->start, iter->end);
        }
    }
    if (found) //if found return delivery return
        return DELIVERY_SUCCESS;

    return NO_ROUTE;  // Delete this line and implement this function correctly
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
    const GeoCoord& start,
    const GeoCoord& end,
    list<StreetSegment>& route,
    double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
