
// stdc
#include "stdlib.h"
#include "stdio.h"
#include "time.h"

// stdc++
#include <iostream>
#include <iterator>
#include <map>
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








int main( int argc, char * argv [] )
{
	int numpoints ;
	const real_type LENGTH = 4.3 ;

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

	int level = 0 ;
	p_coord_to_type::iterator fragit = segment.begin() ;
	while ( true )
	{
		p_coord_to_type::iterator lbound ;
		lbound = fragit++ ;
		if ( fragit == segment.end() ) break ;

		level += lbound->second ;	// the contribution of the point on left
		level_supp[level] += fragit->first - lbound->first ;	// the interval length

		cout << '(' << lbound->first << ',' << fragit->first << ')'
				<< " at level " << level << endl ;
	}

	cout << "level supports: " << level_supp << endl ;


	/* compute partial sums to obtain alpha and beta,
	 * and check the total support [ == 1 ? ] */
	map< int, Line<> > lineset ;
	int negative_inf = numeric_limits<int>::min() ;




	real_type total_support ;
	add_seconds<int,real_type> support_adder ;

	total_support = accumulate(
						level_supp.begin(), level_supp.end(),
						0., support_adder ) ;

	cout << "total support: " << total_support << endl ;


	return 0 ;
}











