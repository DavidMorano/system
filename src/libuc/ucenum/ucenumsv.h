/* ucenumsv HEADER */
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
	ucenumsv

	Description:
	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMSV_INCLUDE
#define	UCENUMSV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentsv.h>		/* <- money shot */


#define	UCENUMSV_MAGIC		0x88776281


typedef UCENUMXX		ucenumsv_head ;
typedef ucentsv			ucenumsv_ent ;

#ifdef	__cplusplus
enum ucenumsvmems {
    	ucenumsvmem_open,
	ucenumsvmem_reset,
	ucenumsvmem_close,
	ucenumsvmem_overlast
} ;
struct ucenumsv ;
struct ucenumsv_op {
	ucenumsv	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumsv *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar *) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (ucenumsv_op) */
struct ucenumsv_co {
	ucenumsv	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumsv *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ucenumsv_co) */
struct ucenumsv : ucenumxx {
	ucenumsv_op	open ;
	ucenumsv_co	reset ;
	ucenumsv_co	close ;
	ucenumsv() noex {
	    open(this,ucenumsvmem_open) ;
	    reset(this,ucenumsvmem_reset) ;
	    close(this,ucenumsvmem_close) ;
	} ;
	ucenumsv(const ucenumsv &) = delete ;
	ucenumsv &operator = (const ucenumsv &) = delete ;
	int readent(ucenumsv_ent *,char *,int) noex ;
	void dtor() noex ;
	~ucenumsv() {
	    dtor() ;
	} ;
} ; /* end struct (ucenumsv) */
#else	/* __cplusplus */
typedef UCENUMXX	ucenumsv ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ucenumsv_open(ucenumsv *,cchar *) noex ;
extern int ucenumsv_close(ucenumsv *) noex ;
extern int ucenumsv_readent(ucenumsv *,ucenumsv_ent *,char *,int) noex ;
extern int ucenumsv_reset(ucenumsv *) noex ;

EXTERNC_end


#endif /* UCENUMSV_INCLUDE */


