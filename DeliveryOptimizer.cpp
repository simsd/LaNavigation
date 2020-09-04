#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;

private:
    const StreetMap* sm;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    this->sm = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = 0;
    newCrowDistance = 0;
    //determining old crow's distance
    if (deliveries.size() >= 1) {
        oldCrowDistance += distanceEarthMiles(depot, deliveries[0].location);
        for (int i = 1; i < deliveries.size(); i++) {
            oldCrowDistance += distanceEarthMiles(deliveries[i - 1].location, deliveries[i].location);
        }
        oldCrowDistance += distanceEarthMiles(deliveries[deliveries.size()-1].location, depot);

        //Here lies the route optomization
        for (int i = 0; i < deliveries.size() - 1; i++)
        {
            for (int j = 0; j < (deliveries.size() - i - 1); j++) {
                if (distanceEarthMiles(depot, deliveries[j].location) > distanceEarthMiles(depot, deliveries[j + 1].location)) {
                    swap(deliveries[j], deliveries[j + 1]);
                }
            }
        }


        newCrowDistance += distanceEarthMiles(depot, deliveries[0].location);
        for (int i = 1; i < deliveries.size(); i++) {
            newCrowDistance += distanceEarthMiles(deliveries[i - 1].location, deliveries[i].location);
        }
        newCrowDistance += distanceEarthMiles(deliveries[deliveries.size() - 1].location, depot);
    }
}

//******************** DeliveryOptimizer   functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
