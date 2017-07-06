#include "support.h"
#include "provided.h"
#include <string>

bool operator<(const GeoCoord& a, const GeoCoord& b) {
    
    if (a.latitude < b.latitude)
        return true;
    else if (a.latitude > b.latitude)
        return false;
    
    //a.latitude == b.latitude
    
    if (a.longitude < b. longitude)
        return true;
    else if (a.longitude > b.longitude)
        return false;
    
    //a.longitude == b.longitude
    
    return false;
}

bool operator==(const GeoCoord& a, const GeoCoord& b) {
    return a.latitudeText == b.latitudeText && a.longitudeText == b.longitudeText;
}