/* dupstr HEADER */
/* lang=C20 */

/* create a (writable) duplicate of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DUPSTR_INCLUDE
#define	DUPSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	DUPSTR		struct dupstr_head
#define	DUPSTR_SHORTLEN	32


struct dupstr_head {
	char		*as ;	/* allocated string */
	char		buf[DUPSTR_SHORTLEN+1] ;
} ;

#ifdef	__cplusplus
enum dupstrmems {
	dupstrmem_finish,
	dupstrmem_overlast
} ;
struct dupstr ;
struct dupstr_co {
	dupstr		*op = nullptr ;
	int		w = -1 ;
	void operator () (dupstr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (dupstr_co) */
struct dupstr : dupstr_head {
	dupstr_co	finish ;
	dupstr() noex {
	    finish(this,dupstrmem_finish) ;
	} ;
	dupstr(const dupstr &) = delete ;
	dupstr &operator = (const dupstr &) = delete ;
	int start(cchar *,int,char **) noex ;
	void dtor() noex ;
	~dupstr() noex {
	    dtor() ;
	} ;
} ; /* end struct (dupstr) */
#else	/* __cplusplus */
typedef DUPSTR		dupstr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	dupstr_start(dupstr *,cchar *,int,char **) noex ;
extern int	dupstr_finish(dupstr *) noex ;

EXTERNC_end


#endif /* DUPSTR_INCLUDE */


