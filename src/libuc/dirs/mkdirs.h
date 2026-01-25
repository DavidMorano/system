/* mkdirs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a series of possibly hierarchical directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKDIRS_INCLUDE
#define	MKDIRS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<fcntl.h>		/* |mode_t| */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	mkdirs(cchar *,mode_t) noex ;

EXTERNC_end


#endif /* MKDIRS_INCLUDE */


