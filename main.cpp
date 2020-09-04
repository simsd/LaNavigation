#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v);
bool parseDelivery(string line, string& lat, string& lon, string& item);

int main(int argc, char* argv[])
{                                             
    StreetMap sm;

    if (!sm.load("mapdata.txt"))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }

    GeoCoord depot;
    vector<DeliveryRequest> deliveries;
    if (!loadDeliveryRequests("deliveries.txt", depot, deliveries))
    {
        cout << "Unable to load delivery request file " << argv[2] << endl;
        return 1;
    }

    cout << "Generating route...\n\n";
    double a = 0.0, b = 0.0;
    DeliveryOptimizer j(&sm);
    j.optimizeDeliveryOrder(depot, deliveries, a, b);
    DeliveryPlanner dp(&sm);
    vector<DeliveryCommand> dcs;

    double totalMiles;
    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
    if (result == BAD_COORD)
    {
        cout << "One or more depot or delivery coordinates are invalid." << endl;
        return 1;
    }
    if (result == NO_ROUTE)
    {
        cout << "No route can be found to deliver all items." << endl;
        return 1;
    }
    cout << "Starting at the depot...\n";
    for (const auto& dc : dcs)
        cout << dc.description() << endl;
    cout << "You are back at the depot and your deliveries are done!\n";
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << totalMiles << " miles travelled for all deliveries." << endl;
}

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v)
{
    ifstream inf(deliveriesFile);
    if (!inf)
        return false;
    string lat;
    string lon;
    inf >> lat >> lon;
    inf.ignore(10000, '\n');
    depot = GeoCoord(lat, lon);
    string line;
    while (getline(inf, line))
    {
        string item;
        if (parseDelivery(line, lat, lon, item))
            v.push_back(DeliveryRequest(item, GeoCoord(lat, lon)));
    }
    return true;
}

bool parseDelivery(string line, string& lat, string& lon, string& item)
{
    const size_t colon = line.find(':');
    if (colon == string::npos)
    {
        cout << "Missing colon in deliveries file line: " << line << endl;
        return false;
    }
    istringstream iss(line.substr(0, colon));
    if (!(iss >> lat >> lon))
    {
        cout << "Bad format in deliveries file line: " << line << endl;
        return false;
    }
    item = line.substr(colon + 1);
    if (item.empty())
    {
        cout << "Missing item in deliveries file line: " << line << endl;
        return false;
    }
    return true;
}

char* match(const char* regex, char* line)
{
    char* matching = NULL;
    int regCount;
    int i = 0;
    int skip = 0;
    while (regex[i] != '\0')
        regCount++;

    if (regCount == 0 || (regex[0] == '\\' && regCount == 1)){
        int k = 0;
        while (line[k] != '\0')
            putchar(line[k]);
        if (line[k - 1] != '\n')
            putchar('\n');
    }
    if (regex[0] == '\\' && regex[1] != '.')
        skip = 1;

        i = 0;
        for(; line[i] != '\0'; i++) {
            if ((skip && regex[1] == line[i]) || regex[0] == '.' || regex[0] == line[i]) {
                matching = (char*)realloc(matching, sizeof(matching) + sizeof(char));
                    matching[0] = line[i];
                    regCount++;
                    for (int k = 0; regex[k] != '\0'; k++) {
                        if (line){}

                    }
            }
        }

}

