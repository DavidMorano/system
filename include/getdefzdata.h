/* getdefzdata HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* this is supposed to provide an OS-independent time management operation */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETDEFZDATA_INCLUDE
#define	GETDEFZDATA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	DEFZDATA		struct defzdata_head


struct defzdata_head {
	int	zoff  ;		/* minutes west of GMT */
	int	isdst ;		/* is-dst flag */
} ;

typedef DEFZDATA	defzdata ;

EXTERNC_begin

extern int	getdefzdata(defzdata *,char *,int,int) noex ;

EXTERNC_end


#endif /* GETDEFZDATA_INCLUDE */


