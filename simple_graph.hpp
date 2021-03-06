
#pragma once

// stdc
#include "stdlib.h"
#include "stdio.h"
#include "assert.h"

// stdc++
#include <iostream>

#include <utility>
#include <iterator>
//#include <numeric>		// for accumulate

//#include <vector>
#include <list>
#include <map>

/* notes about map iterators
 * only the iterators of *erased* map items are invalidated!!
 */

#include <set>
#include <algorithm>

#include "boost/shared_ptr.hpp"

// utility
#include "printcontainer.hpp"

// objects
//#include "Interval.hpp"


using namespace std ;
using namespace boost ;


struct no_graph_data {} ;

template < typename VertexType, typename EdgeType,
	class VDataType = no_graph_data, class EDataType = no_graph_data >
struct simple_graph
{
	struct Vertex ;
	struct Edge ;

	typedef map<VertexType, Vertex>							vertex_container ;
	typedef typename vertex_container::iterator				vertex_iterator ;
	typedef map<EdgeType,Edge>								edge_container ;
	typedef typename edge_container::iterator					edge_iterator ;

	// class vertex_iterator : public typename map<VertexType,Vertex>::iterator {} ;
	//class edge_iterator : public typename map<EdgeType,Edge>::iterator {} ;

	// containers
	vertex_container			V ;
	edge_container				E ;

	struct Vertex
	{
		list< edge_iterator > out_edges ;
		VDataType local_data ;
	};

	struct Edge
	{
		vertex_iterator tail, head ;
		EDataType local_data ;
	};

	vertex_iterator find_vertex( const VertexType & u ) { return V.find( u ) ; }

	vertex_iterator vertex( const VertexType & u )
	{
		V[u] ;		// create one if it does not exist
		return find_vertex( u ) ;
	}

	edge_iterator find_edge( const EdgeType & e ) { return E.find( e ) ; }

	edge_iterator edge( const EdgeType & e, vertex_iterator u, vertex_iterator v )
	{
		edge_iterator it = find_edge( e ) ;
		assert( it == E.end() ) ;

		Edge & edge = E[e] ;	// creates
		edge.tail = u ;
		edge.head = v ;

		it = find_edge( e ) ;
		Vertex & t = u->second ;
		t.out_edges.push_back( it ) ;

		return it ;
	}

	void remove_edge( edge_iterator it )
	{
		Edge & edge = it->second ;
		Vertex & t = edge.tail->second ;
		t.out_edges.erase( it ) ;

		E.erase( it ) ;
	}

	void remove_vertex( vertex_iterator it )
	{
		Vertex & u = it->second ;
		for ( edge_iterator
				e_it = u.out_edges.begin() ; e_it != u.out_edges.end() ; ++e_it )
		{
			remove_edge( e_it ) ;
		}

		V.erase( it ) ;
	}

	friend ostream & operator<< ( ostream & out, simple_graph & graph )
	{
		out << "{ " ;
		for ( edge_iterator
				it = graph.E.begin() ; it != graph.E.end() ; ++it )
		{
			out << it->first << " => ("
					<< it->second.tail->first << ','
					<< it->second.head->first
					<< "), " ;
		}
		out << " }" ;
		return out ;
	}

};



