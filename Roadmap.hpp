/*
 * RoadMap.h
 *
 *  Created on: Aug 29, 2013
 *      Author: nightfender
 */

#pragma once

#include <vector>
#include <map>

#include "simple_graph.hpp"
#include "linarr.hpp"




/* RoadMap traits definitions */

struct RoadData
{
	double length ;
};

template < typename RoadType=string >
struct RoadmapTraits
{
	typedef RoadType 										RoadID ;
	typedef simple_graph<int,RoadID,no_data,RoadData>		Roadmap ;
	typedef typename Roadmap::vertex_iterator				vertex_iterator ;
	typedef typename Roadmap::edge_iterator				road_iterator ;

	struct Address
	{
		RoadID		road ;
		double 		y ;

		friend ostream & operator<< ( ostream & out, const Address & a )
		{
			out << '(' << a.road << ',' << a.y << ')' ;
			return out ;
		}
	};

	typedef vector< Address >								AddressVector ;
};

/* algorithms */



