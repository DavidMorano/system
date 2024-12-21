/* sysmemutil HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* retrieve the utilization (in a percentage of total) of system memory */
/* version %I% last-modified %G% */


/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSMEMUTIL_INCLUDE
#define	SYSMEMUTIL_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	SYSMEMUTIL_DAT		struct sysmemutil_data


struct sysmemutil_data {
	long		mt ;		/* pages "total" */
	long		ma ;		/* pages "available" */
	int		mu ;		/* as a percentage */
} ;

typedef SYSMEMUTIL_DAT	sysmemutil_dat ;

EXTERNC_begin

extern int sysmemutil(sysmemutil_dat *) noex ;

EXTERNC_end


#endif /* SYSMEMUTIL_INCLUDE */


