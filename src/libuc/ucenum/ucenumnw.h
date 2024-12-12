/* ucenumnw HEADER */
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
	ucenumnw

	Description:
	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMNW_INCLUDE
#define	UCENUMNW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentnw.h>		/* <- money shot */


#define	UCENUMNW_MAGIC		0x88776281


typedef UCENUMXX		ucenumnw_head ;
typedef ucentnw			ucenumnw_ent ;

#ifdef	__cplusplus
enum ucenumnwmems {
    	ucenumnwmem_open,
	ucenumnwmem_reset,
	ucenumnwmem_close,
	ucenumnwmem_overlast
} ;
struct ucenumnw ;
struct ucenumnw_op {
	ucenumnw	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumnw *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar *) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (ucenumnw_op) */
struct ucenumnw_co {
	ucenumnw	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumnw *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ucenumnw_co) */
struct ucenumnw : ucenumxx {
	ucenumnw_op	open ;
	ucenumnw_co	reset ;
	ucenumnw_co	close ;
	ucenumnw() noex {
	    open(this,ucenumnwmem_open) ;
	    reset(this,ucenumnwmem_reset) ;
	    close(this,ucenumnwmem_close) ;
	} ;
	ucenumnw(const ucenumnw &) = delete ;
	ucenumnw &operator = (const ucenumnw &) = delete ;
	int readent(ucenumnw_ent *,char *,int) noex ;
	void dtor() noex ;
	~ucenumnw() {
	    dtor() ;
	} ;
} ; /* end struct (ucenumnw) */
#else	/* __cplusplus */
typedef UCENUMXX	ucenumnw ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ucenumnw_open(ucenumnw *,cchar *) noex ;
extern int ucenumnw_close(ucenumnw *) noex ;
extern int ucenumnw_readent(ucenumnw *,ucenumnw_ent *,char *,int) noex ;
extern int ucenumnw_reset(ucenumnw *) noex ;

EXTERNC_end


#endif /* UCENUMNW_INCLUDE */


