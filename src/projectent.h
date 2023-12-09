/* projectent INCLUDE */
/* lang=C20 */

/* subroutines for simple PROJECT object (from UNIX� library-3c) management */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	PROJECTENT_INCLUDE
#define	PROJECTENT_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ucproject.h>


#define	PROJECTENT	struct project
#define	CPROJECTENT	const struct project


#ifdef	__cplusplus
extern "C" {
#endif

extern int projectent_load(PROJECT *,char *,int,CPROJECT *) noex ;
extern int projectent_parse(PROJECT *,char *,int,cchar *,int) noex ;
extern int projectent_size(CPROJECT *) noex ;
extern int projectent_format(CPROJECT *,char *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PROJECTENT_INCLUDE */


