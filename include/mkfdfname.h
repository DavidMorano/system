/* mkfdfname HEADER */
/* lang=C20 */

/* make an "fd" filename */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MKFDFNAME_INCLUDE
#define	MKFDFNAME_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


EXTERNC_begin

extern int mkfdname(char *,int,int) noex ;

EXTERNC_end


#endif /* MKFDFNAME_INCLUDE */


