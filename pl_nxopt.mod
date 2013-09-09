
# roadmap elements
set ROADS ;
set VERTS ;

# (r,u,v) in TOPOLOGY => r is from u->v
set TOPOLOGY within ROADS cross VERTS cross VERTS ;
# each road exactly once
check { road in ROADS } card( { (r,u,v) in TOPOLOGY : r == road } ) == 1 ;

# road surpluses
param b { ROADS }, integer ;
# zero total surplus 
check sum { r in ROADS } b[r] == 0 ;

# lines
set LINES ;
param slope { LINES } ;
param offset { LINES } ;

# rows
set ROWS within LINES cross ROADS ;
# each line assigned to exactly one road
check { line in LINES } card( { (l,r) in ROWS : l == line } ) == 1 ;

# variables
var z { ROADS } ;		# road "assistance", should be integer
var cost { ROADS } ;		# road cost, slack variables


minimize match_cost : sum { r in ROADS } cost[r] ;

subject to conservation { node in VERTS } :
	sum { (r,u,v) in TOPOLOGY : node == u } z[r] == sum { (r,u,v) in TOPOLOGY : node == v } ( z[r] + b[r] ) ;

subject to epigraph { (line,r) in ROWS } :
	cost[r] >= offset[line] + slope[line] * z[r] ;


data ;

set VERTS := 1 2 3 4 ;
set ROADS := N E S W ;
set TOPOLOGY := (N,1,2) (E,2,3) (S,3,4) (W,4,1) ;

param b := N 1 E -2 S -3 W 4 ;

set LINES := N1 N2 E1 E2 S1 S2 W1 W2 ;
param slope := N1 1 N2 -1 E1 2 E2 -2 S1 3 S2 -3 W1 1 W2 -1 ;
param offset := N1 1 N2 1 E1 0 E2 0 S1 2 S2 2 W1 1 W2 1 ;

set ROWS := (N1,N) (N2,N) (E1,E) (E2,E) (S1,S) (S2,S) (W1,W) (W2,W) ;


end ;

