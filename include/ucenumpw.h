/* ucenumpw HEADER */
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
	ucenumpw

	Description:
	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMPW_INCLUDE
#define	UCENUMPW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentpw.h>		/* <- money shot */


#define	UCENUMPW_MAGIC		0x88776281


typedef UCENUMXX		ucenumpw_head ;
typedef ucentpw			ucenumpw_ent ;

#ifdef	__cplusplus
enum ucenumpwmems {
    	ucenumpwmem_open,
	ucenumpwmem_reset,
	ucenumpwmem_close,
	ucenumpwmem_overlast
} ;
struct ucenumpw ;
struct ucenumpw_op {
	ucenumpw	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumpw *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar *) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (ucenumpw_op) */
struct ucenumpw_co {
	ucenumpw	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumpw *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ucenumpw_co) */
struct ucenumpw : ucenumxx {
	ucenumpw_op	open ;
	ucenumpw_co	reset ;
	ucenumpw_co	close ;
	ucenumpw() noex {
	    open(this,ucenumpwmem_open) ;
	    reset(this,ucenumpwmem_reset) ;
	    close(this,ucenumpwmem_close) ;
	} ;
	ucenumpw(const ucenumpw &) = delete ;
	ucenumpw &operator = (const ucenumpw &) = delete ;
	int readent(ucenumpw_ent *,char *,int) noex ;
	void dtor() noex ;
	~ucenumpw() {
	    dtor() ;
	} ;
} ; /* end struct (ucenumpw) */
#else	/* __cplusplus */
typedef UCENUMXX	ucenumpw ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ucenumpw_open(ucenumpw *,cchar *) noex ;
extern int ucenumpw_close(ucenumpw *) noex ;
extern int ucenumpw_readent(ucenumpw *,ucenumpw_ent *,char *,int) noex ;
extern int ucenumpw_reset(ucenumpw *) noex ;

EXTERNC_end


#endif /* UCENUMPW_INCLUDE */


