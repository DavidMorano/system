/* ucmem INCLUDE */
/* lang=C20 */

/* provide some memory UNIX® operations*/
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the MacOS operating system.


*******************************************************************************/

#ifndef	UCMEM_INCLUDE
#define	UCMEM_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	uc_mincore(void *,size_t,char *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCMEM_INCLUDE */


