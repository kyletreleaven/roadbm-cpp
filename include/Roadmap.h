/*
 * RoadMap.h
 *
 *  Created on: Aug 29, 2013
 *      Author: nightfender
 */

#pragma once


#include <map>
#include "boost/graph/adjacency_list.hpp"

using namespace std ;
using namespace boost ;


/* Roadmap class */
template < typename RoadDescriptor, typename VertexDescriptor, typename LengthType >
class Roadmap
{
public :
	typedef RoadDescriptor	road_descriptor ;
	typedef LengthType		road_length_type ;
	typedef map< road_descriptor, road_length_type >
								length_property_type ;


/* data members */
	length_property_type		_lengths ;
};





