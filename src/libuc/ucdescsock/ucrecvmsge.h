/* ucrecvmsge HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-index operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCRECVMSGE_INCLUDE
#define	UCRECVMSGE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

EXTERNC_begin

extern int	uc_recvmsge(int,MSGHDR *,int,int,int) noex ;

EXTERNC_end


#endif /* UCRECVMSGE_INCLUDE */


