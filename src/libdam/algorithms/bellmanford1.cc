/* bellmanford-1 SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++11 */

/* Bellman-Ford algorithm for shortest path through graph */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bellmanford1

	Description:
	This is better than Dijkstra because this algorithm can
	handle negative edge weights and will detect an overall
	negative path length.

	Complexity:

	time worst	O ( |v| · |e| )
	time best	O ( |e| )
	space		O ( |v| )

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cinttypes>
#include	<new>
#include	<initializer_list>
#include	<utility>
#include	<functional>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<vector>
#include	<list>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bellmanford1.hh"


/* local defines */


/* imported namespaces */

using std::vector ;			/* type */
using std::list ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

typedef graph_res		res_t ;
typedef graph_edge		edge_t ;

typedef vector<list<edge_t>>	edges_t ;
typedef list<edge_t>::iterator	edgeit_t ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bellmanford1(res_t *resp,edges_t &edges,int vertices,int vstart) {
	edgeit_t	elit ; /* edge-list-iterator */
	edgeit_t	end ; /* edge-list-iterator */
	cint		ne = intconv(edges.size()) ;
	int		rs = SR_OK ;
	for (int i = 0 ; i < vertices ; i += 1) {
	    resp[i].dist = INT_MAX ;
	    resp[i].prev = -1 ;
	}
	resp[vstart].dist = 0 ;
	for (int i = 0 ; i < (vertices-1) ; i += 1) {
	    bool	f_nochange = true ;
	    for (int u = 0 ; u < ne ; u += 1) { /* edges(u,v) */
	        elit = edges[u].begin() ; /* this is 'list.begin()' */
	        end = edges[u].end() ; /* this is 'list.end()' */
	        while (elit != end) {
	            if (resp[u].dist != INT_MAX) {
	                cint	d = resp[u].dist ;
	                cint	w = (*elit).weight ;
	                cint	v = (*elit).dst ; /* dst vertex */
	                if ((d+w) < resp[v].dist) {
	                    resp[v].dist = (d+w) ;
	                    resp[v].prev = u ;
			    f_nochange = false ;
	                }
	            } /* end if (distance to current vertex not INF) */
	            elit++ ;
	        } /* end while */
	    } /* end for */
	    if (f_nochange) break ;
	} /* end for */
	/* this is the famous "extra cycle" to check for negative paths */
	for (int u = 0 ; u < ne ; u += 1) {
	    cint	d = resp[u].dist ;
	    cint	v = (*elit).dst ; /* dst vertex */
	    cint	w = (*elit).weight ;
	    if ((d+w) < resp[v].dist) {
	        rs = SR_LOOP ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return rs ;
}
/* end subroutine (bellmanford1) */


