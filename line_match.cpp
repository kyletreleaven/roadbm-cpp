
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
#include "boost/graph/adjacency_list.hpp"

// utility
#include "printcontainer.hpp"

// objects
//#include "Interval.hpp"


using namespace std ;
using namespace boost ;

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
		place_data & temp = it->second ;
		list<int> & PP = temp.P_indices ;
		list<int> & QQ = temp.Q_indices ;

		while ( !PP.empty() && !QQ.empty() )
		{
			int i, j;
			i = PP.front() ; PP.pop_front() ;
			j = QQ.front() ; QQ.pop_front() ;
			final_match.push_back( pair<int,int>( i, j ) ) ;
		}
	}

	cout << "segment = " << segment << endl ;

	cout << "MATCH = " << final_match << endl ;


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

	cout << "P equals " << P << endl ;
	cout << "Q equals " << Q << endl ;

	/* ALGORITHM */
	MATCH( P, Q, LENGTH ) ;


#if false
	/* given the level store, we can compute a matching */
	typedef adjacency_list<> easygraph ;	// not a roadnet; doesn't need multi-ness

	/* assign a graph vertex to each node */
	typedef typename graph_traits<easygraph>::vertex_descriptor
			vertex_type ;
	map< Point, vertex_type > points_to_verts ;
	map< vertex_type, Point > verts_to_points ;
	easygraph G ;

	for ( set<Point>::iterator
			p_it = segment.begin() ;
			p_it != segment.end() ;
			++p_it )
	{
		vertex_type u = add_vertex( G ) ;
		points_to_verts[ *p_it ] = u ;
		verts_to_points[ u ] = *p_it ;
	}

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











