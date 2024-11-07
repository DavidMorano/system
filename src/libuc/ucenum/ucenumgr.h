/* ucenumgr HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* enumerate system SYSDB-XX entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucenumgr

	Description:
	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMGR_INCLUDE
#define	UCENUMGR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentgr.h>		/* <- money shot */


#define	UCENUMGR_MAGIC		0x88776281


typedef UCENUMXX		ucenumgr_head ;
typedef ucentgr			ucenumgr_ent ;

#ifdef	__cplusplus
enum ucenumgrmems {
    	ucenumgrmem_open,
	ucenumgrmem_reset,
	ucenumgrmem_close,
	ucenumgrmem_overlast
} ;
struct ucenumgr ;
struct ucenumgr_op {
	ucenumgr	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumgr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar *) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (ucenumgr_op) */
struct ucenumgr_co {
	ucenumgr	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumgr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ucenumgr_co) */
struct ucenumgr : ucenumxx {
	ucenumgr_op	open ;
	ucenumgr_co	reset ;
	ucenumgr_co	close ;
	ucenumgr() noex {
	    open(this,ucenumgrmem_open) ;
	    reset(this,ucenumgrmem_reset) ;
	    close(this,ucenumgrmem_close) ;
	} ;
	ucenumgr(const ucenumgr &) = delete ;
	ucenumgr &operator = (const ucenumgr &) = delete ;
	int readent(ucenumgr_ent *,char *,int) noex ;
	void dtor() noex ;
	~ucenumgr() noex {
	    dtor() ;
	} ;
} ; /* end struct (ucenumgr) */
#else	/* __cplusplus */
typedef UCENUMXX	ucenumgr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ucenumgr_open(ucenumgr *,cchar *) noex ;
extern int ucenumgr_close(ucenumgr *) noex ;
extern int ucenumgr_readent(ucenumgr *,ucenumgr_ent *,char *,int) noex ;
extern int ucenumgr_reset(ucenumgr *) noex ;

EXTERNC_end


#endif /* UCENUMGR_INCLUDE */


