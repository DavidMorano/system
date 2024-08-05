/* dfs-2 HEADER */
/* lang=C++11 */

/* Depth-First-Search (visit all nodes through un-weighted graph) */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We check for discoverability of nodes in a graph.

*******************************************************************************/

#ifndef	DFS2_INCLUDE
#define	DFS2_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vector>
#include	<list>
#include	<usystem.h>


struct dfs2_edge {
	int		dst ;	/* destination vertex */
	int		weight ; /* weight of edge to this vertex */
} ;

struct dfs2_res {
	int		prev ; /* previous vertex */
	int		dist ; /* distance (summed weight) to present vertex */
} ;

extern int dfs2(dfs2_res *,
		std::vector<std::list<dfs2_edge>> *,
		int,int) ;


#endif /* DFS2_INCLUDE */


