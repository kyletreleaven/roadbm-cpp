
// stdc
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "assert.h"

// stdc++
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


struct place_key
{
	double y ;		// the ordering parameter
	queue<int> P_indices, Q_indices ;
} ;

bool operator< ( const place_key & k1, const place_key & k2 ) { return k1.y < k2.y ; }


#if false
/* define point data and its ordering */
struct point_data
{
	double x ;
	int type ;

	point_data( double x, int type ) : x(x), type(type) {} ;
};

bool operator< ( const point_data & p1, const point_data & p2 )
{
	//return this->x < p2.x ;
	return p1.x < p2.x ;
}

ostream & operator<< ( ostream & out, const point_data & pdata )
{
	out << '(' << pdata.x << ',' << pdata.type << ')' ;
	return out;
}
#endif

/* define surplus data */
struct surplus_data
{
	double a, b ;
	int level ;

	surplus_data( double a, double b, int level ) : a(a), b(b), level(level) {} ;
};

bool operator< ( const surplus_data & s1, const surplus_data & s2 )
{
	if ( s1.a < s2.a ) return true ;
	if ( s2.a < s1.a ) return false ;
	if ( s1.b < s2.b ) return true ;
	return false ;
}

ostream & operator<< ( ostream & out, const surplus_data & data )
{
	out << "{ (" << data.a << ',' << data.b << ") => " << data.level << " }" ;
	return out;
}

/* define line data */
struct line_data
{
	double slope ;
	double offset ;
};

ostream & operator<< ( ostream & out, const line_data & data )
{
	out << '<' << data.slope << "*y + " << data.offset << '>' ;
	return out;
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



struct level_data
{
};

struct interval_data
{
};





template< typename ElementType >
vector<ElementType>
shuffle( const vector<ElementType> & vec, const vector<int> & order )
{
	int vec_count = vec.size() ;
	int order_count = order.size() ;

	vector<int> inverse( vec_count, -1 ) ;
	for ( int i=0 ; i<order_count ; i++ ) inverse[ order[i] ] = i ;

	vector<ElementType> res( order_count ) ;
	for ( int j=0 ; j<vec_count ; j++ )
	{
		if ( inverse[j] < 0 ) continue ;
		res[ inverse[j] ] = vec[j] ;
	}

	return res ;
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
	vector< segment_type::iterator > places ;		// index of places *is* vertex id
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
		segment_type::iterator lbound, rbound ;
		rbound = segment.begin() ;
		int idx = 0, level = 0 ;
		while ( true )
		{
			lbound = rbound++ ;
			if ( rbound == segment.end() ) break ;

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

	/* enumerate the places in the topological order and distribute the queues */
	vector<int> order( order_list.begin(), order_list.end() ) ;
	for ( int i=0 ; i < order.size() ; i++ )
	{
		int u = order[i] ;
		segment_type::iterator it = places[u] ;
		// annihilate if you can
		place_data & source = it->second ;
		source.annihilate( final_match ) ;

		// split your lists --- remember, P is *always* the positive queue after annihilation (b/c topo)
		set<int> u_out_edges = g.AdjV[u] ;
		for ( set<int>::iterator
				e_it = u_out_edges.begin() ; e_it != u_out_edges.end() ; ++e_it )
		{
			int e = *e_it ;
			int v = g.AdjE[e].second ;
			place_data & target = places[v]->second ;
			int capacity = edge_weight[e] ;

			list<int> & sP = source.P_indices, &tP = target.P_indices ;
			if ( capacity < sP.size() )
			{
				list<int>::iterator stop ;
				stop = sP.begin() ; advance( stop, capacity ) ;
				tP.splice( tP.end(), sP, sP.begin(), stop ) ;
			}
			else	// this branch is constant time!
			{
				tP.splice( tP.end(), sP ) ;
			}
		}
	}

	cout << "final match? " << final_match << endl ;


#if false
	/* take a cumulative sum and obtain F, also mapping from level to support */
	vector< surplus_data > match_levels ;
	map< int, double > support ;
	support[INT_MIN] = 0. ;
	support[INT_MAX] = 0. ;

	vector< point_data >::const_iterator lbound, rbound ;
	rbound = segment.begin() ;
	while ( true )
	{
		lbound = rbound++ ;
		if ( rbound == segment.end() ) break ;

		static int level = 0 ;
		level += lbound->type ;
		double a = lbound->x, b = rbound->x ;

		match_levels.push_back( surplus_data( a, b, level ) ) ;
		support[level] += ( b - a ) ;
	}
	// no sorts necessary

	cout << match_levels << endl ;
	cout << support << endl ;

	/* create lines data */
	vector< line_data > lines ;
	line_data temp ;


	map< int, double>::const_iterator supp_it ;
	for ( supp_it = support.begin() ; supp_it != support.end() ; supp++ )
	{

	}

#endif

	return final_match ;
}



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

	sort( P.begin(), P.end() ) ;
	sort( Q.begin(), Q.end() ) ;
	cout << "P equals " << P << endl ;
	cout << "Q equals " << Q << endl ;

	/* ALGORITHM */
	MATCH( P, Q, LENGTH ) ;


#if false
	/* given the level store, we can compute a matching */

	cout << verts_to_points << endl ;

	/* add an edge corresponding to each interval */
	for ( LevelStore::iterator
			s_it = store.begin() ;
			s_it != store.end() ;
			++s_it )
	{
		vertex_type u, v ;
		u = points_to_verts[ s_it->first.a ] ;
		v = points_to_verts[ s_it->first.b ] ;

		if ( s_it->second > 0 ) add_edge( u, v, G ) ;
		else if ( s_it->second < 0 ) add_edge( v, u, G ) ;
	}

	/* list the edges, see if it works */
	typedef typename graph_traits<easygraph>::edge_iterator edge_iter_type ;

	pair< edge_iter_type, edge_iter_type > edge_bounds = edges( G ) ;
//	for ( edge_iter_type e_it = edge_bounds.first ; e_it != edge_bounds.second ; ++e_it )
//	{
		// cout << source( *e_it, G ) << "->" << target( *e_it, G ) << endl ;
//	}



	/* perform the matching */





	/* compute partial sums to obtain alpha and beta,
	 * and check the total support [ == 1 ? ] */
	real_type cumsum = 0. ;

	level_support_map cum_level_supp ;
	cum_level_supp[INT_MIN] = 0. ;

	for ( level_support_map::iterator it = level_supp.begin() ;
			it != level_supp.end() ; ++it )
	{
		cumsum += it->second ;
		cum_level_supp[it->first] = cumsum ;
	}

	// cout << "cumsum is: " << cum_level_supp << endl ;

	/* check total support == LENGTH */
	real_type total_support ;
	add_seconds<int,real_type> support_adder ;

	total_support = accumulate(
						level_supp.begin(), level_supp.end(),
						0., support_adder ) ;

	cout << "total support: " << total_support << endl ;


	/* construct the matching graph */
#endif


	return 0 ;
}











