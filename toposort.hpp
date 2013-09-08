
#pragma once

// stdc
#include "stdlib.h"
#include "stdio.h"
//#include "time.h"
#include "assert.h"

// stdc++
#include <iostream>

#include <utility>
#include <iterator>
//#include <numeric>		// for accumulate

#include <vector>
#include <list>
#include <map>
#include <queue>		// for matching
#include <deque>
#include <set>
#include <algorithm>

// boost graph
//#include "boost/graph/adjacency_list.hpp"

// utility
#include "printcontainer.hpp"

// objects
//#include "Interval.hpp"


using namespace std ;


struct intgraph
{
	// set info
	set<int> V ;
	set<int> E ;

	// topology info, insert-only right now
	vector< set<int> > AdjV ;
	vector< pair<int,int> > AdjE ;

	int new_vertex()
	{
		int u = AdjV.size() ;
		AdjV.push_back( set<int>() ) ;
		V.insert( u ) ;
		return u ;
	}

	int new_edge( int u, int v )
	{
		set<int>::iterator u_it = V.find(u) ;
		set<int>::iterator v_it = V.find(v) ;
		assert( u_it != V.end() && v_it != V.end() ) ;

		int e = AdjE.size() ;
		E.insert( e ) ;			// add the edge to set E
		AdjV[u].insert( e ) ;	// add to adjacency list of u
		AdjE.push_back( pair<int,int>(u,v) ) ;	// add signature to AdjE

		return e ;
	}
};

ostream & operator<< ( ostream & out, const intgraph & g )
{
	out << "{ " ;
	for ( set<int>::iterator
			e_it = g.E.begin() ; e_it != g.E.end() ; ++e_it )
	{
		int e = *e_it ;
		pair<int,int> sig = g.AdjE[e] ;
		out << e << " => (" << sig.first << "," << sig.second << "), " ;
	}
	out << " }" ;
	return out ;
}




struct topo_data
{
	static const int ORPHAN = -1 ;
	enum { WHITE, GRAY, BLACK } ;

	vector<int> color ;
	vector<int> parent ;

	topo_data( int numvert ) :
		color( numvert, WHITE ),
		parent( numvert, ORPHAN ) {} ;
};

void topovisit( int u, const intgraph & g, topo_data & tdata, list<int> & order )
{
	tdata.color[u] = topo_data::GRAY ;
	const set<int> & u_out_edges = g.AdjV[u] ;
	for ( set<int>::const_iterator
			e_it = u_out_edges.begin() ; e_it != u_out_edges.end() ; ++e_it )
	{
		int e = *e_it ;
		int v = g.AdjE[e].second ;
		if ( tdata.color[v] == topo_data::WHITE )
		{
			tdata.parent[v] = u ;
			topovisit( v, g, tdata, order ) ;
		}
	}

	tdata.color[u] = topo_data::BLACK ;
	order.push_front( u ) ;
}


list<int> toposort( const intgraph & g )
{
	// from Cormen pseudo-code
	list<int> final_order ;

	int numvert = g.V.size() ;
	topo_data tdata( numvert ) ;

	for ( set<int>::iterator
			u_it = g.V.begin() ; u_it != g.V.end() ; ++u_it )
	{
		int u = *u_it ;
		if ( tdata.color[u] == topo_data::WHITE ) topovisit( u, g, tdata, final_order ) ;
	}

	return final_order ;
}









