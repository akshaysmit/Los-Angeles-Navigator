#include "provided.h"
#include "support.h"
#include "MyMap.h"
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
using namespace std;

typedef pair<double, GeoCoord> coordPair;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string dest, vector<NavSegment>& directions) const;
    
private:
    SegmentMapper m_SegMap;
    AttractionMapper m_AttMap;
    
    struct nodeInfo {               //struct containing information about a vertex in the graph
        
        nodeInfo(string prevLat, string prevLon, double dist)
        : m_prevLat(prevLat), m_prevLon(prevLon), m_distFromStart(dist), m_toBeConsidered(true)
        {}
        
        void DoneConsidering() { m_toBeConsidered = false; }
        
        string m_prevLat;           // Lat/Lon of node from which this vertex was reached
        string m_prevLon;
        double m_distFromStart;
        bool m_toBeConsidered;
    };

    /* private member functions */
    
        //returns true if path found, otherwise returns false. If path is found, vec will hold
        //sequence of geocoordinates in the path. vec is unchanged if there is no path
    bool pathFinder(GeoCoord& begin, GeoCoord& end, vector<GeoCoord>& vec) const;
    bool isDestOnStreetSegment(StreetSegment& current, GeoCoord& dest) const;
    
         //great circle distance between two points
    double heuristic(GeoCoord& current, GeoCoord& end) const;
    
        //Constructs NavSegment objects for the given path
    void pathFormatter(vector<GeoCoord>& path, vector<NavSegment>& result) const;
    
        //Given two GeoCoord objects, this finds the unique StreetSegment with these endpoints
    StreetSegment findStreetSegment(GeoCoord& a, GeoCoord& b) const;
    
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    MapLoader loader;
    if(!loader.load(mapFile))
        return false;
    
    m_AttMap.init(loader);
    m_SegMap.init(loader);
    
	return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    GeoCoord begin, dest;
    
    if(!m_AttMap.getGeoCoord(start, begin))
        return NAV_BAD_SOURCE;
    
    if(!m_AttMap.getGeoCoord(end, dest))
        return NAV_BAD_DESTINATION;
    
    vector<GeoCoord> path;
    
    if(!pathFinder(begin, dest, path))
        return NAV_NO_ROUTE;
    
    pathFormatter(path, directions);        //Constructing the NavSegment objects from the path
    
	return NAV_SUCCESS;
}

/* private member functions */

bool NavigatorImpl::pathFinder(GeoCoord& begin, GeoCoord& dest, vector<GeoCoord> &vec) const {
    
    priority_queue<coordPair, vector<coordPair>, greater<coordPair>> pq;    //declaring a minheap
    MyMap<GeoCoord, nodeInfo> vertices;                                     //holds all encountered vertices
    
    pq.push(make_pair(heuristic(begin, dest), begin));
    vertices.associate(begin, nodeInfo("", "", 0));
    
    while(!pq.empty()) {
        
        GeoCoord curr = pq.top().second;            //highest weight vertex
        nodeInfo* currInfo = vertices.find(curr);
        pq.pop();
        
        if (!currInfo->m_toBeConsidered)            //vertex not to be considered again
            continue;
        
        currInfo->DoneConsidering();
        
        //Check if destination reached
        if (curr == dest) {
            
            vec.clear();
            nodeInfo* tempInfo;
            
            vec.push_back(curr);
            
            string lat = currInfo->m_prevLat;
            string lon = currInfo->m_prevLon;
            
            while (lat != "" && lon != "") {                  //track predecessors
                
                vec.push_back(GeoCoord(lat, lon));
                tempInfo = vertices.find(GeoCoord(lat, lon));
                lat = tempInfo->m_prevLat;
                lon = tempInfo->m_prevLon;
            }
            
            return true;
        }
        
        vector<StreetSegment> associates = m_SegMap.getSegments(curr);
        
        for (int i = 0; i < associates.size(); i++) {
            
            if (isDestOnStreetSegment(associates[i], dest)) {   //destination on this street segment
                
                nodeInfo* destInfo = vertices.find(dest);
                double newDist = currInfo->m_distFromStart + distanceEarthMiles(curr, dest);
                
                if (destInfo == nullptr || destInfo->m_distFromStart > newDist) {   //check if new distance is lower
                    vertices.associate(dest, nodeInfo(curr.latitudeText, curr.longitudeText, newDist));
                    pq.push(make_pair(newDist, dest));
                }
            }
            
            GeoCoord segStart = associates[i].segment.start;
            nodeInfo* startInfo = vertices.find(segStart);
            
            double newDist = currInfo->m_distFromStart + distanceEarthMiles(curr, segStart);
            double newWeight = newDist + heuristic(segStart, dest);
            
                //check if new distance is lower
            if (startInfo == nullptr || (startInfo->m_toBeConsidered && startInfo->m_distFromStart > newDist)) {
                vertices.associate(segStart, nodeInfo(curr.latitudeText, curr.longitudeText, newDist));
                pq.push(make_pair(newWeight, segStart));
            }
           
            GeoCoord segEnd = associates[i].segment.end;
            nodeInfo* endInfo = vertices.find(segEnd);
            
            newDist = currInfo->m_distFromStart + distanceEarthMiles(curr, segEnd);
            newWeight = newDist + heuristic(segEnd, dest);
            
                //check if new distance is lower
            if (endInfo == nullptr || (endInfo->m_toBeConsidered && endInfo->m_distFromStart > newDist)) {
                vertices.associate(segEnd, nodeInfo(curr.latitudeText, curr.longitudeText, newDist));
                pq.push(make_pair(newWeight, segEnd));
            }
   
        }
        
    }
    return false;
}

