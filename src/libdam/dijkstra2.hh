/* dijkstra-2 HEADER */
/* lang=C++11 */

/* Dijkstra (shortest path through graph) */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Dijkstra-1 (shorted path through graph)

*******************************************************************************/

#ifndef	DIJKSTRA2_INCLUDE
#define	DIJKSTRA2_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vector>
#include	<list>
#include	<localmisc.h>

#include	"graph.hh"


extern int dijkstra2(graph_res *,
		std::vector<std::list<graph_edge>> &,
		int,int) noex ;


#endif /* DIJKSTRA2_INCLUDE */


