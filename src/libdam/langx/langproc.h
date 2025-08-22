/* langproc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* computer-language text processor */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

#ifndef	LANGPROC_INCLUDE
#define	LANGPROC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<langstate.h>


#define	LANGPROC_MAGIC		0x13f3c203
#define	LANGPROC		struct langproc_head
#define	LANGPROC_CUR		struct langproc_cur


struct langproc_head {
	langstate	*lsp ;		/* LANGSTAT pointer */
	void		*lvp ;		/* line-value-pointer */
	uint		magic ;
} ;

struct langproc_cur {
    	int		i ;
} ;

typedef LANGPROC	langproc ;
typedef LANGPROC_CUR	langproc_cur ;

EXTERNC_begin

extern int langproc_start(langproc *,cchar *) noex ;
extern int langproc_proc(langproc *,int,cchar *,int) noex ;
extern int langproc_count(langproc *) noex ;
extern int langproc_curbegin(langproc *,langproc_cur *) noex ;
extern int langproc_curenum(langproc *,langproc_cur *,cchar **) noex ;
extern int langproc_curend(langproc *,langproc_cur *) noex ;
extern int langproc_finish(langproc *) noex ;

EXTERNC_end


#endif /* LANGPROC_INCLUDE */