bool NavigatorImpl::isDestOnStreetSegment(StreetSegment &current, GeoCoord &dest) const {
    
    for (int i = 0; i <current.attractions.size(); i++) {
        if (current.attractions[i].geocoordinates == dest)
            return true;
    }
    
    return false;
}

double NavigatorImpl::heuristic(GeoCoord &current, GeoCoord &end) const {
    return distanceEarthMiles(current, end);
}

void NavigatorImpl::pathFormatter(vector<GeoCoord>& path, vector<NavSegment>& result) const {
    
    result.clear();
    for (size_t i = path.size()-1; i > 0; i--) {
        
        StreetSegment curr = findStreetSegment(path[i], path[i-1]);
        GeoSegment final;
        
        final = GeoSegment(path[i], path[i-1]);
        
        if (!result.empty() && curr.streetName != result.back().m_streetName) {     //Insert TURN NavSegment
            
            double angle = angleBetween2Lines(result.back().m_geoSegment, final);
            if (angle < 180)
                result.push_back(NavSegment("left", curr.streetName));
            else
                result.push_back(NavSegment("right", curr.streetName));
        }
        
        double dist = distanceEarthMiles(final.start, final.end);     //Insert PROCEED NavSegment
        double angle = angleOfLine(final);
        
        if (angle <= 22.5)
            result.push_back(NavSegment("east", curr.streetName, dist, final));
        else if (angle <= 67.5)
            result.push_back(NavSegment("northeast", curr.streetName, dist, final));
        else if (angle <= 112.5)
            result.push_back(NavSegment("north", curr.streetName, dist, final));
        else if (angle <= 157.5)
            result.push_back(NavSegment("northwest", curr.streetName, dist, final));
        else if (angle <= 202.5)
            result.push_back(NavSegment("west", curr.streetName, dist, final));
        else if (angle <= 247.5)
            result.push_back(NavSegment("southwest", curr.streetName, dist, final));
        else if (angle <= 292.5)
            result.push_back(NavSegment("south", curr.streetName, dist, final));
        else if (angle <= 337.5)
            result.push_back(NavSegment("southeast", curr.streetName, dist, final));
        else if (angle < 360)
            result.push_back(NavSegment("east", curr.streetName, dist, final));
    }
}

StreetSegment NavigatorImpl::findStreetSegment(GeoCoord &a, GeoCoord &b) const {
    
    vector<StreetSegment> a_associates = m_SegMap.getSegments(a);
    vector<StreetSegment> b_associates = m_SegMap.getSegments(b);
    
    for (int i = 0; i < a_associates.size(); i++) {
        for (int j = 0; j < b_associates.size(); j++) {
            
            if (a_associates[i].segment.start == b_associates[j].segment.start && a_associates[i].segment.end == b_associates[j].segment.end)
                return a_associates[i];
        }
    }
    return StreetSegment();
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}