/* mkprogenv INCLUDE */
/* lang=C20 */

/* make new environment for a program */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKPROGENV_INCLUDE
#define	MKPROGENV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<vechand.h>
#include	<strpack.h>


#define	MKPROGENV	struct mkprogenv_head


struct mkprogenv_head {
	cchar		*uh ;
	cchar		**envv ;
	vechand		env ;
	strpack		stores ;
	char		un[USERNAMELEN+1] ;
} ;

typedef MKPROGENV	mkprohenv ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int mkprogenv_start(MKPROGENV *,cchar **) noex ;
extern int mkprogenv_envset(MKPROGENV *,cchar *,cchar *,int) noex ;
extern int mkprogenv_getvec(MKPROGENV *,cchar ***) noex ;
extern int mkprogenv_finish(MKPROGENV *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MKPROGENV_INCLUDE */


