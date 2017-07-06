#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    size_t m_numSegments;
    vector<StreetSegment> m_segments;
};

MapLoaderImpl::MapLoaderImpl() : m_numSegments(0)
{
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    if (!infile)
        return false;
    
    string s;
    
    while (getline(infile, s)) {                            //segment's street name contained in s
        
        string lat_begin, lon_begin, lat_end, lon_end;
        
        getline(infile, lat_begin, ',');                    //starting lat/lon
        infile >> lon_begin;
        
        infile.ignore(1);
        getline(infile, lat_end, ',');                      //ending lat/lon
        infile >> lon_end;
        
        GeoCoord gc1(lat_begin, lon_begin);
        GeoCoord gc2(lat_end, lon_end);
        
        StreetSegment street_segment;
        street_segment.streetName = s;
        street_segment.segment = GeoSegment(gc1, gc2);
        
        infile.ignore(10000, '\n');
        
        int attraction_num;                                 //number of attractions
        infile >> attraction_num;
        
        infile.ignore(10000, '\n');
        
        for (int i = 0; i < attraction_num; i++) {
            
            string attraction_name;                         //attraction name
            getline(infile, attraction_name, '|');
            string attraction_lat, attraction_lon;
            
            getline(infile, attraction_lat, ',');           //attraction lat/lon
            infile >> attraction_lon;
            
            GeoCoord gc(attraction_lat, attraction_lon);    //create Attraction object
            Attraction att;
            att.name = attraction_name;
            att.geocoordinates = gc;
            
            street_segment.attractions.push_back(att);
            
            infile.ignore(10000, '\n');
        }
     
        m_segments.push_back(street_segment);
        m_numSegments++;
    }
    
	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
    return m_numSegments;
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if (segNum >= m_numSegments)
        return false;
    
    seg = m_segments[segNum];
	return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
