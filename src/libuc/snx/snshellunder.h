/* snshellunder HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* counted c-string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNSHELLUNDER_INCLUDE
#define	SNSHELLUNDER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* |pid_t| */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	snshellunder(char *,int,pid_t,cchar *) noex ;

EXTERNC_end


#endif /* SNSHELLUNDER_INCLUDE */


