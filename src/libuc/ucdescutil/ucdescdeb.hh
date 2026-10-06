/* ucdescutil HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* additional UNIX® support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	uc_seeko
	uc_fcntl

  	Description:
	Additional (or supplemental) support for UNIX® limits.

*******************************************************************************/

#ifndef	UCDESCUTIL_INCLUDE
#define	UCDESCUTIL_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* types? */
#include	<fcntl.h>		/* types? */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


namespace ucdesc {
    extern int ucdesc_revents(char *,int,cshort) noex ;
} /* end namespace (ucdesc) */


#endif /* __cplusplus */
#endif /* UCDESCUTIL_INCLUDE */


