/* ucenumsp HEADER */
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
	ucenumsp

	Description:
	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMSP_INCLUDE
#define	UCENUMSP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentsp.h>		/* <- money shot */


#define	UCENUMSP_MAGIC		0x88776281


typedef UCENUMXX		ucenumsp_head ;
typedef ucentsp			ucenumsp_ent ;

#ifdef	__cplusplus
enum ucenumspmems {
    	ucenumspmem_open,
	ucenumspmem_reset,
	ucenumspmem_close,
	ucenumspmem_overlast
} ;
struct ucenumsp ;
struct ucenumsp_op {
	ucenumsp	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumsp *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar *) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (ucenumsp_op) */
struct ucenumsp_co {
	ucenumsp	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumsp *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ucenumsp_co) */
struct ucenumsp : ucenumxx {
	ucenumsp_op	open ;
	ucenumsp_co	reset ;
	ucenumsp_co	close ;
	ucenumsp() noex {
	    open(this,ucenumspmem_open) ;
	    reset(this,ucenumspmem_reset) ;
	    close(this,ucenumspmem_close) ;
	} ;
	ucenumsp(const ucenumsp &) = delete ;
	ucenumsp &operator = (const ucenumsp &) = delete ;
	int readent(ucenumsp_ent *,char *,int) noex ;
	void dtor() noex ;
	~ucenumsp() {
	    dtor() ;
	} ;
} ; /* end struct (ucenumsp) */
#else	/* __cplusplus */
typedef UCENUMXX	ucenumsp ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ucenumsp_open(ucenumsp *,cchar *) noex ;
extern int ucenumsp_close(ucenumsp *) noex ;
extern int ucenumsp_readent(ucenumsp *,ucenumsp_ent *,char *,int) noex ;
extern int ucenumsp_reset(ucenumsp *) noex ;

EXTERNC_end


#endif /* UCENUMSP_INCLUDE */


