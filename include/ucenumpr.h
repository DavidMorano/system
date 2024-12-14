/* ucenumpr HEADER */
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
	ucenumpr

	Description:
	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMPR_INCLUDE
#define	UCENUMPR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentpr.h>		/* <- money shot */


#define	UCENUMPR_MAGIC		0x88776281


typedef UCENUMXX		ucenumpr_head ;
typedef ucentpr			ucenumpr_ent ;

#ifdef	__cplusplus
enum ucenumprmems {
    	ucenumprmem_open,
	ucenumprmem_reset,
	ucenumprmem_close,
	ucenumprmem_overlast
} ;
struct ucenumpr ;
struct ucenumpr_op {
	ucenumpr	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumpr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar *) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (ucenumpr_op) */
struct ucenumpr_co {
	ucenumpr	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumpr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ucenumpr_co) */
struct ucenumpr : ucenumxx {
	ucenumpr_op	open ;
	ucenumpr_co	reset ;
	ucenumpr_co	close ;
	ucenumpr() noex {
	    open(this,ucenumprmem_open) ;
	    reset(this,ucenumprmem_reset) ;
	    close(this,ucenumprmem_close) ;
	} ;
	ucenumpr(const ucenumpr &) = delete ;
	ucenumpr &operator = (const ucenumpr &) = delete ;
	int readent(ucenumpr_ent *,char *,int) noex ;
	void dtor() noex ;
	~ucenumpr() {
	    dtor() ;
	} ;
} ; /* end struct (ucenumpr) */
#else	/* __cplusplus */
typedef UCENUMXX	ucenumpr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ucenumpr_open(ucenumpr *,cchar *) noex ;
extern int ucenumpr_close(ucenumpr *) noex ;
extern int ucenumpr_readent(ucenumpr *,ucenumpr_ent *,char *,int) noex ;
extern int ucenumpr_reset(ucenumpr *) noex ;

EXTERNC_end


#endif /* UCENUMPR_INCLUDE */


