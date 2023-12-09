/* spwdent INCLUDE */
/* lang=C20 */

/* SPWD structure management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SPWDENT_INCLUDE
#define	SPWDENT_INCLUDE


#include	<envstandards.h>
#include	<spwd.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#define	SPWDENT		struct spwd
#define	CSPWDENT	const struct spwd


#ifdef	__cplusplus
extern "C" {
#endif

extern int spwdent_load(SPWD *,char *,int,CSPWD *) noex ;
extern int spwdent_parse(SPWD *,char *,int,cchar *,int) noex ;
extern int spwdent_size(CSPWD *) noex ;
extern int spwdent_format(CSPWD *,char *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SPWDENT_INCLUDE */


