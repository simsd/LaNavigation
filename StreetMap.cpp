#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <set>
using namespace std;

//Below are my hashe functions for geocoords, strings , ints and chars
unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

unsigned int hasher(const int& i)
{
    return std::hash<int>()(i);
}

unsigned int hasher(const string& s)
{
    return std::hash<string>()(s);
}


class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;

private:
    //this is an expandable hash map still will hold all the street data
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> m_streets;
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
    //delete all street segment pointers
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream fin;
    fin.open(mapFile); //open the file and check if it fails
    if (fin.fail())
        return false;
    //get first infomation
    string streetname;
    string coord;
    string count;
    //read all of the data from the mapFile 
    while (!fin.eof()) { //while the file is not empty
        getline(fin, streetname); //to get street name
        getline(fin, count);
        string lon1 = "", lat1 = "", lon2 = "", lat2 = "";
        for (int i = 0; i < stoi(count); i++) {
            lon1 = "", lat1 = "", lon2 = "", lat2 = "";
            int space = 0;
            getline(fin, coord);
            for (int i = 0; i < coord.size(); i++) {
                if (space == 0) {
                    if (coord[i] == ' ') {
                        space++;
                        continue;
                    }
                    lon1 += coord[i];
                }
                else if (space == 1) {
                    if (coord[i] == ' ') {
                        space++;
                        continue;
                    }
                    lat1 += coord[i];
                }
                else if (space == 2) {
                    if (coord[i] == ' ') {
                        space++;
                        continue;
                    }
                    lon2 += coord[i];
                }
                else if (space == 3) {
                    if (coord[i] == ' ') {
                        space++;
                        continue;
                    }
                    lat2 += coord[i];
                }
            }
            GeoCoord starting(lon1, lat1);
            GeoCoord ending(lon2, lat2);
            StreetSegment ss(starting, ending, streetname);
            //if this is a new starting geo for the hashTable create a vector and associate it 
            vector<StreetSegment>* s = m_streets.find(starting);
            if (s == nullptr) { //if vector does not exist
                vector<StreetSegment> s; //create and push back segment and place in hashTable
                s.push_back(ss);
                m_streets.associate(starting, s);
            }
            else { //otherwise push back to the vectro and re-associate it 
                s->push_back(ss); 
            }
            StreetSegment se(ending, starting, streetname);
            vector<StreetSegment>* e = m_streets.find(ending); //look for vector of ending
            if (e == nullptr) { //if not create it, push seg, and associate it
                vector<StreetSegment> s;
                s.push_back(se);
                m_streets.associate(ending, s);
            }
            else {
                //pushback segment to vector
                e->push_back(se);
            }
        }
    }
    //close the file input and return true;
    fin.close();
    return true;
    
    
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    //try to find vector in hash table 
    const vector<StreetSegment>* b = m_streets.find(gc);
    if (b != nullptr) { //if it is nullptr, set segs to b and return false
        segs = *(b);
        return true;
    }
    //if not found return false
    return false;


}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    return m_impl->getSegmentsThatStartWith(gc, segs);
}
