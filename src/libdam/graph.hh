/* graph HEADER */
/* encoding=ISO8859-1 */
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
	graph

	Description:
	We collect some graph-related elements.

*******************************************************************************/

#ifndef	GRAPH_INCLUDE
#define	GRAPH_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vector>
#include	<list>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


struct graph_edger {
	int		src ;	/* source vertex */
	int		dst ;	/* destination vertex */
	int		weight ; /* weight of edge to this vertex */
} ;

struct graph_edge {
	int		dst ;	/* destination vertex */
	int		weight ; /* weight of edge to this vertex */
	graph_edge() noex : dst(0), weight(0) { } ;
	graph_edge(int ad,int aw = 0) noex : dst(ad), weight(aw) { } ;
} ;

struct graph_res {
	int		dist ; /* distance (summed weight) to present vertex */
	int		prev ; /* previous vertex */
	graph_res() noex : dist(0), prev(-1) { } ;
	graph_res(int ad,int ap = -1) noex : dist(ad), prev(ap) { } ;
	graph_res &operator = (const graph_res &other) noex {
	    dist = other.dist ;
	    prev = other.prev ;
	    return (*this) ;
	} ;
	graph_res &operator = (int adist) noex {
	    dist = adist ;
	    prev = -1 ;
	    return (*this) ;
	} ;
} ;


#endif /* __cplusplus */
#endif /* GRAPH_INCLUDE */


