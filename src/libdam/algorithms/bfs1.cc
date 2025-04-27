/* bfs-1 SUPPORT (Breath-First-Search-1) */
/* encoding=ISO8859-1 */
/* lang=C++11 */

/* Breath-First-Search (shortest route through un-weighted graph) */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bfs1

	Description:
	This is the (famous) Breath-First-Search algorithm to find
	the shorted (least number of hops) route through an un-weighted
	graph.

	Complexity:

	O ( |v| + |e| )

	Synopsis:
	int bfs1(res_t *resp,edges_t &edges,int vertices,int vstart)

	Arguments:
	resp		array of result structures:
				dist	level or distance from starting node
				prev	previous node (ancestor)
	edges		vector of lists describing adjacency edges of graph
	vertices	number of vertices in graph
	vstart		starting vertex

	Returns:
	-		result structs are filled in 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cinttypes>
#include	<new>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<functional>
#include	<list>
#include	<vector>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfs1.hh"


/* local defines */


/* imported namespaces */

using std::vector ;			/* type */
using std::list ;			/* type */


/* external subroutines */


/* local structures */

typedef bfs1_res		res_t ;
typedef bfs1_edge		edge_t ;
typedef vector<list<edge_t>>	edges_t ;
typedef list<edge_t>::iterator	edgeit_t ;


/* forward refereces */


/* exported subroutines */

int bfs1(res_t *resp,edges_t &edges,int vertices,int vstart) {
	list<int>	vq ; /* vertex-queue */
	int		rs = SR_OK ;
	edgeit_t	elit ; /* edge-list-iterator */
	edgeit_t	end ; /* edge-list-iterator */
	for (int i = 0 ; i < vertices ; i += 1) {
	    resp[i].dist = -1 ; /* "level" */
	    resp[i].prev = -1 ;
	}
	resp[vstart].dist = 0 ; /* "level" */
	resp[vstart].prev = -1 ;
	vq.push_back(vstart) ;
	while (! vq.empty()) {
	    cint	u = vq.front() ;
	    {
	        elit = edges[u].begin() ; /* this is 'list.begin()' */
	        end = edges[u].end() ; /* this is 'list.end()' */
	        while (elit != end) {
	            cint	v = (*elit).dst ; /* dst vertex */
	            if (resp[v].dist < 0) {
	                resp[v].dist = (resp[u].dist + 1) ;
	                resp[v].prev = u ;
			vq.push_back(v) ;
	            } /* end if */
	            elit++ ;
		} /* end while */
	    } /* end block */
	    vq.pop_front() ;
	} /* end while */
	return rs ;
}
/* end subroutine (bfs1) */


