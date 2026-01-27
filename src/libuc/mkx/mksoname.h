/* mksoname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a program-root file-path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKSONAME_INCLUDE
#define	MKSONAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	mksoname(char *,cchar *,int) noex ;

EXTERNC_end


#endif /* MKSONAME_INCLUDE */


