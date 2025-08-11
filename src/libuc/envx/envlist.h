/* envlist HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* environment container */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ENVLIST_INCLUDE
#define	ENVLIST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<hdb.h>


#define	ENVLIST			struct envlist_head
#define	ENVLIST_CHUNKSIZE	256


struct envlist_head {
	hdb		*elp ;
	void		*store ;	/* <- this is a STRPACK object */
} ;

#ifdef	__cplusplus
enum envlistmems {
    	envlistmem_start,
	envlistmem_count,
	envlistmem_finish,
	envlistmem_overlast
} ;
struct envlist ;
struct envlist_co {
	envlist		*op = nullptr ;
	int		w = -1 ;
	void operator () (envlist *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (envlist_co) */
struct envlist : envlist_head {
	envlist_co	start ;
	envlist_co	count ;
	envlist_co	finish ;
	envlist() noex {
	    start(this,envlistmem_start) ;
	    count(this,envlistmem_count) ;
	    finish(this,envlistmem_finish) ;
	    elp = nullptr ;
	} ;
	envlist(const envlist &) = delete ;
	envlist &operator = (const envlist &) = delete ;
	int addkeyval(cchar *,cchar *,int = -1) noex ;
	int add(cchar *,int = -1) noex ;
	int present(cchar *,int = -1,cchar ** = nullptr) noex ;
	void dtor() noex ;
	destruct envlist() {
	    if (elp) dtor() ;
	} ;
} ; /* end struct (envlist) */
#else	/* __cplusplus */
typedef ENVLIST		envlist ;
#endif /* __cplusplus */

EXTERNC_begin

extern int envlist_start(envlist *,int) noex ;
extern int envlist_addkeyval(envlist *,cchar *,cchar *,int) noex ;
extern int envlist_add(envlist *,cchar *,int) noex ;
extern int envlist_present(envlist *,cchar *,int,cchar **) noex ;
extern int envlist_count(envlist *) noex ;
extern int envlist_finish(envlist *) noex ;

EXTERNC_end


#endif /* ENVLIST_INCLUDE */


