/* ucdescbase HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* auxillary operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ucdescbase

*******************************************************************************/

#ifndef	UCDESCBASE_INCLUDE
#define	UCDESCBASE_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libuc {
    struct ucdescbase {
	int	dummy ;
	int operator () (int) noex ;
	virtual int callstd(int) noex = 0 ;
    } ; /* end struct (ucdescbase) */
} /* end namespace */


#endif /* __cplusplus */
#endif /* UCDESCBASE_INCLUDE */


