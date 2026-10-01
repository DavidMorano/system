/* usys_strxbrk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_strxbrk.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strobrk(cchar *s,cchar *ss) noex {
    	return strpbrk(s,ss) ;
} /* end subroutine (strobrk) */

char *strrbrk(cchar *s,cchar *ss) noex {
	cint		n = lenstr(s) ;
	bool		f = false ;
	char		*rsp ;
	rsp = charp(s + n) ;
	while (--rsp >= s) {
	    cint ch = int(*rsp & UCHAR_MAX) ;
	    f = (strchr(ss,ch) != nullptr) ;
	    if (f) break ;
	} /* end while */
	return (f) ? rsp : nullptr ;
} /* end subroutine (strrbrk) */


