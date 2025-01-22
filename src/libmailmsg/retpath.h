/* retpath HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process the "retpath" NEWS object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RETPATH_INCLUDE
#define	RETPATH_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>


#define	RETPATH		vecstr


typedef	RETPATH		retpath ;

EXTERNC_begin

extern int retpath_start(retpath *) noex ;
extern int retpath_parse(retpath *,cchar *,int) noex ;
extern int retpath_add(retpath *,cchar *,int) noex ;
extern int retpath_count(retpath *) noex ;
extern int retpath_search(retpath *,cchar *,cchar **) noex ;
extern int retpath_get(retpath *,int,cchar **) noex ;
extern int retpath_mk(retpath *,char *,int) noex ;
extern int retpath_finish(retpath *) noex ;

EXTERNC_end


#endif /* retpath_INCLUDE */


