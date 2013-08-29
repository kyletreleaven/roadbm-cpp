
#include <deque>
#include <vector>
#include <list>
#include <iostream>
#include "boost/graph/vector_as_graph.hpp"
#include "boost/graph/topological_sort.hpp"

using namespace std ;
using namespace boost ;


int main()
{
	const char * tasks[] =
	{
		"pick up kids from school",		// 0
		"buy groceries",			// 1
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
	g[0].push_back(3) ;	// get kids from school before soccer
	g[2].push_back(1) ; 	// get cash before groceries

	deque<int> topo_order ;
	topological_sort( g, front_inserter( topo_order ) ) ;

	for ( deque<int>::iterator it = topo_order.begin() ; it != topo_order.end() ; ++it )
	{
		cout << tasks[*it] << endl ;
	}
}
