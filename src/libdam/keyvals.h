/* keyvals HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* key-values (in-memory only) operations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	KEYVALS_INCLUDE
#define	KEYVALS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>
#include	<hdb.h>


#define	KEYVALS			struct keyvals_head
#define	KEYVALS_CUR		struct keyvals_cursor
#define	KEYVALS_MAGIC		0x31415993
#define	KEYVALS_DEFENTS		100


struct keyvals_cursor {
	hdb_cur		*ecp ;
	int		i ;
} ;

struct keyvals_head {
	vecobj		*keyp ;		/* store "keys" */
	hdb		*bykeyp ;	/* indexed by key only */
	hdb		*bykeyvalp ;	/* indexed by key-val together */
	uint		magic ;
} ;

typedef KEYVALS		keyvals ;
typedef KEYVALS_CUR	keyvals_cur ;

EXTERNC_begin

extern int keyvals_start(keyvals *,int) noex ;
extern int keyvals_add(keyvals *,int,cchar *,cchar *,int) noex ;
extern int keyvals_already(keyvals *,cchar *,int) noex ;
extern int keyvals_count(keyvals *) noex ;
extern int keyvals_curbegin(keyvals *,keyvals_cur *) noex ;
extern int keyvals_curend(keyvals *,keyvals_cur *) noex ;
extern int keyvals_curenumkey(keyvals *,keyvals_cur *,cchar **) noex ;
extern int keyvals_curenum(keyvals *,keyvals_cur *,cchar **,cchar **) noex ;
extern int keyvals_fetch(keyvals *,cchar *,keyvals_cur *,cchar **) noex ;
extern int keyvals_delset(keyvals *,int) noex ;
extern int keyvals_delkey(keyvals *,cchar *,int) noex ;
extern int keyvals_check(keyvals *,time_t) noex ;
extern int keyvals_finish(keyvals *) noex ;

EXTERNC_end


#endif /* KEYVALS_INCLUDE */


