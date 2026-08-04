/* linehist HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* line history */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

#ifndef	LINEHIST_INCLUDE
#define	LINEHIST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<langstate.h>		/* LIBUC */


#define	LINEHIST		struct linehist_head
#define	LINEHIST_MAGIC		0x13f3c203


struct linehist_head {
	langstate	*lsp ;		/* LANGSTAT pointer */
	void		*lvp ;		/* line history */
	uint		magval ;
	char		ss[2] ;		/* search-characters */
} ; /* end struct */

typedef LINEHIST	linehist ;

EXTERNC_begin

extern int linehist_start	(linehist *,cchar *) noex ;
extern int linehist_proc	(linehist *,int,cchar *,int) noex ;
extern int linehist_count	(linehist *) noex ;
extern int linehist_get		(linehist *,int,int *) noex ;
extern int linehist_finish	(linehist *) noex ;

EXTERNC_end


#endif /* LINEHIST_INCLUDE */


