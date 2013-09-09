
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

// utility
#include "printcontainer.hpp"


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









list< pair<int,int> >
MATCH( const vector<double> & P, const vector<double> & Q, double length )
{
	list< pair<int,int> > final_match ;

	/* sort indices of P and Q into place_data situated on a segment */
	typedef map< double, place_data > segment_type ;
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
	// testing
	//segment[0.].P_indices.push_back( -1 ) ;
	//segment[0.].Q_indices.push_back( -1 ) ;
	for ( segment_type::iterator
			it = segment.begin() ; it != segment.end() ; ++it )
	{
		it->second.annihilate( final_match ) ;
	}

	cout << "segment = " << segment << endl ;
	cout << "PREMATCH = " << final_match << endl ;

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



int main( int argc, char * argv [] )
{
	int numpoints ;
	const double LENGTH = 4.3 ;

	/* process command line */
	if ( argc != 2 || sscanf( argv[1], "%d", &numpoints ) == 0 || numpoints < 0 )
	{
		cout << "usage: " << argv[0] << " [number of points (positive)]" << endl ;
		return 0 ;
	}

	/* seed random */
	srand( (unsigned int) time(NULL) ) ;

	/* generate random points */
	vector<double> P(numpoints), Q(numpoints) ;
	for ( int i=0 ; i < numpoints ; i++ )
	{
		P[i] = LENGTH * uniform() ;
		Q[i] = LENGTH * uniform() ;
	}

	cout << "P equals " << P << endl ;
	cout << "Q equals " << Q << endl ;


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


	return 0 ;
}











