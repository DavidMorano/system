/* usupport_matkeystr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match the key part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTMATKEYSTR_INCLUDE
#define	USUPPORTMATKEYSTR_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int	matkeystr(con mainv,cchar *,int) noex ;
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTMATKEYSTR_INCLUDE */


