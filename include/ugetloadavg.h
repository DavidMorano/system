/* ugetloadavg HEADER */
/* lang=C20 */

/* version %I% last-modified %G% */
/* auxillary operating system support */


/* revision history:

	= 1998-03-21, David A�D� Morano
	This module was originally written.

	= 2017-08-01, David A�D� Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright � 1998,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the Apple Darwin operating system.

*******************************************************************************/

#ifndef	UGETLOADAVG_INCLUDE
#define	UGETLOADAVG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#if	defined(SYSHAS_LOADAVGINT) && (SYSHAS_LOADAVGINT > 0)

/* NOTHING for now */

#else /* defined(SYSHAS_LOADAVGINT) && (SYSHAS_LOADAVGINT > 0) */

EXTERNC_begin
extern int __getloadavg(int *,int) noex ;
EXTERNC_end

#endif /* defined(SYSHAS_LOADAVGINT) && (SYSHAS_LOADAVGINT > 0) */

#ifdef	__cplusplus
namespace libuc {
    extern int	ucgetloadavg(double *,int) noex ;
}
#endif /* __cplusplus */

EXTERNC_begin
extern int	u_getloadavg(uint *,int) noex ;
EXTERNC_end


#endif /* UGETLOADAVG_INCLUDE */


