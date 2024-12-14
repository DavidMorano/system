/* ucenumpj HEADER */
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
	ucenumpj

	Description:
	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMPJ_INCLUDE
#define	UCENUMPJ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentpj.h>		/* <- money shot */


#define	UCENUMPJ_MAGIC		0x88776216


typedef UCENUMXX		ucenumpj_head ;
typedef ucentpj			ucenumpj_ent ;

#ifdef	__cplusplus
enum ucenumpjmems {
    	ucenumpjmem_open,
	ucenumpjmem_reset,
	ucenumpjmem_close,
	ucenumpjmem_overlast
} ;
struct ucenumpj ;
struct ucenumpj_op {
	ucenumpj	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumpj *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar *) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (ucenumpj_op) */
struct ucenumpj_co {
	ucenumpj	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumpj *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ucenumpj_co) */
struct ucenumpj : ucenumxx {
	ucenumpj_op	open ;
	ucenumpj_co	reset ;
	ucenumpj_co	close ;
	ucenumpj() noex {
	    open(this,ucenumpjmem_open) ;
	    reset(this,ucenumpjmem_reset) ;
	    close(this,ucenumpjmem_close) ;
	} ;
	ucenumpj(const ucenumpj &) = delete ;
	ucenumpj &operator = (const ucenumpj &) = delete ;
	int readent(ucenumpj_ent *,char *,int) noex ;
	void dtor() noex ;
	~ucenumpj() {
	    dtor() ;
	} ;
} ; /* end struct (ucenumpj) */
#else	/* __cplusplus */
typedef UCENUMXX	ucenumpj ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ucenumpj_open(ucenumpj *,cchar *) noex ;
extern int ucenumpj_close(ucenumpj *) noex ;
extern int ucenumpj_readent(ucenumpj *,ucenumpj_ent *,char *,int) noex ;
extern int ucenumpj_reset(ucenumpj *) noex ;

EXTERNC_end


#endif /* UCENUMPJ_INCLUDE */


