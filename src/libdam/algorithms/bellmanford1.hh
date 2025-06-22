/* bellmanford-1 HEADER */
/* charset=ISO8859-1 */
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
	Bellman-Ford (using adjacency list, C++STL style)

*******************************************************************************/

#ifndef	BELLMANFORD1_INCLUDE
#define	BELLMANFORD1_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vector>
#include	<list>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	"graph.hh"


extern int bellmanford1(graph_res *,
		std::vector<std::list<graph_edge>> &,
		int,int) ;


#endif /* __cplusplus */
#endif /* BELLMANFORD1_INCLUDE */


