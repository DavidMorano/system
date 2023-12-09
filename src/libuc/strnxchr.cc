/* strnxchr */
/* lang=C++20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */

#define	CF_STRCHR	1		/* use |strchr(3c)| */
#define	CF_STRRCHR	1		/* use |strrchr(3c)| */

/* revision history:

	= 1999-06-08, David A�D� Morano
	This subroutine was written again for Rightcore Network
	Services (RNS).

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnchr

	Description:
	Yes, this is quite the same as |strchr(3c)| except that a
	length of the test string (the first argument) can be given.

	Synopsis:
	char *strnchr(cchar *sp,int sl,int ch) noex

	Arguments:
	sp		string to search through
	sl		maximum number of character to search
	ch		the character to search for

	Returns:
	nullptr		did not find the character
	!= nullptr	address of found character in searched string


	Name:
	strnrchr

	Description:
	Yes, this is quite the same as |strrchr(3c)| except that a
	length of the test string (the first argument) can be given.

	Synopsis:
	char *strnrchr(cchar *sp,int sl,int ch) noex

	Arguments:
	sp		pointer to string
	sl		length of string
	ch		character to search for

	Returns:
	-		address of found character or nullptr if not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<cstring>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strnxchr.h"


/* local defines */

#ifndef	CF_STRCHR
#define	CF_STRCHR	0
#endif

#ifndef	CF_STRRCHR
#define	CF_STRRCHR	0
#endif


/* local variables */

constexpr bool		f_strchr = CF_STRCHR ;
constexpr bool		f_strrchr = CF_STRRCHR ;


/* exported subroutines */

char *strnochr(cchar *sp,int sl,int sch) noex {
	char		*rsp = nullptr ;
	if (sp) {
	    bool	f = false ;
	    sch &= UCHAR_MAX ;
	    if (sl < 0) {
	        if constexpr (f_strchr) {
	            rsp = strchr(sp,sch) ;
	        } else {
	            while (*sp) {
	                f = (mkchar(*sp) == sch) ;
		        if (f) break ;
	                sp += 1 ;
	            } /* end while */
		    if (f) rsp = ((char *) sp) ;
	        } /* end if-constexpr (f_strchr) */
	    } else {
	        cchar	*lsp = (sp + sl) ;
	        while ((sp < lsp) && *sp) {
	            f = (mkchar(*sp) == sch) ;
		    if (f) break ;
	            sp += 1 ;
	        } /* end while */
		if (f) rsp = ((char *) sp) ;
	    } /* end if */
	} /* end if (non-null) */
	return rsp ;
}
/* end subroutine (strnochr) */

char *strnrchr(cchar *sp,int sl,int sch) noex {
	char		*rsp = nullptr ;
	if (sp) {
	    bool	fdone = false ;
	    sch &= UCHAR_MAX ;
	    if (sl < 0) {
		if constexpr (f_strrchr) {
		    fdone = true ;
	            rsp = strrchr(sp,sch) ;
		} else {
	            sl = strlen(sp) ;
		} /* end if-constexpr (f_strrchr) */
	    } /* end if (no length given) */
	    if ((!fdone) && (sl >= 0)) {
	        bool	f = false ;
	        cchar	*csp = (sp+sl) ;
	        while (--csp >= sp) {
	            cint	ch = mkchar(*csp) ;
	            f = (ch == sch) ;
	            if (f) break ;
	        } /* end while */
		if (f) rsp = ((char *) csp) ;
	    } /* end if */
	} /* end if (non-null) */
	return rsp ;
}
/* end subroutine (strnrchr) */


