/* bcspec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* load a Bible-Citation-Specification */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-02-07, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BCSPEC_INCLUDE
#define	BCSPEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	BCSPEC		struct bcspec_head


struct bcspec_head {
	cchar		*sp ;
	int		sl ;
	uchar		b, c, v ;
} ; /* end struct */

typedef BCSPEC		bcspec ;

EXTERNC_begin

extern int bcspec_load(bcspec *,cchar *,int) noex ;

EXTERNC_end


#endif /* BCSPEC_INCLUDE */


