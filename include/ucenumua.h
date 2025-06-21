/* ucenumua HEADER */
/* charset=ISO8859-1 */
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
	ucenumua

	Description:
	We enumerate (reentrantly and thread safely) system SYSDB-XX
	entries from the system SYSDB-XX database.

*******************************************************************************/

#ifndef	UCENUMUA_INCLUDE
#define	UCENUMUA_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<userattr.h>		/* <- ?? */

#include	<ucenumxx.h>		/* <- money shot */
#include	<ucentua.h>		/* <- money shot */


#define	UCENUMUA_MAGIC		0x88776217


typedef UCENUMXX		ucenumua_head ;
typedef ucentua			ucenumua_ent ;

#ifdef	__cplusplus
enum ucenumuamems {
    	ucenumuamem_open,
	ucenumuamem_reset,
	ucenumuamem_close,
	ucenumuamem_overlast
} ;
struct ucenumua ;
struct ucenumua_op {
	ucenumua	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumua *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (cchar *) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (ucenumua_op) */
struct ucenumua_co {
	ucenumua	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucenumua *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ucenumua_co) */
struct ucenumua : ucenumxx {
	ucenumua_op	open ;
	ucenumua_co	reset ;
	ucenumua_co	close ;
	ucenumua() noex {
	    open(this,ucenumuamem_open) ;
	    reset(this,ucenumuamem_reset) ;
	    close(this,ucenumuamem_close) ;
	} ;
	ucenumua(const ucenumua &) = delete ;
	ucenumua &operator = (const ucenumua &) = delete ;
	int readent(ucenumua_ent *,char *,int) noex ;
	void dtor() noex ;
	~ucenumua() {
	    dtor() ;
	} ;
} ; /* end struct (ucenumua) */
#else	/* __cplusplus */
typedef UCENUMXX	ucenumua ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ucenumua_open(ucenumua *,cchar *) noex ;
extern int ucenumua_close(ucenumua *) noex ;
extern int ucenumua_readent(ucenumua *,ucenumua_ent *,char *,int) noex ;
extern int ucenumua_reset(ucenumua *) noex ;

EXTERNC_end


#endif /* UCENUMUA_INCLUDE */


