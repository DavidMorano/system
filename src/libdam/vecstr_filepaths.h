/* vecstr_filepaths HEADER (Process-File-Paths) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* parse a "paths" file */
/* version %I% last-modified %G% */

/* revision history:

	= 1998-04-09, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VECSTRFILEPATHS_INCLUDE
#define	VECSTRFILEPATHS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */


EXTERNC_begin
extern int vecstr_filepaths(vecstr *,cchar *,cchar *) noex ;
EXTERNC_end


#endif /* VECSTRFILEPATHS_INCLUDE */


