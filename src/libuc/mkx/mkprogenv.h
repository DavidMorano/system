/* mkprogenv HEADER */
/* lang=C20 */

/* make new environment for a program */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKPROGENV_INCLUDE
#define	MKPROGENV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<vechand.h>
#include	<strpack.h>
#include	<localmisc.h>		/* <- |USERNAMELEN| ? */


#define	MKPROGENV	struct mkprogenv_head


struct mkprogenv_head {
	cchar		*uh ;
	cchar		**envv ;
	vechand		env ;
	strpack		stores ;
	char		un[USERNAMELEN+1] ;
} ;

typedef MKPROGENV	mkprohenv ;

EXTERNC_begin

extern int mkprogenv_start(MKPROGENV *,cchar **) noex ;
extern int mkprogenv_envset(MKPROGENV *,cchar *,cchar *,int) noex ;
extern int mkprogenv_getvec(MKPROGENV *,cchar ***) noex ;
extern int mkprogenv_finish(MKPROGENV *) noex ;

EXTERNC_end


#endif /* MKPROGENV_INCLUDE */


