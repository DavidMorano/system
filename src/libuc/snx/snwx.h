/* snwx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* counted c-string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNWX_INCLUDE
#define	SNWX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stdarg.h>		/* |va_list| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int snwvprintf(char *,int,cchar *,va_list) noex ;
extern int snwprintf(char *,int,cchar *,...) noex ;

EXTERNC_end

#ifdef	__cplusplus

namespace libuc {
    inline int snvprintf(char *rbuf,int rlen,cchar *fmt,va_list ap) noex {
	return snwvprintf(rbuf,rlen,fmt,ap) ;
    } /* end subroutine (snvprintf) */
    extern int snprintf(char *,int,cchar *,...) noex ;
} /* end namespace (libuc) */

#endif /* __cplusplus */


#endif /* SNWX_INCLUDE */


