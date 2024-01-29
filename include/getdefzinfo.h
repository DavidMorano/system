/* getdefzinfo HEADER */
/* lang=C++20 */

/* this is supposed to provide an OS-independent time management operation */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETDEFZINFO_INCLUDE
#define	GETDEFZINFO_INCLUDE


#include	<envstandards.h>	/* first to configure */
#include	<time.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>		/* <- |TZABBRLEN| */


#define	GETDEFZINFO		struct getdefzinfo
#define	GETDEFZINFO_ZNAMELEN	TZABBRLEN


struct getdefzinfo {
	int	zoff  ;		/* minutes west of GMT */
	int	isdst ;		/* is-dst flag */
	char	zname[GETDEFZINFO_ZNAMELEN + 1] ;
} ;

EXTERNC_begin

extern int	getdefzinfo(GETDEFZINFO *,int) noex ;

EXTERNC_end


#endif /* GETDEFZINFO_INCLUDE */


