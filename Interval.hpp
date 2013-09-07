
#pragma once

#include <iostream>

using namespace std ;



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



