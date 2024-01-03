/* linefold HEADER */
/* lang=C20 */

/* manage folding of a line */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	LINEFOLD_INCLUDE
#define	LINEFOLD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<usystem.h>
#include	<vecobj.h>
#include	<localmisc.h>


#define	LINEFOLD_MAGIC		0x88431773
#define	LINEFOLD		struct linefold_head
#define	LINEFOLD_FL		struct linefold_flags

#define	LINEFOLD_MCARRIAGE	0x0001


struct linefold_flags {
	uint		dummy:1 ;
} ;

struct linefold_head {
	uint		magic ;
	LINEFOLD_FL	f ;
	vecobj		lines ;
} ;

typedef LINEFOLD	linefold ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int linefold_start(LINEFOLD *,int,int,cchar *,int) noex ;
extern int linefold_get(LINEFOLD *,int,cchar **) noex ;
extern int linefold_getline(LINEFOLD *,int,cchar **) noex ;
extern int linefold_finish(LINEFOLD *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* LINEFOLD_INCLUDE */


