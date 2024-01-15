/* bcspec HEADER */
/* lang=C20 */

/* load a Bible-Citation-Specification */
/* version %I% last-modified %G% */


/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

#ifndef	BCSPEC_INCLUDE
#define	BCSPEC_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	BCSPEC		struct bcspec


struct bcspec_head {
	cchar		*np ;
	int		nl ;
	schar		b, c, v ;
} ;

typedef BCSPEC		bcspec ;

EXTERNC_begin

extern int bcspec_load(bcspec *,cchar *,int) noex ;

EXTERNC_end


#endif /* BCSPEC_INCLUDE */


