




typedef LinearArrangement<vertex_struct,interval_struct> Arrangement ;

Arrangement
segment_cost_characterization( const segment_type & segment )
{
	/* obtain the level supports of F */
	double zerocost = 0. ;		// cost of a matching with "zero assistance"
	map<int, double> level_support ;
	{
		int level = 0 ;
		segment_type::const_iterator lbound, rbound ;
		for ( rbound = segment.begin(), lbound = rbound++ ;
				rbound != segment.end() ; lbound++, rbound++ )
		{
			double y1, y2, len ;
			y1 = lbound->first, y2 = rbound->first, len = y2 - y1 ;
			const place_data & temp = lbound->second ;

			level += temp.P_indices.size() - temp.Q_indices.size() ;
			zerocost += len * abs( level ) ;
			level_support[level] += len ;
		}
	}
	cout << "supports " << level_support << endl ;

	/* create a linear arrangement from level supports */
	Arrangement value_arr ;

	//int numlevels = level_support.size() ;
	//list<vertex_struct> 		vertex_data ;
	//list<interval_struct> 		interval_data ;

	double length = 0. ;
	for ( map<int,double>::iterator
			it = level_support.begin() ; it != level_support.end() ; ++it )
	{
		int 	f		= it->first ;
		double	supp 	= it->second ;
		Arrangement::Vertex & v = value_arr.insert( f ) ;
		v.local_data.support = supp ;

		length += supp ;
	}

	// about to traverse the arrangement
	Arrangement::Vertex 	* v_left = & value_arr.vertices.begin()->second ;
	Arrangement::Vertex 	* v_zero ;
	Arrangement::Interval 	* I_left = & *v_left->left ;

	// some initialization
	//interval_data.push_front( interval_struct() ) ;
	I_left->local_data.slope = length ;		// length won't compile...

	Arrangement::Vertex		* v_curr ;
	Arrangement::Interval	* I_curr ;

	// traverse left-to-right to compute slopes
	{
		double slope = I_left->local_data.slope ;
		v_curr = v_left ;
		while ( true )
		{
			if ( (int) v_curr->y == 0 ) v_zero = v_curr ;		// remember the zero vertex, plox

			I_curr = & *v_curr->right ;
			slope -= 2 * v_curr->local_data.support ;
			I_curr->local_data.slope = slope ;

			if ( I_curr->right == NULL ) break ;
			v_curr = I_curr->right ;
		}
	}

	// traverse zero-to-left, then zero-to-right, to obtain offsets
	{
		double offset = zerocost ;
		v_curr = v_zero ;
		while ( true )
		{
			I_curr = & *v_curr->left ;
			I_curr->local_data.offset = offset ;
			offset -= I_curr->local_data.slope * I_curr->length() ;	// subtract, since negative direction

			if ( I_curr->left == NULL ) break ;
			v_curr = I_curr->left ;
		}
	}
	{
		double offset = zerocost ;
		v_curr = v_zero ;
		while ( true )
		{
			I_curr = & *v_curr->right ;
			I_curr->local_data.offset = offset ;
			offset += I_curr->local_data.slope * I_curr->length() ;	// add, since positive direction

			if ( I_curr->right == NULL ) break ;
			v_curr = I_curr->right ;
		}
	}

	return value_arr ;
}




























