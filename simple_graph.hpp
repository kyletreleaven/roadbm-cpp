
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


struct no_data {} ;

template < typename VertexType, typename EdgeType,
	class VDataType = no_data, class EDataType = no_data >
class simple_graph
{
public :
	struct Vertex ;
	struct Edge ;

	typedef map<VertexType, Vertex>							vertex_container ;
	typedef typename vertex_container::iterator				vertex_iterator ;
	typedef typename vertex_container::const_iterator			vertex_const_iterator ;
	typedef map<EdgeType,Edge>								edge_container ;
	typedef typename edge_container::iterator					edge_iterator ;
	typedef typename edge_container::const_iterator			edge_const_iterator ;

	// class vertex_iterator : public typename map<VertexType,Vertex>::iterator {} ;
	//class edge_iterator : public typename map<EdgeType,Edge>::iterator {} ;

private :
	// containers
	vertex_container			V ;
	edge_container				E ;

public :
	const vertex_container & vertices() const
	{
		return V ;
	}

	const edge_container & edges() const
	{
		return E ;
	}

	class Vertex
	{
		friend class simple_graph ;		// automatic by parentage?
		list< edge_iterator > _out_edges ;

	public :
		VDataType local_data ;
		const list< edge_iterator > & out_edges() { return _out_edges ; }
	};

	class Edge
	{
		friend class simple_graph ;
		vertex_iterator _tail, _head ;

	public :
		EDataType local_data ;
		vertex_iterator tail() { return _tail ; }
		vertex_iterator head() { return _head ; }
	};

	vertex_iterator find_vertex( const VertexType & u ) { return V.find( u ) ; }

	vertex_iterator vertex( const VertexType & u )
	{
		V[u] ;		// create one if it does not exist
		return find_vertex( u ) ;
	}

	edge_iterator edge( const EdgeType & e ) { return E.find( e ) ; }

	edge_iterator edge( const EdgeType & e, vertex_iterator u, vertex_iterator v )
	{
		edge_iterator it = edge( e ) ;
		assert( it == E.end() ) ;

		Edge & ee = E[e] ;	// creates
		ee._tail = u ;
		ee._head = v ;

		it = edge( e ) ;
		Vertex & t = u->second ;
		t._out_edges.push_back( it ) ;

		return it ;
	}

	void remove_edge( edge_iterator it )
	{
		Edge & e = it->second ;
		Vertex & t = e._tail->second ;
		t._out_edges.erase( it ) ;

		E.erase( it ) ;
	}

	void remove_vertex( vertex_iterator it )
	{
		Vertex & u = it->second ;
		for ( edge_iterator
				e_it = u._out_edges.begin() ; e_it != u._out_edges.end() ; ++e_it )
		{
			remove_edge( e_it ) ;
		}

		V.erase( it ) ;
	}

	friend ostream & operator<< ( ostream & out, simple_graph & graph )
	{
		out << "{ " ;
		for ( typename edge_container::const_iterator
				it = graph.edges().begin() ;
				it != graph.edges().end() ;
				++it )
		{
			out << it->first << " => ("
					<< it->second.tail()->first << ','
					<< it->second.head()->first
					<< "), " ;
		}
		out << " }" ;
		return out ;
	}

};










