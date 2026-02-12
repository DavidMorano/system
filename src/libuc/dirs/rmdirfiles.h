/* rmdirfiles HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a series of possibly hierarchical directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rmdirfiles

	Description:
	Remove files from a specified directory that are older than
	a specified interval (in seconds), and which match a prefix
	c-string.

	Synopsis:
	int rmdirfiles(cchar *dname,cchar *prefix,int to) noex

	Arguments:
	dname		directory name (as a string)
	prefix		optional prefix string for selection for removal
	to		time-out interval in seconds

	Returns:
	>=0		number of files removed (deleted)
	<0		error (system-return)

*******************************************************************************/

#ifndef	RMDIRFILES_INCLUDE
#define	RMDIRFILES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int rmdirfiles(cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* RMDIRFILES_INCLUDE */


