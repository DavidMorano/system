/* expcook HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Expand-Cookie - creates the substitution variables for cookie escapes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	EXPCOOK_INCLUDE
#define	EXPCOOK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<hdbstr.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */


#define	EXPCOOK		struct expcook_head
#define	EXPCOOK_CUR	struct expcook_cursor
#define	EXPCOOK_MAGIC	0x75837393


struct expcook_cursor {
	hdbstr_cur	*clp ;
} ; /* end struct (expcook_cursor) */

struct expcook_head {
	hdbstr		*hlp ;
	uint		magval ;
} ; /* end struct (expcook_head) */

typedef EXPCOOK_CUR	expcook_cur ;

#ifdef	__cplusplus
enum expcookmems {
    	expcookmem_start,
    	expcookmem_finish,
	expcookmem_overlast
} ; /* end enum (expcookmems) */
struct expcook ;
struct expcook_co {
	expcook		*op = nullptr ;
	int		w = -1 ;
	void operator () (expcook *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ; /* end struct (expcook_co) */
struct expcook : expcook_head {
    	expcook_co	start ;
    	expcook_co	finish ;
	expcook() noex {
	    start	(this,expcookmem_start) ;
	    finish	(this,expcookmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	int add		(cchar *,cchar *,int) noex ;
	int curbegin	(expcook_cur *) noex ;
	int curend	(expcook_cur *) noex ;
	int curenum	(expcook_cur *,char *,int) noex ;
	int findkey	(cchar *,int,cchar **) noex ;
	int delkey	(cchar *) noex ;
	int exp		(int,char *,int,cchar *,int) noex ;
	int expbuf	(int,buffer *,cchar *,int) noex ;
	void dtor() noex ;
	destruct expcook() {
	    if (magval) dtor() ;
	} ; /* end destruct */
} ; /* end struct (expcook) */
#else
typedef EXPCOOK		expcook ;
#endif /* __cplusplus */

EXTERNC_begin

extern int expcook_start	(expcook *) noex ;
extern int expcook_add		(expcook *,cchar *,cchar *,int) noex ;
extern int expcook_curbegin	(expcook *,expcook_cur *) noex ;
extern int expcook_curend	(expcook *,expcook_cur *) noex ;
extern int expcook_curenum	(expcook *,expcook_cur *,char *,int) noex ;
extern int expcook_findkey	(expcook *,cchar *,int,cchar **) noex ;
extern int expcook_delkey	(expcook *,cchar *) noex ;
extern int expcook_exp		(expcook *,int,char *,int,cchar *,int) noex ;
extern int expcook_expbuf	(expcook *,int,buffer *,cchar *,int) noex ;
extern int expcook_finish	(expcook *) noex ;

EXTERNC_end


#endif /* EXPCOOK_INCLUDE */


