/* splitaddr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Split-Address mail management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SPLITADDR_INCLUDE
#define	SPLITADDR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<vechand.h>


#define	SPLITADDR	struct splitaddr_head
#define	SPLITADDR_MAGIC	0x97543178


struct splitaddr_head {
	vechand		*comp ;		/* pointer to VECHAND object */
	cchar		*lpart ;
	cchar		*mailaddr ;
	uint		magic ;
	int		nd ;
} ;

typedef	SPLITADDR	splitaddr ;

EXTERNC_begin

extern int splitaddr_start(splitaddr *,cchar *) noex ;
extern int splitaddr_prematch(splitaddr *,splitaddr *) noex ;
extern int splitaddr_finish(splitaddr *) noex ;

EXTERNC_end


#endif /* SPLITADDR_INCLUDE */


