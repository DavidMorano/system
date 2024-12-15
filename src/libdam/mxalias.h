/* mxalias HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MXALIAS_INCLUDE
#define	MXALIAS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<usystem.h>		/* for 'ino_t' */
#include	<vecobj.h>
#include	<keyvals.h>
#include	<localmisc.h>


#define	MXALIAS		struct mxalias_head
#define	MXALIAS_CUR	struct mxalias_cursor
#define	MXALIAS_MAGIC	0x23456112


struct mxalias_cursor {
	uint		magic ;
	char		*vbuf ;
	char		**vals ;
	KEYVALS_CUR	*kvcp ;
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
	vecobj		files ;
	KEYVALS		entries ;
	struct mxalias_flags	f ;
	time_t		ti_access ;
	time_t		ti_check ;
	uint		magic ;
	int		ncursors ;
	int		count ;
} ;


#if	(! defined(MXALIAS_MASTER)) || (MXALIAS_MASTER == 0)

#ifdef	__cplusplus
extern "C" {
#endif

extern int	mxalias_open(MXALIAS *,cchar *,cchar *) ;
extern int	mxalias_count(MXALIAS *) ;
extern int	mxalias_curbegin(MXALIAS *,MXALIAS_CUR *) ;
extern int	mxalias_lookup(MXALIAS *,MXALIAS_CUR *,
			cchar *,int) ;
extern int	mxalias_read(MXALIAS *,MXALIAS_CUR *,char *,int) ;
extern int	mxalias_enum(MXALIAS *,MXALIAS_CUR *,
			char *,int,char *,int) ;
extern int	mxalias_curend(MXALIAS *,MXALIAS_CUR *) ;
extern int	mxalias_audit(MXALIAS *) ;
extern int	mxalias_close(MXALIAS *) ;

#ifdef	__cplusplus
}
#endif

#endif /* MXALIAS_MASTER */

#endif /* MXALIAS_INCLUDE */


