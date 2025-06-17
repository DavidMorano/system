/* bfs-1 HEADER (Breath-First-Search-1) */
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
	bfs1

	Description:
	This is the (famous) Breath-First-Search algorithm to find
	the shorted (least number of hops) route through an un-weighted
	graph.

*******************************************************************************/

#ifndef	BFS1_INCLUDE
#define	BFS1_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<vector>
#include	<list>
#include	<usystem.h>


struct bfs1_edge {
	int		dst ;	/* destination vertex */
	int		weight ; /* weight of edge to this vertex */
} ;

struct bfs1_res {
	int		prev ; /* previous vertex */
	int		dist ; /* distance (summed weight) to present vertex */
} ;


extern int bfs1(bfs1_res *,
		std::vector<std::list<bfs1_edge>> *,
		int,int) ;


#endif /* __cplusplus */
#endif /* BFS1_INCLUDE */


