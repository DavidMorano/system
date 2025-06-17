/* dijkstra-1 HEADER */
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
	Dijkstra-1 (shorted path through graph)

*******************************************************************************/

#ifndef	DIJKSTRA1_INCLUDE
#define	DIJKSTRA1_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vector>
#include	<list>
#include	<localmisc.h>

#include	"graph.hh"


extern int dijkstra1(graph_res *,
		std::vector<std::list<graph_edge>> &,
		int,int) ;


#endif /* __cplusplus */
#endif /* DIJKSTRA1_INCLUDE */


