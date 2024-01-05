/* initnow HEADER */
/* lang=C20 */

/* initiali a NOW structure */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	INITNOW_INCLUDE
#define	INITNOW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int initnow(TIMEB *,char *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* INITNOW_INCLUDE */


