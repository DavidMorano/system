/* snflags HEADER */
/* lang=C20 */

/* make string version of some flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Ths object is used in the creation of flags strings.

******************************************************************************/

#ifndef	SNFLAGS_INCLUDE
#define	SNFLAGS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


#define	SNFLAGS		struct snflags_head


struct snflags_head {
	char		*bp ;
	int		c ;
	int		bl ;
	int		bi ;
} ;

typedef SNFLAGS		snflags ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int snflags_start(snflags *,char *,int) noex ;
extern int snflags_addstr(snflags *,cchar *) noex ;
extern int snflags_addstrw(snflags *,cchar *,int) noex ;
extern int snflags_finish(snflags *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SNFLAGS_INCLUDE */