list< pair<int,int> >
MATCH( const vector<double> & P, const vector<double> & Q, double length )
{
	list< pair<int,int> > final_match ;

	/* sort indices of P and Q into place_data situated on a segment */
	segment_type segment ;
	segment[0.] ; segment[length] ;		// create the endpoints

	for ( int i=0 ; i < P.size() ; i++ )
	{
		double y = P[i] ;
		assert ( y >= 0. && y <= length ) ;
		segment[y].P_indices.push_back( i ) ;
	}
	for ( int i=0 ; i < Q.size() ; i++ )
	{
		double y = Q[i] ;
		assert ( y >= 0. && y <= length ) ;
		segment[y].Q_indices.push_back( i ) ;
	}

	/* pre-process the place_data to determine immediate matches */
	for ( segment_type::iterator
			it = segment.begin() ; it != segment.end() ; ++it )
	{
		it->second.annihilate( final_match ) ;
	}

	cout << "segment = " << segment << endl ;
	cout << "PREMATCH = " << final_match << endl ;


	Arrangement value_arr = segment_cost_characterization( segment ) ;
	list< interval_struct > interval_data ;
	Arrangement::Interval * I_curr = & *value_arr.vertices.begin()->second.left ;	// gross
	while ( true )
	{
		interval_data.push_back( I_curr->local_data ) ;
		if ( I_curr->right == NULL ) break ;
		I_curr = & *I_curr->right->right ;
	}
	cout << "lines: " << interval_data << endl ;




	/* prepare the instance graph */
	intgraph g ;
	vector< segment_type::iterator > places ;		// index of a place *is* it's graph vertex id
	/* assign a graph vertex to each node */
	for ( segment_type::iterator
			it = segment.begin() ; it != segment.end() ; ++it )
	{
		g.new_vertex() ;
		places.push_back( it ) ;
	}



	/* construct the instance graph */
	vector<int> edge_weight ;		// index is edge id

	{	// don't want to intrude on local scope
		int level = 0 ;		// someday, will be z_r, instead

		segment_type::iterator lbound, rbound ;
		rbound = segment.begin() ;
		int idx = 0 ;	// left node index
		while ( true )
		{
			lbound = rbound++ ;
			if ( rbound == segment.end() ) break ;		// no intervals left

			place_data & temp = lbound->second ;
			level += temp.P_indices.size() - temp.Q_indices.size() ;

			// possibly make an edge
			int e ;
			if ( level > 0 )
			{
				e = g.new_edge( idx, idx+1 ) ;
				edge_weight.push_back( level ) ;
			}
			else if ( level < 0 )
			{
				e = g.new_edge( idx+1, idx ) ;
				edge_weight.push_back( -level ) ;
			}

			idx++ ;
		}
	}

	cout << "graph " << g << endl ;

	list<int> order_list = toposort( g ) ;
	cout << "order list " << order_list << endl ;

	/* enumerate the places in the topological order and push the queues around */
	vector<int> order( order_list.begin(), order_list.end() ) ;
	for ( int i=0 ; i < order.size() ; i++ )
	{
		int u = order[i] ;
		segment_type::iterator it = places[u] ;
		// annihilate if you can
		place_data & source = it->second ;
		source.annihilate( final_match ) ;

		// split your lists ---
		// remember, P is *always* the positive queue after annihilation (b/c topo)
		// to be "fair", make sure to pre-pend the source list in *front* of the target one
		set<int> & u_out_edges = g.AdjV[u] ;
		for ( set<int>::iterator
				e_it = u_out_edges.begin() ; e_it != u_out_edges.end() ; ++e_it )
		{
			int e = *e_it ;
			int v = g.AdjE[e].second ;
			place_data & target = places[v]->second ;
			int capacity = edge_weight[e] ;

			list<int> & sP = source.P_indices, &tP = target.P_indices ;
			if ( capacity < sP.size() )		// then, we *have* to suffer linear time
			{
				list<int>::iterator stop ;
				stop = sP.begin() ; advance( stop, capacity ) ;
				tP.splice( tP.begin(), sP, sP.begin(), stop ) ;
			}
			else	// this branch is constant time!
			{
				tP.splice( tP.begin(), sP ) ;
			}
		}
	}

	cout << "final match " << final_match << endl ;

	return final_match ;
}




struct match_cost_func
{
	const vector<double> & P ;
	const vector<double> & Q ;

	match_cost_func( const vector<double> & P, const vector<double> & Q ) :
		P(P), Q(Q) {} ;

	double operator() ( const list< pair<int,int> > & match )
	{
		double res = 0. ;

		for ( list< pair<int,int> >::const_iterator
				it = match.begin() ; it != match.end() ; ++it )
		{
			res += abs( P[it->first] - Q[it->second] ) ;
		}

		return res ;
	}
};

