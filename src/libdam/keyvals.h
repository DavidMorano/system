/* keyvals HEADER */
/* lang=C20 */

/* key-values file operations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	KEYVALS_INCLUDE
#define	KEYVALS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<vecobj.h>
#include	<hdb.h>
#include	<localmisc.h>


#define	KEYVALS			struct keyvals_head
#define	KEYVALS_CUR		struct keyvals_cursor
#define	KEYVALS_MAGIC		0x31415993
#define	KEYVALS_DEFENTS	100


struct keyvals_cursor {
	HDB_CUR		ec ;
	int		i ;
} ;

struct keyvals_head {
	VECOBJ		keys ;
	HDB		bykey ;		/* indexed by key only */
	HDB		bykeyval ;	/* indexed by key-val together */
	uint		magic ;
} ;

typedef KEYVALS		keyvals ;
typedef KEYVALS_CUR	keyvals_cur ;

EXTERNC_begin

extern int keyvals_start(keyvals *,int) ;
extern int keyvals_add(keyvals *,int,cchar *,cchar *,int) ;
extern int keyvals_count(keyvals *) ;
extern int keyvals_curbegin(keyvals *,keyvals_cur *) ;
extern int keyvals_curend(keyvals *,keyvals_cur *) ;
extern int keyvals_enumkey(keyvals *,keyvals_cur *,cchar **) ;
extern int keyvals_enum(keyvals *,keyvals_cur *,cchar **,cchar **) ;
extern int keyvals_fetch(keyvals *,cchar *,keyvals_cur *,cchar **) ;
extern int keyvals_delset(keyvals *,int) ;
extern int keyvals_delkey(keyvals *,cchar *,int) ;
extern int keyvals_check(keyvals *,time_t) ;
extern int keyvals_finish(keyvals *) ;

EXTERNC_end


#endif /* KEYVALS_INCLUDE */


