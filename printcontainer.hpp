/*
 * printmap.hpp
 *
 *  Created on: Sep 7, 2013
 *      Author: nightfender
 */

#ifndef PRINTMAP_HPP_
#define PRINTMAP_HPP_

#include "stdlib.h"
#include "stdio.h"

#include <iostream>

#include <vector>
#include <map>


using namespace std ;

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
		cout << it->first << " => " << it->second ;
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


template < typename ElementType >
ostream & operator<< ( ostream & out, const set<ElementType> & myset )
{
	out << "{ " ;
	for ( typename set<ElementType>::iterator
			it = myset.begin() ; it != myset.end() ; ++it )
	{
		out << *it << ", " ;
	}
	out << " }" ;
	return out ;
}










#endif /* PRINTMAP_HPP_ */

