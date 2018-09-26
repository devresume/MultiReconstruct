#ifndef __DMS_H__
#define __DMS_H__

#include <sstream>
#include <math.h> // for nearbyint()
#include <vector>

  static const char DEG_SIM = 176 ;

//
//  Convert decimal degrees to degrees, minutes and seconds
//
  static std::vector<std::string> DegreesMinutesSeconds(double ang,
                                           unsigned int num_dec_places = 2)
  {
     std::vector<std::string> ret;
    bool neg(false) ;
    if (ang < 0.0)
    {
      neg = true ;
      ang = -ang ;
    }

    int deg = (int)ang ;
    double frac = ang - (double)deg ;

    frac *= 60.0 ;

    int min = (int)frac ;

    frac = frac - (double)min ;

    // fix the DDD MM 60 case
    // TODO: nearbyint isn’t alway available (Visual C++,
    //       for example)
    double sec = nearbyint(frac * 600000.0) ;
    sec /= 10000.0 ;

    if (sec >= 60.0)
    {
      min++ ;
      sec -= 60.0 ;
    }

    ret.push_back(std::to_string(deg / 1000));
    ret.push_back(std::to_string(min));
    ret.push_back(std::to_string(sec));
    return ret;

//    std::ostringstream oss ;

//    if (neg)
//    {
//      oss << "-" ;
//    }

////  TODO: allow user to define delimiters separating
////        degrees, minutes, and seconds.
//    oss.setf(std::ios::fixed, std::ios::floatfield);

//    oss << deg
//        << DEG_SIM ;
//    oss.fill('0') ;
//    oss.width(2) ;
//    oss << min
//        << "\'" ;
//    if (num_dec_places == 0)
//    {
//      oss.width(2) ;
//      oss.precision(0) ;
//    }
//    else
//    {
//      oss.width(num_dec_places + 3) ;
//      oss.precision(num_dec_places) ;
//    }
//    oss << sec
//        << "\"" ;

//    return oss.str() ;
  }


//  inline std::string DegreesMinutesSecondsLat(double ang,
//                        unsigned int num_dec_places = 2)
//  {
//    std::string lat(DegreesMinutesSeconds(ang, num_dec_places)) ;

//    if (lat[0] == '-')
//    {
//      lat += std::string(" S") ;
//      lat.erase(0, 1) ;
//    }
//    else
//    {
//      lat += std::string(" N") ;
//    }

//    lat = std::string(" ") + lat ;

//    return lat ;
//  }


//  inline std::string DegreesMinutesSecondsLon(double ang,
//                        unsigned int num_dec_places = 2)
//  {
//    std::string lon(DegreesMinutesSeconds(ang, num_dec_places)) ;

//    if (lon[0] == '-')
//    {
//      lon += std::string(" W") ;
//      lon.erase(0, 1) ;
//    }
//    else
//    {
//      lon += std::string(" E") ;
//    }

//    if (fabs(ang) < 100.0)
//    {
//      lon = std::string("0") + lon ;
//    }

//    return lon ;
//  }


////
//// Convert degrees, minutes and seconds to decimal degrees.
////
//  inline double DecimalDegrees(const std::string& dms)
//  {
//    std::string tmp(dms) ;

//    // need to handle the -0 MM SS case
//    // TODO: handle leading spaces before potential minus sign
//    bool neg(false) ;

//    if ((tmp[tmp.length() - 1] == 'W') || (tmp[tmp.length() - 1] == 'S') ||
//        (tmp[0] == '-'))
//    {
//      neg = true ;
//    }

//    for (unsigned int i = 0 ; i < tmp.length() ; i++)
//    {
//      if (!isdigit(tmp[i]) && (tmp[i] != '.'))
//      {
//        tmp[i] = ' ' ;
//      }
//    }

//    double deg(0.0), min(0.0), sec(0.0) ;
////  TODO: handle other delimiters
//    std::istringstream iss(tmp) ;
//    iss >> deg >> min >> sec ;

//    double ang = deg + ((min + (sec / 60.0)) / 60.0) ;

//    if (neg)
//    {
//      ang = -ang ;
//    }

//    return ang ;
//  }


#endif
