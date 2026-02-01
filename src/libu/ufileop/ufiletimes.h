/* ufiletimes HEADER (UNIX® file operations) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_filetime
	u_filetimes

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UFILETIMES_INCLUDE
#define	UFILETIMES_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int u_filetime(cchar *,CUTIMBUF *) noex ;
extern int u_filetimes(cchar *,CTIMEVAL *) noex ;

local inline int u_utime(cchar *fn,CUTIMBUF *tbp) noex {
    	return u_filetime(fn,tbp) ;
}
local inline int u_utimes(cchar *fn,CTIMEVAL *tvp) noex {
    	return u_filetimes(fn,tvp) ;
}

EXTERNC_end


#endif /* UFILETIMES_INCLUDE */


