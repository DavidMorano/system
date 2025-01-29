/* dfsa SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++11 */

/* Depth-First-Search (visit all nodes through un-weighted graph) */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dfsa

	Descrption:
        This is the (famous) Depth-First-Search algorithm to find the
        connectivity route through an un-weighted graph.

	Features:
	recursive

	Complexity:
	O ( |v| + |e| )

	Synopsis:
	int dfsa(res_t *resp,edges_t &edges,int vertices)

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

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cinttypes>
#include	<new>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<functional>
#include	<list>
#include	<vector>
#include	<usystem.h>
#include	<localmisc.h>

module dfsa ;

/* local defines */


/* imported modules */


/* imported namespaces */

using std::vector ;			/* type */
using std::list ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

typedef dfsa_res		res_t ;
typedef dfsa_edge		edge_t ;
typedef vector<list<edge_t>>	edges_t ;
typedef list<edge_t>::iterator	edgeit_t ;


/* forward refereces */

int dfsa_visit(res_t *,edges_t &,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int dfsa(res_t *resp,edges_t &edges,int vertices) noex {
	int		rs = SR_OK ;
	for (int i = 0 ; i < vertices ; i += 1) {
	    resp[i].dist = -1 ; /* "level" */
	    resp[i].prev = -1 ;
	}
	for (int i = 0 ; i < vertices ; i += 1) {
	    resp[i].dist = 0 ; /* level */
	    rs = dfsa_visit(resp,edges,i) ;
	    if (rs < 0) break ;
	}
	return rs ;
}
/* end subroutine (dfsa) */

int dfsa_one(res_t *resp,edges_t &edges,int vertices,int u) noex {
	int		rs = SR_OK ;
	for (int i = 0 ; i < vertices ; i += 1) {
	    resp[i].dist = -1 ; /* "level" */
	    resp[i].prev = -1 ;
	}
	resp[u].dist = 0 ; /* level */
	rs = dfsa_visit(resp,edges,u) ;
	return rs ;
}
/* end subroutine (dfsa_one) */

int dfsa_visit(res_t *resp,edges_t &edges,int u) noex {
	edgeit_t	elit ; /* edge-list-iterator */
	edgeit_t	end ; /* edge-list-iterator */
	int		rs = SR_OK ;
	elit = edges[u].begin() ; /* this is 'list.begin()' */
	end = edges[u].end() ; /* this is 'list.end()' */
	while (elit != end) {
	    cint	v = (*elit).dst ; /* dst vertex */
	    if (resp[v].dist < 0) { /* not visited */
	        resp[v].dist = (resp[u].dist + 1) ;
	        resp[v].prev = u ;
	        rs = dfsa_visit(resp,edges,v) ;
	    } /* end if */
	    elit++ ;
	    if (rs < 0) break ;
	} /* end while */
	return rs ;
}
/* end subroutine (dfsa_visit) */


