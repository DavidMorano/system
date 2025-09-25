/* comsatmsg HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* create and parse COMSAT messages */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	COMSATMSG_INCLUDE
#define	COMSATMSG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	COMSATMSG	struct comsatmsg_head


struct comsatmsg_head {
    	char		*a ;		/* memory-allocation token */
	char		*username ;
	char		*fname ;
	ulong		offset ;
} ;

#ifdef	__cplusplus
enum comsatmsgmems {
    	comsatmsgmem_start,
	comsatmsgmem_finish,
	comsatmsgmem_overlast
} ;
struct comsatmsg ;
struct comsatmsg_co {
	comsatmsg	*op = nullptr ;
	int		w = -1 ;
	void operator () (comsatmsg *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (comsatmsg_co) */
struct comsatmsg : comsatmsg_head {
	comsatmsg_co	start ;
	comsatmsg_co	finish ;
	comsatmsg() noex {
	    start	(this,comsatmsgmem_start) ;
	    finish	(this,comsatmsgmem_finish) ;
	    a = nullptr ;
	} ;
	comsatmsg(const comsatmsg &) = delete ;
	comsatmsg &operator = (const comsatmsg &) = delete ;
	int rd(char *,int) noex ;
	int wr(cchar *,int = -1) noex ;
	void dtor() noex ;
	destruct comsatmsg() {
	    if (a) dtor() ;
	} ;
} ; /* end struct (comsatmsg) */
#else	/* __cplusplus */
typedef COMSATMSG	comsatmsg ;
#endif /* __cplusplus */

EXTERNC_begin

extern int comsatmsg_start(comsatmsg *) noex ;
extern int comsatmsg_rd(comsatmsg,char *,int) noex ;
extern int comsatmsg_wr(comsatmsg,cchar *,int) noex ;
extern int comsatmsg_finish(comsatmsg *) noex ;

EXTERNC_end


#endif /* COMSATMSG_INCLUDE */


