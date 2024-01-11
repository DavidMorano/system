/* getexecname HEADER */
/* lang=C20 */

/* get a the file-name that this program was 'exec'ed from */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETEXECNAME_INCLUDE
#define	GETEXECNAME_INCLUDE


#include	<envstandards.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<utypedefs.h>
#include	<clanguage.h>


#if	(! defined(OSNAME_SunOS)) || (OSNAME_SunOS == 0)

EXTERNC_begin

extern cchar	*getexecname() noex ;

EXTERNC_end

#endif /* (! defined(OSNAME_SunOS)) || (OSNAME_SunOS == 0) */


#endif /* GETEXECNAME_INCLUDE */


