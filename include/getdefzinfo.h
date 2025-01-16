/* getdefzinfo HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* this is supposed to provide an OS-independent time management operation */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETDEFZINFO_INCLUDE
#define	GETDEFZINFO_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	DEFZINFO		struct defzinfo_head


struct defzinfo_head {
	int	zoff  ;		/* minutes west of GMT */
	int	isdst ;		/* is-dst flag */
} ;

typedef DEFZINFO	defzinfo ;

EXTERNC_begin

extern int	getdefzinfo(defzinfo *,char *,int,int) noex ;

EXTERNC_end


#endif /* GETDEFZINFO_INCLUDE */


