/* libutil HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LIBUTIL_INCLUDE
#define	LIBUTIL_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#ifndef	SUBROUTINE_CSTRNLEN
#define	SUBROUTINE_CSTRNLEN
constexpr int cstrnlen(cchar *sp,int sl = -1) noex {
    	cchar		*cp = sp ;
	while (*cp++ && sl) {
	    sl -= 1 ;
	}
	return int(cp - sp) ;
}
#endif /* __cplusplus */
#endif /* SUBROUTINE_CSTRNLEN */

#ifndef	SUBROUTINE_CSTRLEN
#define	SUBROUTINE_CSTRLEN
constexpr int cstrlen(cchar *sp) noex {
    	int		len = 0 ;
	while (sp[len]) len += 1 ;
	return len ;
}
#endif /* SUBROUTINE_CSTRLEN */


#endif /* LIBUTIL_INCLUDE */


