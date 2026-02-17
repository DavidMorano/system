/* getpwlogname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the user PASSWD entry based on logname (not UID) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HETPWLOGNAME_INCLUDE
#define	HETPWLOGNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ucentpw.h>


EXTERNC_begin

extern int	getpwlogname(ucentpw *,char *,int) noex ;

EXTERNC_end


#endif /* HETPWLOGNAME_INCLUDE */


