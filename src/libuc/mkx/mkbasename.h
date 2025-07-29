/* mkbasename HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* create a resuling c-string w/ the base-name of a given file-path */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 2000 David A-D- Morano.  All rights reserved. */

#ifndef	MKBASENAME_INCLUDE
#define	MKBASENAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	mkbasename(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* MKBASENAME_INCLUDE */


