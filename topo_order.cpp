
#include <deque>
#include <vector>
#include <list>
#include <iostream>
#include "boost/graph/vector_as_graph.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"

using namespace std ;
using namespace boost ;


int main()
{
	const char * tasks[] =
	{
		"pick up kids from school",		// 0
		"buy groceries (and snacks)",		// 1
		"get cash",				// 2
		"drop off kids at soccer",		// 3
		"cook dinner",				// 4
		"pickup kids from soccer",		// 5
		"eat dinner"				// 6
	};
	const int n_tasks = sizeof(tasks) / sizeof( char * ) ;

	vector< std::list<int> > g( n_tasks ) ;
	g[5].push_back(6) ;	// get kids from soccer before dinner
	g[4].push_back(6) ;	// cook dinner before eat dinner
	g[3].push_back(5) ;	// drop off at soccer before pickup at soccer
	g[1].push_back(4) ;	// buy groceries before cook dinner
	g[1].push_back(3) ;	// buy [snacks] before soccer
	g[0].push_back(3) ;	// get kids from school before soccer
	g[2].push_back(1) ; 	// get cash before groceries

	adjacency_list<> gg ;
	add_edge( 0, 3, gg ) ;
	add_edge( 1, 3, gg ) ;
	add_edge( 1, 4, gg ) ;
	add_edge( 2, 1, gg ) ;
	add_edge( 3, 5, gg ) ;
	add_edge( 4, 6, gg ) ;
	add_edge( 5, 6, gg ) ;

	typedef deque<int> order_struct ;
	order_struct topo1, topo2 ;

	topological_sort( g, front_inserter( topo1 ) ) ;
	topological_sort( gg, front_inserter( topo2 ) ) ;

	vector<order_struct *> orders ;
	orders.push_back( &topo1 ) ;
	orders.push_back( &topo2 ) ;
	for ( vector<order_struct *>::iterator o_it = orders.begin() ; o_it != orders.end() ; ++o_it )
	{
		order_struct & topo = **o_it ;
		for ( order_struct::iterator it = topo.begin() ; it != topo.end() ; ++it )
		{
			cout << tasks[*it] << endl ;
		}
	}
}




