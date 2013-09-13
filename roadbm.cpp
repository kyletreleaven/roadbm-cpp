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




struct place_data
{
	typedef list<int> index_container_type ;
	index_container_type P_indices ;
	index_container_type Q_indices ;

	void annihilate( list< pair<int,int> > & match )
	{
		while ( !P_indices.empty() && !Q_indices.empty() )
		{
			int i, j ;
			i = P_indices.front() ; P_indices.pop_front() ;
			j = Q_indices.front() ; Q_indices.pop_front() ;
			match.push_back( pair<int,int>( i, j ) ) ;
		}
	}
};

ostream & operator<< ( ostream & out, const place_data & pd )
{
	out << "{ P => " << pd.P_indices
		<< ", Q => " << pd.Q_indices
		<< " }" ;
	return out ;
}


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




typedef map< double, place_data > segment_type ;
typedef LinearArrangement<vertex_struct,interval_struct> Arrangement ;


Arrangement
segment_cost_characterization( const segment_type & segment )
{
	/* obtain the level supports of F */
	double zerocost = 0. ;		// cost of a matching with "zero assistance"
	map<int, double> level_support ;
	{
		int level = 0 ;
		segment_type::const_iterator lbound, rbound ;
		for ( rbound = segment.begin(), lbound = rbound++ ;
				rbound != segment.end() ; lbound++, rbound++ )
		{
			double y1, y2, len ;
			y1 = lbound->first, y2 = rbound->first, len = y2 - y1 ;
			const place_data & temp = lbound->second ;

			level += temp.P_indices.size() - temp.Q_indices.size() ;
			zerocost += len * abs( level ) ;
			level_support[level] += len ;
		}
	}
	cout << "supports " << level_support << endl ;

	/* create a linear arrangement from level supports */
	Arrangement value_arr ;

	//int numlevels = level_support.size() ;
	//list<vertex_struct> 		vertex_data ;
	//list<interval_struct> 		interval_data ;

	double length = 0. ;
	for ( map<int,double>::iterator
			it = level_support.begin() ; it != level_support.end() ; ++it )
	{
		int 	f		= it->first ;
		double	supp 	= it->second ;
		Arrangement::Vertex & v = value_arr.insert( f ) ;
		v.local_data.support = supp ;

		length += supp ;
	}

	// about to traverse the arrangement
	Arrangement::Vertex 	* v_left = & value_arr.vertices.begin()->second ;
	Arrangement::Vertex 	* v_zero ;
	Arrangement::Interval 	* I_left = & *v_left->left ;

	// some initialization
	//interval_data.push_front( interval_struct() ) ;
	I_left->local_data.slope = length ;		// length won't compile...

	Arrangement::Vertex		* v_curr ;
	Arrangement::Interval	* I_curr ;

	// traverse left-to-right to compute slopes
	{
		double slope = I_left->local_data.slope ;
		v_curr = v_left ;
		while ( true )
		{
			if ( (int) v_curr->y == 0 ) v_zero = v_curr ;		// remember the zero vertex, plox

			I_curr = & *v_curr->right ;
			slope -= 2 * v_curr->local_data.support ;
			I_curr->local_data.slope = slope ;

			if ( I_curr->right == NULL ) break ;
			v_curr = I_curr->right ;
		}
	}

	// traverse zero-to-left, then zero-to-right, to obtain offsets
	{
		double offset = zerocost ;
		v_curr = v_zero ;
		while ( true )
		{
			I_curr = & *v_curr->left ;
			I_curr->local_data.offset = offset ;
			offset -= I_curr->local_data.slope * I_curr->length() ;	// subtract, since negative direction

			if ( I_curr->left == NULL ) break ;
			v_curr = I_curr->left ;
		}
	}
	{
		double offset = zerocost ;
		v_curr = v_zero ;
		while ( true )
		{
			I_curr = & *v_curr->right ;
			I_curr->local_data.offset = offset ;
			offset += I_curr->local_data.slope * I_curr->length() ;	// add, since positive direction

			if ( I_curr->right == NULL ) break ;
			v_curr = I_curr->right ;
		}
	}

	return value_arr ;
}







