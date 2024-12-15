/* xwords HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* extract extra words from a single given word */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	XWORDS_INCLUDE
#define	XWORDS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	XWORDS		struct xwords_head
#define	XWORDS_WORD	struct xwords_worditem
#define	XWORDS_MAX	3


struct xwords_worditem {
	cchar		*wp ;
	int		wl ;
} ;

struct xwords_head {
	XWORDS_WORD	words[XWORDS_MAX] ;
	XWORDS_WORD	*xa ;
	int		nwords ;
} ;

typedef	XWORDS		xwords ;
typedef	XWORDS_WORD	xwords_word ;

EXTERNC_begin

extern int	xwords_start(xwords *,cchar *,int) noex ;
extern int	xwords_get(xwords *,int,cchar **) noex ;
extern int	xwords_finish(xwords *) noex ;

EXTERNC_end


#endif /* XWORDS_INCLUDE */


