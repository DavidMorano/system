/* envhelp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* help w/ handling environment */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ENVHELP_INCLUDE
#define	ENVHELP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<vechand.h>
#include	<strpack.h>


#define	ENVHELP		struct envhelp_head


struct envhelp_head {
	vechand		*elp ;		/* Environment-List-Pointer */
	strpack		*spp ;		/* String-Pack-Pointer */
} ;

#ifdef	__cplusplus
enum envhelpmems {
    	envhelpmem_sort,
	envhelpmem_count,
	envhelpmem_finish,
	envhelpmem_overlast
} ;
struct envhelp ;
struct envhelp_co {
	envhelp		*op = nullptr ;
	int		w = -1 ;
	void operator () (envhelp *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (envhelp_co) */
struct envhelp : envhelp_head {
	envhelp_co	sort ;
	envhelp_co	count ;
	envhelp_co	finish ;
	envhelp() noex {
	    sort(this,envhelpmem_sort) ;
	    count(this,envhelpmem_count) ;
	    finish(this,envhelpmem_finish) ;
	    elp = nullptr ;
	} ;
	envhelp(const envhelp &) = delete ;
	envhelp &operator = (const envhelp &) = delete ;
	int start(mainv,mainv) noex ;
	int present(cchar *,int,cchar **) noex ;
	int envset(cchar *,cchar *,int) noex ;
	int getvec(mainv *) noex ;
	void dtor() noex ;
	destruct envhelp() {
	    if (elp) dtor() ;
	} ;
} ; /* end struct (envhelp) */
#else	/* __cplusplus */
typedef ENVHELP		envhelp ;
#endif /* __cplusplus */

EXTERNC_begin

extern int envhelp_start(envhelp *,mainv,mainv) noex ;
extern int envhelp_present(envhelp *,cchar *,int,cchar **) noex ;
extern int envhelp_envset(envhelp *,cchar *,cchar *,int) noex ;
extern int envhelp_sort(envhelp *) noex ;
extern int envhelp_count(envhelp *) noex ;
extern int envhelp_getvec(envhelp *,mainv *) noex ;
extern int envhelp_finish(envhelp *) noex ;

EXTERNC_end


#endif /* ENVHELP_INCLUDE */


