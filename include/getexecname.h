/* getexecname HEADER */
/* charset=ISO8859-1 */
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


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>
#include	<stdlib.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysflag.h>		/* |F_DARWIN| + |F_LINUX| */

#if	F_DARWIN || F_LINUX

EXTERNC_begin

extern cchar	*getexecname() noex ;

EXTERNC_end

#endif /* F_DARWIN || F_LINUX */


#endif /* GETEXECNAME_INCLUDE */


