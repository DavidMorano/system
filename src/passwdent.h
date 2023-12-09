/* passwdent INCLUDE */
/* lang=C20 */

/* PASSWD structure management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PASSWDENT_INCLUDE
#define	PASSWDENT_INCLUDE


#include	<envstandards.h>
#include	<pwd.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#define	PASSWDENT	struct passwd
#define	CPASSWDENT	const struct passwd


#ifdef	__cplusplus
extern "C" {
#endif

extern int passwdent_load(PASSWD *,char *,int,CPASSWD *) noex ;
extern int passwdent_parse(PASSWD *,char *,int,cchar *,int) noex ;
extern int passwdent_size(CPASSWD *) noex ;
extern int passwdent_format(CPASSWD *,char *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PASSWDENT_INCLUDE */


