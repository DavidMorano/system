/* xwords HEADER */
/* charset=ISO8859-1 */
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

typedef	XWORDS_WORD	xwords_word ;

#ifdef	__cplusplus
enum xwordsmems {
	xwordsmem_finish,
	xwordsmem_overlast
} ;
struct xwords ;
struct xwords_co {
	xwords		*op = nullptr ;
	int		w = -1 ;
	void operator () (xwords *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (xwords_co) */
struct xwords : xwords_head {
	xwords_co	finish ;
	xwords() noex {
	    finish(this,xwordsmem_finish) ;
	} ;
	xwords(const xwords &) = delete ;
	xwords &operator = (const xwords &) = delete ;
	int start(cchar *,int) noex ;
	int get(int,cchar **) noex ;
	int del(int = -1) noex ;
	void dtor() noex ;
	~xwords() {
	    dtor() ;
	} ;
} ; /* end struct (xwords) */
#else	/* __cplusplus */
typedef XWORDS		xwords ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	xwords_start(xwords *,cchar *,int) noex ;
extern int	xwords_get(xwords *,int,cchar **) noex ;
extern int	xwords_finish(xwords *) noex ;

EXTERNC_end


#endif /* XWORDS_INCLUDE */


