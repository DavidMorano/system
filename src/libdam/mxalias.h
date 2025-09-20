/* mxalias HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage a MXALIAS (Mail-X Alias) object */
/* version %I% last-modified %G% */


/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	MXALIAS_INCLUDE
#define	MXALIAS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* |time_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>
#include	<keyvals.h>


#define	MXALIAS		struct mxalias_head
#define	MXALIAS_FL	struct mxalias_flags
#define	MXALIAS_CUR	struct mxalias_cursor
#define	MXALIAS_MAGIC	0x23456112


struct mxalias_cursor {
	char		*vbuf ;
	char		**vals ;
	KEYVALS_CUR	*kvcp ;
	uint		magic ;
	int		nvals ;
	int		i ;
} ;

struct mxalias_flags {
	uint		hold:1 ;
	uint		cursoracc:1 ;
} ;

struct mxalias_head {
	cchar		*pr ;
	cchar		*username ;
	cchar		*userdname ;
	cchar		*pwd ;
	vecobj		*flp ;
	keyvals		*elp ;
	time_t		ti_access ;
	time_t		ti_check ;
	MXALIAS_FL	fl ;
	uint		magic ;
	int		ncursors ;
	int		count ;
} ;

typedef	MXALIAS		mxalias ;
typedef	MXALIAS_FL	mxalias_fl ;
typedef	MXALIAS_CUR	mxalias_cur ;

EXTERNC_begin

extern int	mxalias_open(mxalias *,cchar *,cchar *) noex ;
extern int	mxalias_count(mxalias *) noex ;
extern int	mxalias_curbegin(mxalias *,mxalias_cur *) noex ;
extern int	mxalias_curlook(mxalias *,mxalias_cur *,cchar *,int) noex ;
extern int	mxalias_curread(mxalias *,mxalias_cur *,char *,int) noex ;
extern int	mxalias_curenum(mxalias *,mxalias_cur *,
			char *,int,char *,int) noex ;
extern int	mxalias_curend(mxalias *,mxalias_cur *) noex ;
extern int	mxalias_audit(mxalias *) noex ;
extern int	mxalias_close(mxalias *) noex ;

EXTERNC_end


#endif /* MXALIAS_INCLUDE */


