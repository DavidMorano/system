/* ourmjd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a counted c-string to Modified-Juluan-Day (MJD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ourmjd

	Description:
	Does the given string contain a Modified-Julian-Day (MJD)
	specification?

	Synopsis:
	int ourmjd(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	>0		MJD
	0		no MJD found
	<0		error (system-return)

	Notes:
	1. This subrouine is meant to replace the older:
		hasourmjd
	subroutine.

*******************************************************************************/

#ifndef	OURMJD_INCLUDE
#define	OURMJD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	ourmjd(cchar *,int) noex ;

EXTERNC_end


#endif /* OURMJD_INCLUDE */


