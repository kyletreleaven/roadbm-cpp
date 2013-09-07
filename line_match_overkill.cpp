
// stdc
#include "stdlib.h"
#include "stdio.h"
#include "time.h"

// stdc++
#include <iostream>

#include <utility>
#include <iterator>

#include <list>
#include <map>
#include <queue>		// for matching
#include <deque>
#include <numeric>		// for accumulate
#include <set>

// boost graph
#include "boost/graph/adjacency_list.hpp"

// utility
#include "printcontainer.hpp"

// objects
//#include "Interval.hpp"


using namespace std ;
using namespace boost ;


typedef double real_type ;




double uniform()
{
	return (double) rand() / RAND_MAX ;
}




// a functor for accumulate, which generically adds pair.second's
template <typename Key, typename T>
struct add_seconds
{
	typedef pair<Key,T> pair_spec ;

	T operator() ( const T & curr_sum, const pair_spec & p )
	{
		T res = curr_sum + p.second ;
		if ( false )
		{
			cout << "adding " << p.second << " to " << curr_sum
					<< "; getting " << res << endl ;
		}

		return res ;
	}
};






template <typename Domain=double, typename Range=double, typename Mult=double>
class Line
{
public :
	Range	yintercept ;
	Mult	slope ;

} ;

template <typename Domain, typename Range, typename Mult>
ostream & operator<< ( ostream & out, const Line<Domain,Range,Mult> & line )
{
	out << "{ intercept => " << line.yintercept << ", "
			<< "slope => " << line.slope << " }" ;
	return out ;
}


/* point class with type (in P or Q) annotation */
struct Point
{
	double x ;
	int type ;

	Point() {} ;		// need a default constructor
	Point ( double x, int type ) : x(x), type(type) {} ;
	Point ( const Point & p ) : x(p.x), type(p.type) {} ;
};

//Point & operator= ( Point & p1, const Point & p2 ) { return

bool operator< ( const Point & lhs, const Point & rhs ) { 	return ( lhs.x < rhs.x ) ; }

ostream & operator<< ( ostream & out, const Point & p )
{
	out << "(" << p.x << "," << p.type << ")" ;
	return out ;
}


struct Interval
{
	Point a, b ;
	Interval( const Point & a, const Point & b ) : a(a), b(b) {} ;
};

ostream & operator<< ( ostream & out, const Interval & interval )
{
	out << '(' << interval.a << ',' << interval.b << ')' ;
	return out ;
}

bool operator< ( const Interval & lhs, const Interval & rhs )
{
	if ( lhs.a < rhs.a ) return true ;
	if ( rhs.a < lhs.a ) return false ;
	if ( lhs.b < rhs.b ) return true ;
	return false ;
}





//typedef Interval<double> dInterval ;
typedef map<Interval,int> LevelStore ;

void populate_level_store( set<Point> & points, LevelStore & store, double left_bound, double right_bound )
{
	int level = 0 ;

	typedef typename set<Point>::iterator PointSetIterator ;
	PointSetIterator fragit = points.begin() ;
	while ( true )
	{
		PointSetIterator lbound = fragit++ ;
		if ( fragit == points.end() ) break ;

		int type = lbound->type ;
		Point a = *lbound, b = *fragit ;
		//double a = lbound->x, b = fragit->x ;

		level += type ;	// the contribution of the point on left
		//store[level] += b - a ; // the interval length

		//cout << '(' << a << ',' << b << ')'
				//<< " at level " << level << endl ;

		store[ Interval(a,b) ] = level ;
	}
	//cout << "level supports: " << level_supp << endl ;
}










int main( int argc, char * argv [] )
{
	int numpoints ;
	const real_type LENGTH = 4.3 ;
	const int int_negative_inf = numeric_limits<int>::min() ;

	/* process command line */
	if ( argc != 2 || sscanf( argv[1], "%d", &numpoints ) == 0 || numpoints < 0 )
	{
		cout << "usage: " << argv[0] << " [number of points (positive)]" << endl ;
		return 0 ;
	}

	/* seed random */
	srand( (unsigned int) time(NULL) ) ;

	/* generate random points */

	vector<real_type> P(numpoints), Q(numpoints) ;
	for ( int i=0 ; i < numpoints ; i++ )
	{
		P[i] = LENGTH * uniform() ;
		Q[i] = LENGTH * uniform() ;
	}

	cout << "P equals " << P << endl ;
	cout << "Q equals " << Q << endl ;

	/* ALGORITHM */
	set<Point> segment ;

	vector<real_type>::iterator it ;
	for ( it=P.begin() ; it != P.end() ; it++ ) segment.insert( Point(*it,1) ) ;
	for ( it=Q.begin() ; it != Q.end() ; it++ ) segment.insert( Point(*it,-1) ) ;

	cout << "all points: " << segment << endl ;

	LevelStore store ;
	populate_level_store( segment, store, 0, LENGTH ) ;
	// get_level_store( segment, 0, LENGTH ) ;
	cout << "level store: " << store << endl ;



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



#if false



	/* perform the matching */





	/* compute partial sums to obtain alpha and beta,
	 * and check the total support [ == 1 ? ] */
	real_type cumsum = 0. ;

	level_support_map cum_level_supp ;
	cum_level_supp[int_negative_inf] = 0. ;

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











