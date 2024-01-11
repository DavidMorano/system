/* strnxsub SUPPORT */
/* lang=C++20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */

#define	CF_STRNSTR	1		/* used standard |strnstr(3c)| */
#define	CF_STRCASESTR	1		/* used standard |strcasestr(3c)| */

/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

	= 2023-12-20, David A­D­ Morano
	Everyone (!) has |strnstr(3c)| now. After all of these
	years, I finally discovered that. I must have actually
	written this subroutine back in the 1980s or so when
	|strnstr(3c)| did not yet exist. So I am adding a compile-time
	option (see above in this file) to use the UNIX® C-lang
	Standard-Library subroutine |strnstr(3c)| when calling our
	own |strnsub(3uc)|. Oops! Note that the function signature 
	on the standard |strnstr(3c)| is NOT the same as our own
	old |strnsub(3uc)|, so I adapt it to match. Everything diddo
	w/ the (now) standard |strcasestr(3c)|.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strn{x}sub

	Description:
	This subroutine determines if the parameter string (argument
	's2') is or is not a substring specified by the first two
	arguments.  This subroutine either returns a pointer to the
	the begining of the found substring or NULL if not found.

	Synopsis:
	char *strn{x}sub(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined
	ss	null terminated substring to search for

	Returns:
	-	pointer to found substring
	NULL	substring was not found

*******************************************************************************/

#include	<envstandards.h>
#include	<cstring>		/* <- |strstr(3c)| + |strnstr(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<nleadstr.h>
#include	<localmisc.h>

#include	"strn.h"


/* local defines */

#ifndef	CF_STRNSTR
#define	CF_STRNSTR	0
#endif

#ifndef	CF_STRCASESTR
#define	CF_STRCASESTR	0
#endif


/* local namespaces */


/* local typedefs */

typedef char	*charp ;


/* forward references */

static char	*strnsub_local(cchar *,int,cchar *) noex ;
static char	*strncasesub_local(cchar *,int,cchar *,int) noex ;


/* local variables */

constexpr bool	f_strnstr = CF_STRNSTR ;
constexpr bool	f_strcasestr = CF_STRCASESTR ;


/* exported subroutines */

char *strnsub(cchar *sp,int sl,cchar *ss) noex {
	char		*rp = nullptr ;
	if (sp && ss) {
	    if (sl >= 0) {
		if constexpr (f_strnstr) {
		    rp = strnstr(sp,ss,sl) ;
		} else {
	            rp = strnsub_local(sp,sl,ss) ;
		}
	    } else {
	        rp = strstr(sp,ss) ;
	    }
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (strnsub) */

char *strncasesub(cchar *sp,int sl,cchar *s2) noex {
	char		*rp = nullptr ;
	if (sp && s2) {
	    cint	s2len = strlen(s2) ;
	    rp = charp(sp) ;
	    if (s2len > 0) {
		if (sl >= 0) {
		    rp = strncasesub_local(sp,sl,s2,s2len) ;
		} else {
		    if constexpr (f_strcasestr) {
		        rp = strcasestr(sp,s2) ;
		    } else {
		        rp = strncasesub_local(sp,sl,s2,s2len) ;
		    } /* end if-constexpr) */
		} /* end if */
	    } /* end if (positive) */
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (strncasesub) */


/* local subroutines */

char *strnsub_local(cchar *sp,int sl,cchar *s2) noex {
	cint		s2len = strlen(s2) ;
	char		*rp = (char *) sp ;
	if (s2len > 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (s2len <= sl) {
	        int	i ; /* <- used afterwards */
		bool	f = false ;
	        for (i = 0 ; i <= (sl-s2len) ; i += 1) {
	            f = ((s2len == 0) || (sp[i] == s2[0])) ;
	            f = f && (strncmp((sp+i),s2,s2len) == 0) ;
		    if (f) break ;
	        } /* end for */
	        rp = charp((f) ? (sp+i) : nullptr) ;
	    } else {
	        rp = nullptr ;
	    }
	} /* end if (positive) */
	return rp ;
}
/* end subroutine (strnsub_local) */

char *strncasesub_local(cchar *sp,int sl,cchar *ss,int s2len) noex {
	char		*rp = nullptr ;
        if (sl < 0) sl = strlen(sp) ;
        if (s2len <= sl) {
            cint        s2lead = CHAR_TOLC(ss[0]) ;
            int         i ;
            bool        f = false ;
            for (i = 0 ; i <= (sl-s2len) ; i += 1) {
                if (CHAR_TOLC(sp[i]) == s2lead) {
                    cint        m = nleadcasestr((sp+i),ss,s2len) ;
                    f = (m == s2len) ;
                    if (f) break ;
                } /* end if */
            } /* end for */
            rp = charp((f) ? (sp+i) : nullptr) ;
        } /* end if */
	return rp ;
}
/* end subroutine (strncasesub_local) */


