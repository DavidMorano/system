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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| + |strnlen(3c)| */
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
#endif /* SUBROUTINE_CSTRNLEN */

#ifndef	SUBROUTINE_CSTRLEN
#define	SUBROUTINE_CSTRLEN
constexpr int cstrlen(cchar *sp) noex {
    	int		len = 0 ;
	while (sp[len]) len += 1 ;
	return len ;
}
#endif /* SUBROUTINE_CSTRLEN */

#ifndef	SUBROUTINE_XSTRNLEN
#define	SUBROUTINE_XSTRNLEN
constexpr int xstrnlen(cchar *sp,int sl) noex {
    	int		len = 0 ;
	if (sl >= 0) {
    	    csize	ssize = size_t(sl) ;
	    len = intconv(strnlen(sp,ssize)) ;
	} else {
	    len = intconv(strlen(sp)) ;
	}
    	return len ;
}
#endif /* SUBROUTINE_XSTRNLEN */

#ifndef	SUBROUTINE_XSTRLEN
#define	SUBROUTINE_XSTRLEN
constexpr int xstrlen(cchar *sp) noex {
    	return intconv(strlen(sp)) ;
}
#endif /* SUBROUTINE_XSTRLEN */

#ifndef	SUBROUTINE_XSTRCSPN
#define	SUBROUTINE_XSTRCSPN
constexpr int xstrcspn(cchar *sp,cchar *ss) noex {
    	return intconv(strcspn(sp,ss)) ;
}
#endif	/* SUBROUTINE_XSTRCSPN */


#endif /* __cplusplus */
#endif /* LIBUTIL_INCLUDE */


