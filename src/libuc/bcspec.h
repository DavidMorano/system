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
	const char	*np ;
	int		nl ;
	schar		b, c, v ;
} ;

typedef struct bcspec_head	bcspec ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int bcspec_load(bcspec *,const char *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* BCSPEC_INCLUDE */


