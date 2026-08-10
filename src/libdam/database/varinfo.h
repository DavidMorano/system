/* varinfo HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* VARINFO management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VARINFO_INCLUDE
#define	VARINFO_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	VARINFOINFO_DAT	struct varinfo_data


struct varinfo_data {
	size_t		fsize ;
	time_t		mtime ;
} ; /* end struct */

typedef VARINFOINFO_DAT	varinfo_dat ;

EXTERNC_begin

extern int	varinfo_get(varinfo_dat *,cchar *,int) noex ;
extern int	varunlink(cchar *,int) noex ;

EXTERNC_end


#endif /* VARINFO_INCLUDE */


