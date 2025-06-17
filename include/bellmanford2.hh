/* bellmanford-2 HEADER */
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
	Bellman-Ford (using array of edges)

*******************************************************************************/

#ifndef	BELLMANFORD2_INCLUDE
#define	BELLMANFORD2_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vector>
#include	<list>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	"graph.hh"


extern int bellmanford2(graph_res *, struct graph_edger *, int,int) ;


#endif /* __cplusplus */
#endif /* BELLMANFORD2_INCLUDE */


