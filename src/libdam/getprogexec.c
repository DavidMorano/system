/* getprogexec */
/* lang=C++20 */

/* get the program execution name */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written as a KSH built-in command.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getprogexec

	Synopsis:
	int getprogexec(char *ebuf,int elen) noex

	Arguments:
	ebuf		buffer to store result
	elen		length of supplied retult buffer

	Returns:
	>=0		length if result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stdlib.h>
#include	<usystem.h>
#include	<sncpy.h>
#include	<localmisc.h>

#include	"getexecname.h"


/* exported subroutines */

int getprogexec(char *ebuf,int elen) noex {
	int		rs = SR_FAULT ;
	if (ebuf) {
	    cchar	*en ;
	    rs = SR_NOSYS ;
	    ebuf[0] = '\0' ;
	    if ((en = getexecname()) != nullptr) {
	        rs = sncpy1(ebuf,elen,en) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getprogexec) */


