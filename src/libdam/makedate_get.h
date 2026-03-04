/* makedate_get HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the name on the MAKEDATE string */
/* version %I% last-modified %G% */


/* revision history:

	= 1988-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	makedate_get

  	Description:
	We get the date out of the "makedate" ID string.

	Synopsis:
	int makedate_get(cchar *,cchar **) noex

	Arguments:
	makedate	pointer to the MAKEDATE string
	rpp		pointer to pointer to hold result

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#ifndef	MAKEDATEGET_INCLUDE
#define	MAKEDATEGET_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	makedate_get(cchar *,cchar **) noex ;

EXTERNC_end


#endif /* MAKEDATEGET_INCLUDE */


