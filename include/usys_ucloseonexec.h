/* usys_ucloseonexec HEADER */
/* lang=C20 */

/* operating support for retrieving the NIS domain-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an utility function for use by other emulators.

*******************************************************************************/

#ifndef	USYSUCLOSEONEXEC_INCLUDE
#define	USYSUCLOSEONEXEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>


EXTERNC_begin

extern sysret_t ucloseonexec(int,int) noex ;

EXTERNC_end


#endif /* USYSUCLOSEONEXEC_INCLUDE */


