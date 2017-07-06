#include "provided.h"
#include "MyMap.h"
#include "support.h"
#include <vector>
#include <string>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    MyMap<GeoCoord, vector<StreetSegment>> m_map;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    for (int i = 0; i < ml.getNumSegments(); i++) {
        
        StreetSegment seg;
        
        ml.getSegment(i, seg);
    
        GeoCoord start(seg.segment.start.latitudeText, seg.segment.start.longitudeText);
        GeoCoord end(seg.segment.end.latitudeText, seg.segment.end.longitudeText);
        
        vector<StreetSegment>* vec = m_map.find(start);
        
        if (vec == nullptr) {               //create new pair if key not present
            vector<StreetSegment> temp;
            temp.push_back(seg);
            m_map.associate(start, temp);
        }                                   //add StreetSegment to value vector if key present
        else
            (*vec).push_back(seg);
        
        vec = m_map.find(end);
        
        if (vec == nullptr) {               //create new pair if key not present
            vector<StreetSegment> temp;
            temp.push_back(seg);
            m_map.associate(end, temp);
        }                                   //add StreetSegment to value vector if key present
        else
            (*vec).push_back(seg);
        
        for (int i = 0; i < seg.attractions.size(); i++) {  //Attractions
            
            GeoCoord key(seg.attractions[i].geocoordinates.latitudeText,
                            seg.attractions[i].geocoordinates.longitudeText);
            
            if (key == start || key == end) //StreetSegment already associated with geocoordinate
                continue;
            
            vec = m_map.find(key);
            
            if (vec == nullptr) {           //create new pair if key not present
                vector<StreetSegment> temp;
                temp.push_back(seg);
                m_map.associate(key, temp);
            }
            else                            //add StreetSegment to value vector if key present
                (*vec).push_back(seg);
        }
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    GeoCoord temp(gc.latitudeText, gc.longitudeText);
    const vector<StreetSegment>* vec = m_map.find(temp);
	
    if (vec == nullptr)
        return vector<StreetSegment>();
    else
        return *vec;
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
