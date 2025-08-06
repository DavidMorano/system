/* snflags HEADER */
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
	snflags

	Description:
	Ths object is used in the creation of flags strings.

******************************************************************************/

#ifndef	SNFLAGS_INCLUDE
#define	SNFLAGS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	SNFLAGS		struct snflags_head


struct snflags_head {
	char		*bp ;
	int		c ;
	int		bl ;
	int		bi ;
} ;

#ifdef	__cplusplus
enum snflagsmems {
    	snflagsmem_count,
    	snflagsmem_len,
	snflagsmem_finish,
	snflagsmem_overlast
} ;
struct snflags ;
struct snflags_co {
	snflags		*op = nullptr ;
	int		w = -1 ;
	void operator () (snflags *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (snflags_co) */
struct snflags : snflags_head {
	snflags_co	count ;
	snflags_co	len ;
	snflags_co	finish ;
	snflags() noex {
	    count(this,snflagsmem_count) ;
	    len(this,snflagsmem_len) ;
	    finish(this,snflagsmem_finish) ;
	    bp = nullptr ;
	} ;
	snflags(const snflags &) = delete ;
	snflags &operator = (const snflags &) = delete ;
	int start(char *,int) noex ;
	int addstr(cchar *) noex ;
	int addstrw(cchar *,int = -1) noex ;
	void dtor() noex ;
	destruct snflags() {
	    if (bp) dtor() ;
	} ;
} ; /* end struct (snflags) */
#else	/* __cplusplus */
typedef SNFLAGS		snflags ;
#endif /* __cplusplus */

EXTERNC_begin

extern int snflags_start(snflags *,char *,int) noex ;
extern int snflags_addstr(snflags *,cchar *) noex ;
extern int snflags_addstrw(snflags *,cchar *,int) noex ;
extern int snflags_count(snflags *) noex ;
extern int snflags_len(snflags *) noex ;
extern int snflags_finish(snflags *) noex ;

EXTERNC_end


#endif /* SNFLAGS_INCLUDE */


