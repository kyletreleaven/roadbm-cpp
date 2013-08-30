
// stdc
#include "stdlib.h"
#include "stdio.h"
#include "time.h"

// stdc++
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <queue>		// for matching
#include <deque>
#include <numeric>		// for accumulate


// boost graph
#include "boost/graph/adjacency_list.hpp"


using namespace std ;
using namespace boost ;


typedef double real_type ;


double uniform()
{
	return (double) rand() / RAND_MAX ;
}


///*
template <typename T>
ostream & operator<< ( ostream & out, const vector<T> & vec )
{
	typedef typename vector<T>::const_iterator iter_type ;
	for ( iter_type it = vec.begin() ; it != vec.end() ; ++it )
	{
		if ( it != vec.begin() ) out << ", " ;
		cout << *it ;
	}
	return out ;
}
//*/

///*
template <typename K, typename V>
ostream & operator<< ( ostream & out, const map<K,V> & M )
{
	typedef typename map<K,V>::const_iterator iter_type ;
	for ( iter_type it = M.begin() ; it != M.end() ; ++it )
	{
		if ( it != M.begin() ) out << ", " ;
		cout << *it ;
	}
	return out ;
}
//*/

template <typename First, typename Second>
//template <>
ostream & operator<< ( ostream & out, const pair<First,Second> & t )
{
	out << '(' << t.first << ',' << t.second << ')' ;
	return out ;
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



template <typename Range=double>
struct Interval
{
	Range a ;
	Range b ;
	Interval( Range a, Range b ) : a(a), b(b) {} ;
} ;

struct IntervalData
{
	int type ;
	int level ;
	IntervalData( int type, int level ) : type(type), level(level) {} ;
};



template <typename Range>
ostream & operator<< ( ostream & out, const Interval<Range> & interval )
{
	out << '(' << interval.a << ',' << interval.b << ')' ;
	return out ;
}

template <typename Range>
bool operator< ( const Interval<Range> & lhs, const Interval<Range> & rhs )
{
	if ( lhs.a < rhs.a ) return true ;
	if ( lhs.a > rhs.a ) return false ;
	if ( lhs.b < rhs.b ) return true ;
	return false ;
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

	/* insert points into a segment, mark type */
	typedef map<real_type,int> p_coord_to_type ;
	p_coord_to_type segment ;
	segment[0] = 0 ; segment[LENGTH] = 0 ;

	vector<real_type>::iterator it ;
	for ( it=P.begin() ; it != P.end() ; it++ ) segment[*it] = 1 ;
	for ( it=Q.begin() ; it != Q.end() ; it++ ) segment[*it] = -1 ;

	cout << "all points: " << segment << endl ;

	/* do a cumulative sum to compute level support */
	typedef map<int,real_type> level_support_map ;
	level_support_map level_supp ;

	typedef Interval<real_type> myinterval_type ;
	map< myinterval_type, int > level_store ;


	int level = 0 ;
	p_coord_to_type::iterator fragit = segment.begin() ;
	while ( true )
	{
		p_coord_to_type::iterator lbound ;
		lbound = fragit++ ;
		if ( fragit == segment.end() ) break ;

		int type = lbound->second ;
		real_type a = lbound->first, b = fragit->first ;\

		level += type ;	// the contribution of the point on left
		level_supp[level] += b - a ; // the interval length

		cout << '(' << a << ',' << b << ')'
				<< " at level " << level << endl ;

		level_store[ myinterval_type(a,b) ] = level ;
	}

	cout << "level supports: " << level_supp << endl ;
	cout << "level store: " << level_store << endl ;

	/* given the level store, we can compute a matching */


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



	return 0 ;
}











