/* usys_strsub HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (STRSUB) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_strw

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSSTRSUB_INCLUDE
#define	USYSSTRSUB_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<string.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifndef	DECLARATION_STRSUB
#define	DECLARATION_STRSUB
namespace usys {
    local inline char *strsub(cchar *sp,cchar *ss) noex {
    	return strstr(sp,ss) ;
    } /* end */
} /* end namespace (usys) */
#endif /* DECLARATION_STRSUB */

#ifndef	DECLARATION_STRSUBN
#define	DECLARATION_STRSUBN
namespace usys {
    local inline char *strsubn(cchar *sp,cchar *ss,int n) noex {
    	csize nsize = size_t(n) ;
    	return strnstr(sp,ss,nsize) ;
    } /* end */
} /* end namespace (usys) */
#endif /* DECLARATION_STRSUBN */


#endif /* __cplusplus */
#endif /* USYSSTRSUB_INCLUDE */


