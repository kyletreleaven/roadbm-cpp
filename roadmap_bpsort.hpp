
#pragma once

#include "Roadmap.hpp"
#include "linarr.hpp"


struct place_data
{
	typedef list<int> index_container_type ;
	index_container_type P_indices ;
	index_container_type Q_indices ;

	void annihilate( list< pair<int,int> > & match )
	{
		while ( !P_indices.empty() && !Q_indices.empty() )
		{
			int i, j ;
			i = P_indices.front() ; P_indices.pop_front() ;
			j = Q_indices.front() ; Q_indices.pop_front() ;
			match.push_back( pair<int,int>( i, j ) ) ;
		}
	}
};

ostream & operator<< ( ostream & out, const place_data & pd )
{
	out << "{ P => " << pd.P_indices
		<< ", Q => " << pd.Q_indices
		<< " }" ;
	return out ;
}

typedef LinearArrangement< place_data > segment_type ;
//typedef map< double, place_data > segment_type ;

ostream & operator<< ( ostream & out, const segment_type & seg )
{
	cout << "{ " ;

	map< double, segment_type::Vertex >::const_iterator it ;
	for ( it = seg.vertices.begin() ; it != seg.vertices.end() ; ++it )
	{
		out << it->first << " => " << it->second.local_data << ", " ;
	}
	cout << " }" ;
	return out ;
}


/* sort indices of P and Q into place_data situated on each segment of a roadmap */
template < typename RMTraits >
map< typename RMTraits::RoadID ,
	segment_type > roadmap_bpsort(		const typename RMTraits::AddressVector & P,
										const typename RMTraits::AddressVector & Q )
{
	typedef typename RMTraits::RoadID
			road_id_type ;
	map< road_id_type, segment_type > sorted ;

	/* also, will need to populate each road with a left-most/right-most */
	//segment_type segment ;
	//segment[0.] ; segment[length] ;		// create the endpoints

	/* filter P */
	for ( int i=0 ; i < P.size() ; i++ )
	{
		cout << i << ", " ;
		const road_id_type
				road 	= P[i].road ;
		double	y 		= P[i].y ;

		segment_type & segment = sorted[ road ] ;
		segment.vertex(y).local_data.P_indices.push_back( i ) ;
	}
	cout << endl ;

	/* filter Q */
	for ( int i=0 ; i < Q.size() ; i++ )
	{
		const road_id_type
				road 	= Q[i].road ;
		double	y 		= Q[i].y ;

		segment_type & segment = sorted[ road ] ;
		segment.vertex(y).local_data.Q_indices.push_back( i ) ;
	}

	return sorted ;
}