list< pair<int,int> >
MATCH( const vector<double> & P, const vector<double> & Q, double length )
{
	list< pair<int,int> > final_match ;

	/* sort indices of P and Q into place_data situated on a segment */
	segment_type segment ;
	segment[0.] ; segment[length] ;		// create the endpoints

	for ( int i=0 ; i < P.size() ; i++ )
	{
		double y = P[i] ;
		assert ( y >= 0. && y <= length ) ;
		segment[y].P_indices.push_back( i ) ;
	}
	for ( int i=0 ; i < Q.size() ; i++ )
	{
		double y = Q[i] ;
		assert ( y >= 0. && y <= length ) ;
		segment[y].Q_indices.push_back( i ) ;
	}

	/* pre-process the place_data to determine immediate matches */
	for ( segment_type::iterator
			it = segment.begin() ; it != segment.end() ; ++it )
	{
		it->second.annihilate( final_match ) ;
	}

	cout << "segment = " << segment << endl ;
	cout << "PREMATCH = " << final_match << endl ;


	Arrangement value_arr = segment_cost_characterization( segment ) ;
	list< interval_struct > interval_data ;
	Arrangement::Interval * I_curr = & *value_arr.vertices.begin()->second.left ;	// gross
	while ( true )
	{
		interval_data.push_back( I_curr->local_data ) ;
		if ( I_curr->right == NULL ) break ;
		I_curr = & *I_curr->right->right ;
	}
	cout << "lines: " << interval_data << endl ;




	/* prepare the instance graph */
	intgraph g ;
	vector< segment_type::iterator > places ;		// index of a place *is* it's graph vertex id
	/* assign a graph vertex to each node */
	for ( segment_type::iterator
			it = segment.begin() ; it != segment.end() ; ++it )
	{
		g.new_vertex() ;
		places.push_back( it ) ;
	}



	/* construct the instance graph */
	vector<int> edge_weight ;		// index is edge id

	{	// don't want to intrude on local scope
		int level = 0 ;		// someday, will be z_r, instead

		segment_type::iterator lbound, rbound ;
		rbound = segment.begin() ;
		int idx = 0 ;	// left node index
		while ( true )
		{
			lbound = rbound++ ;
			if ( rbound == segment.end() ) break ;		// no intervals left

			place_data & temp = lbound->second ;
			level += temp.P_indices.size() - temp.Q_indices.size() ;

			// possibly make an edge
			int e ;
			if ( level > 0 )
			{
				e = g.new_edge( idx, idx+1 ) ;
				edge_weight.push_back( level ) ;
			}
			else if ( level < 0 )
			{
				e = g.new_edge( idx+1, idx ) ;
				edge_weight.push_back( -level ) ;
			}

			idx++ ;
		}
	}

	cout << "graph " << g << endl ;

	list<int> order_list = toposort( g ) ;
	cout << "order list " << order_list << endl ;

	/* enumerate the places in the topological order and push the queues around */
	vector<int> order( order_list.begin(), order_list.end() ) ;
	for ( int i=0 ; i < order.size() ; i++ )
	{
		int u = order[i] ;
		segment_type::iterator it = places[u] ;
		// annihilate if you can
		place_data & source = it->second ;
		source.annihilate( final_match ) ;

		// split your lists ---
		// remember, P is *always* the positive queue after annihilation (b/c topo)
		// to be "fair", make sure to pre-pend the source list in *front* of the target one
		set<int> & u_out_edges = g.AdjV[u] ;
		for ( set<int>::iterator
				e_it = u_out_edges.begin() ; e_it != u_out_edges.end() ; ++e_it )
		{
			int e = *e_it ;
			int v = g.AdjE[e].second ;
			place_data & target = places[v]->second ;
			int capacity = edge_weight[e] ;

			list<int> & sP = source.P_indices, &tP = target.P_indices ;
			if ( capacity < sP.size() )		// then, we *have* to suffer linear time
			{
				list<int>::iterator stop ;
				stop = sP.begin() ; advance( stop, capacity ) ;
				tP.splice( tP.begin(), sP, sP.begin(), stop ) ;
			}
			else	// this branch is constant time!
			{
				tP.splice( tP.begin(), sP ) ;
			}
		}
	}

	cout << "final match " << final_match << endl ;

	return final_match ;
}




struct match_cost_func
{
	const vector<double> & P ;
	const vector<double> & Q ;

	match_cost_func( const vector<double> & P, const vector<double> & Q ) :
		P(P), Q(Q) {} ;

	double operator() ( const list< pair<int,int> > & match )
	{
		double res = 0. ;

		for ( list< pair<int,int> >::const_iterator
				it = match.begin() ; it != match.end() ; ++it )
		{
			res += abs( P[it->first] - Q[it->second] ) ;
		}

		return res ;
	}
};



struct RoadData
{
	double length ;
};

template <typename RoadType>
struct Address
{
	RoadType road ;
	double y ;

	friend ostream & operator<< ( ostream & out, const Address & a )
	{
		out << '(' << a.road << ',' << a.y << ')' ;
		return out ;
	}
};




typedef simple_graph<int,string,no_data,RoadData>		Roadmap ;
typedef typename Roadmap::vertex_iterator				node_iterator ;
typedef typename Roadmap::edge_iterator				road_iterator ;


int main( int argc, char * argv [] )
{
	int numpoints ;
	//const double LENGTH = 4.3 ;

	/* process command line */
	if ( argc != 2 || sscanf( argv[1], "%d", &numpoints ) == 0 || numpoints < 0 )
	{
		cout << "usage: " << argv[0] << " [number of points (positive)]" << endl ;
		return 0 ;
	}

	/* construct an example graph --- later take input! */
	Roadmap MAP ;
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
	typedef Address<string> StringAddr ;
	vector<StringAddr> P(numpoints), Q(numpoints) ;
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











