/* dijkstra-1 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* Dijkstra (shortest path through graph) */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dijkstra1

	Description:
	This executes the Dijkstra algorithm to find the sorted
	path through a weighted graph.

	Fatures:
	+ pretty close to optimal speed (no queue, just linear search)

	Complexity:
	O ( |v| + |v|log|e| )

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<initializer_list>
#include	<utility>
#include	<functional>
#include	<algorithm>
#include	<vector>
#include	<list>
#include	<usystem.h>
#include	<localmisc.h>

#include	"dijkstra1.hh"


/* local defines */


/* imported namespaces */

using std::initializer_list ;		/* type */
using std::vector ;			/* type */
using std::list ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

typedef graph_res		res_t ;
typedef graph_edge		edge_t ;

typedef vector<list<edge_t>>	edges_t ;
typedef list<edge_t>::iterator	edgeit_t ;


/* forward refereces */

static int minvertex(bool *,res_t *,int) ;


/* local variables */


/* exported variables */


/* exported subroutines */

int dijkstra1(res_t *resp,edges_t &edges,int vertices,int vstart) {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	if (bool *visited ; (visited = new(nothrow) bool [vertices+1]) != np) {
	    edgeit_t	elit ; /* edge-list-iterator */
	    edgeit_t	end ; /* edge-list-iterator */
	    cint	ne = intconv(edges.size()) ;
	    for (int i = 0 ; i < vertices ; i += 1) {
		visited[i] = false ;
	    }
	    for (int i = 0 ; i < vertices ; i += 1) {
	        resp[i].dist = INT_MAX ;
	        resp[i].prev = -1 ;
	    }
	    resp[vstart].dist = 0 ;
	    for (int i = 0 ; i < (vertices-1) ; i += 1) {
	        cint	u = minvertex(visited,resp,vertices) ;
	        if ((u >= 0) && (u < ne)) {
	            elit = edges[u].begin() ; /* this is 'list.begin()' */
	            end = edges[u].end() ; /* this is 'list.end()' */
		    visited[u] = true ;
	            while (elit != end) {
	                cint	v = (*elit).dst ; /* dst vertex */
	                if ((! visited[v]) && (resp[u].dist != INT_MAX)) {
	                    cint	d = resp[u].dist ;
	                    cint	w = (*elit).weight ;
	                    if ((d+w) < resp[v].dist) {
	                        resp[v].dist = (d+w) ;
	                        resp[v].prev = u ;
	                    }
	                } /* end if (distance to current vertex not INF) */
	                elit++ ;
	            } /* end while */
	        } /* end block */
	    } /* end for */
	} else {
	    rs = SR_NOMEM ;
	}
	return rs ;
}
/* end subroutine (dijkstra1) */


/* local subroutines */

static int minvertex(bool *visited,res_t *resp,int n) {
	int		vmin = INT_MAX ;
	int		v = -1 ;
   	for (int i = 0 ; i < n ; i += 1) {
            if (! visited[i]) {
		if (resp[i].dist <= vmin) {
		    vmin = resp[i].dist ;
		    v = i ;
		}
	    }
	}
   	return v ;
}
/* end subroutine (minvertex) */


