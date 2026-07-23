/* snflager HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make string version of some flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	snflager

	Description:
	Ths object is used in the creation of flags strings.

******************************************************************************/

#ifndef	SNFLAGER_INCLUDE
#define	SNFLAGER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	SNFLAGER		struct snflager_head


struct snflager_head {
	char		*bp ;
	int		c ;
	int		bl ;
	int		bi ;
} ; /* end struct */

#ifdef	__cplusplus
enum snflagermems {
    	snflagermem_count,
    	snflagermem_len,
	snflagermem_finish,
	snflagermem_overlast
} ; /* end enum (snflagermems) */
struct snflager ;
struct snflager_co {
	snflager		*op = nullptr ;
	int		w = -1 ;
	void operator () (snflager *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (snflager_co) */
struct snflager : snflager_head {
	snflager_co	count ;
	snflager_co	len ;
	snflager_co	finish ;
	snflager() noex {
	    count(this,snflagermem_count) ;
	    len(this,snflagermem_len) ;
	    finish(this,snflagermem_finish) ;
	    bp = nullptr ;
	} ; /* end ctor */
	snflager(const snflager &) = delete ;
	snflager &operator = (const snflager &) = delete ;
	int start(char *,int) noex ;
	int addstr(cchar *) noex ;
	int addstrw(cchar *,int = -1) noex ;
	void dtor() noex ;
	destruct snflager() {
	    if (bp) dtor() ;
	} ; /* end destruct */
} ; /* end struct (snflager) */
#else	/* __cplusplus */
typedef SNFLAGER		snflager ;
#endif /* __cplusplus */

EXTERNC_begin

extern int snflager_start	(snflager *,char *,int) noex ;
extern int snflager_addstr	(snflager *,cchar *) noex ;
extern int snflager_addstrw	(snflager *,cchar *,int) noex ;
extern int snflager_count	(snflager *) noex ;
extern int snflager_len		(snflager *) noex ;
extern int snflager_finish	(snflager *) noex ;

EXTERNC_end


#endif /* SNFLAGER_INCLUDE */


