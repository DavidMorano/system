/* bellmanford-2 HEADER */
/* lang=C++11 */

/* Bellman-Ford algorithm for shortest path through graph */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Bellman-Ford (using array of edges)

*******************************************************************************/

#ifndef	BELLMANFORD2_INCLUDE
#define	BELLMANFORD2_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vector>
#include	<list>
#include	<localmisc.h>

#include	"graph.hh"


extern int bellmanford2(graph_res *, struct graph_edger *, int,int) ;


#endif /* BELLMANFORD2_INCLUDE */


