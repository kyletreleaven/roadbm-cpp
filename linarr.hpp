/*
 * linarr.hpp
 *
 *  Created on: Sep 9, 2013
 *      Author: nightfender
 */

#pragma once

#include <iostream>
//#include <set>
#include <map>
#include <list>

#include "boost/scoped_ptr.hpp"


using namespace std ;
using namespace boost ;

struct no_data {} ;


template< class VDataType = no_data, class IDataType = no_data >
struct LinearArrangement
{
	struct Vertex ;
	struct Interval ;

	/* typedefs */
	typedef typename list<Interval>::iterator		interval_iter_type ;


	/* member data */
	map< double,Vertex > 	vertices ;
	list< Interval >		intervals ;

	interval_iter_type		whole_line ;

	/* definitions */
	struct Vertex
	{
		double y ;
		// stop flip-flopping; these have to be interval_iters to facilitate Interval removal
		interval_iter_type left, right ;

		VDataType local_data ;

		friend ostream &
		operator<< ( ostream & out, const Vertex & v )
		{
			out << v.y ;
			return out ;
		}
	};

	struct Interval
	{
		Vertex * left, * right ;
		IDataType local_data ;

		Interval() : left(NULL), right(NULL) {} ;
		Interval( const Vertex & l, const Vertex & r ) : left( &l ), right( &r ) {} ;
		Interval( Vertex * l, Vertex * r ) : left(l), right(r) {} ;

		friend ostream &
		operator<< ( ostream & out, const Interval & I )
		{
			out << '(' ;
			if ( I.left == NULL )
				out << "-inf" ;
			else
				out << I.left->y ;
			out << ',' ;
			if ( I.right == NULL )
				out << "+inf" ;
			else
				out << I.right->y ;
			out << ')' ;

			return out ;
		}

		/* members */
		double length() const
		{
			if ( left == NULL || right == NULL ) return -1 ;
			return right->y - left->y ;
		}
	};

	/* CTOR */
	LinearArrangement()
	{
		intervals.push_back( Interval() ) ;		// add the "whole line" interval
		whole_line = intervals.begin() ;
	} ;

	/* member functions */
	interval_iter_type find( double y ) const
	{
		typedef typename map<double,Vertex>::const_iterator map_iter_type ;

		if ( vertices.empty() )
		{
			return whole_line ;
		}
		else
		{
			map_iter_type it = vertices.upper_bound( y ) ;
			--it ;
			return it->second.right ;
		}
	}

	Interval & interval_of( double y ) const
	{
		return *find( y ) ;
	}

	Vertex & insert( double y )
	{
		// find the interval containing y
		interval_iter_type it = find( y ) ;
		Interval & curr = *it ;

		cout << y << " contained in " << curr << endl ;

		Vertex * p_u, * p_v ;
		p_u = curr.left, p_v = curr.right ;
		//if ( p_u == NULL ) cout << "p_u is NULL" << endl ;
		//if ( p_v == NULL ) cout << "p_v is NULL" << endl ;

		// create a new vertex
		assert( p_u == NULL || y != p_u->y ) ;	// make sure it's not at same location as the left bound
		Vertex & new_vert = vertices[y] ;
		new_vert.y = y ;

		// make two new Intervals, with all the right pointers
		intervals.push_front( Interval( p_u, &new_vert ) ) ;
		new_vert.left = intervals.begin() ;
		if ( p_u != NULL ) p_u->right = new_vert.left ;
		intervals.push_front( Interval( &new_vert, p_v ) ) ;
		new_vert.right = intervals.begin() ;
		if ( p_v != NULL ) p_v->left = new_vert.right ;

		cout << "splitting into: " ;
		cout << *new_vert.left << " and " << *new_vert.right << endl ;

		// destroy the old interval
		intervals.erase( it ) ;

		return new_vert ;
	}

	Vertex & vertex( double y )
	{
		Interval & I = interval_of( y ) ;
		Vertex & u = *I.left ;

		if ( y == u.y )
			return u ;
		else
			return insert( y ) ;
	}

	Interval & operator[] ( double y ) { return interval_of( y ) ; }

	/* no removal... yet! */
};


