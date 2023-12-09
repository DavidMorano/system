/* keyvals */
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
#define	KEYVALS_CUR		struct keyvals_c
#define	KEYVALS_MAGIC		0x31415993
#define	KEYVALS_DEFENTS	100


struct keyvals_c {
	HDB_CUR		ec ;
	int		i ;
} ;

struct keyvals_head {
	uint		magic ;
	VECOBJ		keys ;
	HDB		bykey ;		/* indexed by key only */
	HDB		bykeyval ;	/* indexed by key-val together */
} ;

typedef struct keyvals_head	keyvals ;
typedef struct keyvals_c	keyvals_cur ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int keyvals_start(KEYVALS *,int) ;
extern int keyvals_add(KEYVALS *,int,cchar *,cchar *,int) ;
extern int keyvals_count(KEYVALS *) ;
extern int keyvals_curbegin(KEYVALS *,KEYVALS_CUR *) ;
extern int keyvals_curend(KEYVALS *,KEYVALS_CUR *) ;
extern int keyvals_enumkey(KEYVALS *,KEYVALS_CUR *,cchar **) ;
extern int keyvals_enum(KEYVALS *,KEYVALS_CUR *,cchar **,cchar **) ;
extern int keyvals_fetch(KEYVALS *,cchar *,KEYVALS_CUR *,cchar **) ;
extern int keyvals_delset(KEYVALS *,int) ;
extern int keyvals_delkey(KEYVALS *,cchar *,int) ;
extern int keyvals_check(KEYVALS *,time_t) ;
extern int keyvals_finish(KEYVALS *) ;

#ifdef	__cplusplus
}
#endif


#endif /* KEYVALS_INCLUDE */


