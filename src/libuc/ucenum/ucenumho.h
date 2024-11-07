/* ucenumho HEADER */
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
	ucenumho

	Descrption:
	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMHO_INCLUDE
#define	UCENUMHO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentho.h>		/* <- money shot */


#define	UCENUMHO_MAGIC		0x88776281


typedef UCENUMXX		ucenumho_head ;
typedef ucentho			ucenumho_ent ;

#ifdef	__cplusplus
enum ucenumhomems {
    	ucenumhomem_open,
	ucenumhomem_reset,
	ucenumhomem_close,
	ucenumhomem_overlast
} ;
struct ucenumho ;
struct ucenumho_op {
	ucenumho	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumho *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar *) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (ucenumho_op) */
struct ucenumho_co {
	ucenumho	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumho *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ucenumho_co) */
struct ucenumho : ucenumxx {
	ucenumho_op	open ;
	ucenumho_co	reset ;
	ucenumho_co	close ;
	ucenumho() noex {
	    open(this,ucenumhomem_open) ;
	    reset(this,ucenumhomem_reset) ;
	    close(this,ucenumhomem_close) ;
	} ;
	ucenumho(const ucenumho &) = delete ;
	ucenumho &operator = (const ucenumho &) = delete ;
	int readent(ucenumho_ent *,char *,int) noex ;
	void dtor() noex ;
	~ucenumho() noex {
	    dtor() ;
	} ;
} ; /* end struct (ucenumho) */
#else	/* __cplusplus */
typedef UCENUMXX	ucenumho ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ucenumho_open(ucenumho *,cchar *) noex ;
extern int ucenumho_close(ucenumho *) noex ;
extern int ucenumho_readent(ucenumho *,ucenumho_ent *,char *,int) noex ;
extern int ucenumho_reset(ucenumho *) noex ;

EXTERNC_end


#endif /* UCENUMHO_INCLUDE */


