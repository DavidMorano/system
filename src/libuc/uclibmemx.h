/* uclibmemx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCLIBMEMX_INCLUDE
#define	UCLIBMEMX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<uclibmem.h>

EXTERNC_begin

extern int uc_libmemsys(int,char **) noex ;

local inline int uc_libmemallocsys(int w,char **rpp) noex  {
    	return uc_libmemsys(w,rpp) ;
}

EXTERNC_end


#endif /* UCLIBMEMX_INCLUDE */


