/* hasourmjd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does the given counted c-string contains a Modified-Juluan-Day (MJD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasourmjd

	Description:
	Does the given string contain a Modified-Julian-Day (MJD)
	specification?

	Synopsis:
	bool hasourmjd(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	true		assertion succeeded
	false		assertion failed

	Notes:
	1. These (our) Modified-Julian-Dayss (MJDs) have the following
	format:
		m{dddd}
	where 'm' is the letter 'm' and {dddd} is a sring of decimal
	digits.

*******************************************************************************/

#ifndef	HASOURMJD_INCLUDE
#define	HASOURMJD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool	hasourmjd(cchar *,int) noex ;

EXTERNC_end


#endif /* HASOURMJD_INCLUDE */


