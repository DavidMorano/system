/* mklineclean HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* clean up a line of text w/ options */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 2000 David A-D- Morano.  All rights reserved. */

#ifndef	MKLINECLEAN_INCLUDE
#define	MKLINECLEAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<linecleanopt.h>	/* line-cleaning options */


EXTERNC_begin

extern int	mklineclean(char *,int,int,cchar *,int) noex ;

EXTERNC_end


#endif /* MKLINECLEAN_INCLUDE */


