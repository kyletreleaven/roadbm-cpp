/*
 * roadbm.cpp
 *
 *  Created on: Aug 15, 2013
 *      Author: nightfender
 */

// stdc
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "assert.h"

// stdc++
#include <cmath>
#include <iostream>

#include <limits>
#include <utility>
#include <iterator>

#include <list>
#include <map>
#include <queue>		// for matching
#include <deque>
#include <numeric>		// for accumulate
#include <set>
#include <algorithm>

// boost graph
#include "toposort.hpp"
#include "simple_graph.hpp"

// utility
#include "printcontainer.hpp"
#include "linarr.hpp"
//
#include "Roadmap.hpp"
#include "roadmap_bpsort.hpp"


using namespace std ;
//using namespace boost ;

//int screwdriver = numeric_limits<int>::min() ;
//int intmax = numeric_limits<int>::max() ;
//const double INF = numeric_limits<double>::infinity() ;
//const double NEGINF = -INF ;


double uniform()
{
	return (double) rand() / RAND_MAX ;
}




/* typedefs for linear arrangement template */
struct vertex_struct
{
	double support ;
};

struct interval_struct
{
	double slope ;
	double offset ;
};

ostream & operator << ( ostream & out, const interval_struct & I )
{
	out << '<' << I.slope << " * y + " << I.offset << '>' ;
	return out ;
}















int main( int argc, char * argv [] )
{
	int numpoints ;

	/* process command line */
	if ( argc != 2 || sscanf( argv[1], "%d", &numpoints ) == 0 || numpoints < 0 )
	{
		cout << "usage: " << argv[0] << " [number of points (positive)]" << endl ;
		return 0 ;
	}

	/* construct an example graph --- later take input! */
	typedef RoadmapTraits<string>	rmtraits ;
	typename rmtraits::Roadmap		MAP ;
	typedef typename rmtraits::vertex_iterator	node_iterator ;
	typedef typename rmtraits::road_iterator		road_iterator ;

	vector< node_iterator > verts ;
	verts.push_back( MAP.vertex(0) ) ;
	verts.push_back( MAP.vertex(1) ) ;
	verts.push_back( MAP.vertex(2) ) ;

	vector< road_iterator > roads ;
	road_iterator temp ;
	temp = MAP.edge( "L", verts[0], verts[1] ) ;
	temp->second.local_data.length = 1. ;
	roads.push_back( temp ) ;

	temp = MAP.edge( "R", verts[1], verts[2] ) ;
	temp->second.local_data.length = 2. ;
	roads.push_back( temp ) ;

	const int numroad = roads.size() ;

	/* seed random */
	srand( (unsigned int) time(NULL) ) ;

	/* generate random points */
	typename rmtraits::AddressVector P(numpoints), Q(numpoints) ;
	for ( int i=0 ; i < numpoints ; i++ )
	{
		temp = roads[ rand() % numroad ] ;
		P[i].road = temp->first ;
		P[i].y = temp->second.local_data.length * uniform() ;		// this is kinda ugly

		temp = roads[ rand() % numroad ] ;
		Q[i].road = temp->first ;
		Q[i].y = temp->second.local_data.length * uniform() ;		// this is kinda ugly
	}

	cout << "P equals " << P << endl ;
	cout << "Q equals " << Q << endl ;

	//typedef RoadmapTraits<string> rmtraits ;
	map< typename rmtraits::RoadID, segment_type > sorted ;
	// sorted = roadmap_bpsort<rmtraits>( P, Q ) ;

	roadmap_bpsort( P, Q ) ;

	//cout << roadmap_bpsort<rmtraits>( P, Q ) << endl ;

	//cout << endl << sorted << endl ;

#if false
	/* I think this will adapt to the sorting, since P and Q are references */
	match_cost_func match_cost( P, Q ) ;

	/* ALGORITHM */
	list< pair<int,int> > match1, match2 ;

	match1 = MATCH( P, Q, LENGTH ) ;
	cout << "unsorted match cost: " << match_cost( match1 ) << endl ;

	sort( P.begin(), P.end() ) ;
	sort( Q.begin(), Q.end() ) ;
	match2 = MATCH( P, Q, LENGTH ) ;
	cout << "sorted match cost: " << match_cost( match2 ) << endl ;
#endif

	return 0 ;
}











