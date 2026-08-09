/* mfsns HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* version %I% last-modified %G% */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-08-10, David A­D­ Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright © 2008,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	MFSNS_INCLUDE
#define	MFSNS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBDAM */


#define	MFSNS		struct mfsns_head
#define	MFSNS_CUR	struct mfsns_cursor
#define	MFSNS_CA	struct mfsns_calls
#define	MFSNS_MAGIC	0x99447244
/* query options */
#define	MFSNS_ONOSERV	(1<<0)		/* do not call the server */
#define	MFSNS_OPREFIX	(1<<1)		/* prefix match */


struct mfsns_cursor {
	void	*scp ;		/* SO-cursor pointer */
	uint	magval ;
} ; /* end struct */

struct mfsns_calls {
	int	(*open)(void *,cchar *) ;
	int	(*setopts)(void *,int) ;
	int	(*get)(void *,char *,int,cchar *,int) ;
	int	(*curbegin)(void *,void *) ;
	int	(*enumerate)(void *,void *,char *,int,int) ;
	int	(*curend)(void *,void *) ;
	int	(*audit)(void *) ;
	int	(*close)(void *) ;
} ; /* end struct */

struct mfsns_head {
	MODLOAD		loader ;
	MFSNS_CALLS	call ;
	void		*obj ;		/* object pointer */
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ; /* end struct */

typedef	MFSNS		mfsns ;
typedef	MFSNS_CUR	mfsns_cur ;
typedef	MFSNS_CA	mfsns_ca ;

EXTERNC_begin

extern int mfsns_open(mfsns *,cchar *) noex ;
extern int mfsns_setopts(mfsns *,int) noex ;
extern int mfsns_get(mfsns *,char *,int,cchar *,int) noex ;
extern int mfsns_curbegin(mfsns *,mfsns_cur *) noex ;
extern int mfsns_curenum(mfsns *,mfsns_cur *,char *,int,int) noex ;
extern int mfsns_curend(mfsns *,mfsns_cur *) noex ;
extern int mfsns_audit(mfsns *) noex ;
extern int mfsns_close(mfsns *) noex ;

EXTERNC_end


#endif /* MFSNS_INCLUDE */


