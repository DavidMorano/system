/* ucfiletimes HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* additional UNIX® support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	uc_filetime
	uc_filetimes

	Aaliases:
	uc_utime	
	uc_utimes	

  	Description:
	Additional (or supplemental) support for UNIX® limits.

	Synopsis:
	int uc_filetime		(cchar *fn,CUTIMBUF *tbp) noex
	int uc_filetimes	(cchar *fn,CTIMEVAL *tbp) noex

	Arguments:
	fn		file-name
	tbp		time-buf pointer
	tvp		time-val pointer

*******************************************************************************/

#ifndef	UCFILETIMES_INCLUDE
#define	UCFILETIMES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	uc_filetime	(cchar *,CUTIMBUF *) noex ;
extern int	uc_filetimes	(cchar *,CTIMEVAL *) noex ;

local inline int uc_utime(cchar *fn,CUTIMBUF *tbp) noex {
    	return uc_filetime(fn,tbp) ;
}
local inline int uc_utimes(cchar *fn,CTIMEVAL *tvp) noex {
    	return uc_filetimes(fn,tvp) ;
}

EXTERNC_end


#endif /* UCFILETIMES_INCLUDE */


