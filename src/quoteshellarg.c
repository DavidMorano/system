/* quoteshellarg */
/* lang=C20 */

/* subroutine to quote arguments for safe passage through a SHELL */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	quoteshellarg

	Description:
	This subroutine examines a string supplied by the caller
	and produces an output string with any necessary quotes
	applied such that interpretation by a POSIX-conforming SHELL
	will result in the original string.

	Synopsis:
	int quoteshellarg(char *dp,int dl,cchar *ap.int al,char **nvpp) noex

	Arguments:

	dp		result buffer pointer
	dl		result buffer length
	ap		shell argument to be quoted
	al		length of shell argument to be quoted
	nvpp		pointer to user pointer to receive the user supplied
			buffer address if the operation is successful
			(where did this come from?)

	Returns:
	>=0		length of used destination buffer from conversion
	<0		destination buffer was not big enough or other problem

**************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<field.h>
#include	<mkquoted.h>
#include	<localmisc.h>


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int quoteshellarg(char *dp,int dl,cchar *ap,int al,char **nvpp) noex {
	int		rs ;
	rs = mkquoted(buf,buflen,arg,arglen) ;
	if (nvpp) {
	    *nvpp = (rs >= 0) ? dp : nullptr ;
	}
	return rs ;
}
/* end subroutine (quoteshellarg) */


