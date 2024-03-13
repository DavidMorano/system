/* linehist HEADER */
/* lang=C20 */

/* line history */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	LINEHIST_INCLUDE
#define	LINEHIST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<langstate.h>
#include	<localmisc.h>


#define	LINEHIST_MAGIC		0x13f3c203
#define	LINEHIST		struct linehist_head
#define	LINEHIST_LINE		struct linehist_line


struct linehist_head {
	LANGSTATE	ls ;
	void		*lvp ;		/* line history */
	uint		magic ;
	char		ss[2] ;		/* search-characters */
} ;

typedef LINEHUST	linehist ;

EXTERNC_begin

extern int linehist_start(LINEHIST *,cchar *) noex ;
extern int linehist_proc(LINEHIST *,int,cchar *,int) noex ;
extern int linehist_count(LINEHIST *) noex ;
extern int linehist_get(LINEHIST *,int,int *) noex ;
extern int linehist_finish(LINEHIST *) noex ;

EXTERNC_end


#endif /* LINEHIST_INCLUDE */


