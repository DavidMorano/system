/* hasmisc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the given counted c-string have some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	has{x}

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.


	Name:
	hsempty

	Description:
        This subroutine determines if a given c-string is empty or not.

	Synopsis:
	bool hasempty(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	1		true (empty)
	0		false (not empty)


	Name:
	haseoh

	Description:
	Determine if the given c-string consists of an End-Of-Header
	(EOH) sequence.  An EOH is a leadering blank line of two
	sorts:
	<NL>
	<CR><NL>

	Synopsis:
	bool haseoh(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		assertion fails
	true		assertion succeeds

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| + |strncmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<matstr.h>		/* LIBUC |matstr(3uc)| used below */
#include	<char.h>		/* LIBUC */
#include	<stdfnames.h>		/* LIBU |stdfname| used below */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"hasmisc.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */

/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	twochars(char *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

bool hasempty(cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    for ( ; sl && *sp ; sp += 1) {
	        f = ISWHT(*sp) ;
	        if (! f) break ;
		sl -= 1 ;
	    } /* end for */
	    if ((! f) && sl && *sp) {
	        f = (*sp == '\n') ;
	    } /* end if */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasempty) */

bool hasdots(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    if ((sl != 0) && (sp[0] == '.')) {
	        if (sl < 0) sl = lenstr(sp) ;
	        f = f || (sl == 1) ;
	        f = f || ((sl == 2) && (sp[1] == '.')) ;
	    }
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasdots) */

bool haseoh(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    if (sl > 0) {
	        f = (sp[0] == '\n') ;
	        f = f || ((sl > 1) && (sp[0] == '\r') && (sp[1] == '\n')) ;
	    } /* end if */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (haseoh) */

bool hasdoublewhite(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    bool	f_prev = false ;
	    for (int i = 0 ; sl-- && sp[i] ; i += 1) {
	        cbool	f_white = ISWHT(sp[i]) ;
	        if (f_white) {
		    f = f_prev ;
	            f = f || (sp[i] != CH_SP) ;	/* to force compaction */
	            f = f || (i == 0) ; 	/* to force compaction */
	            if (f) break ;
	        } /* end if */
	        f_prev = f_white ;
	    } /* end for */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasdoublewhite) */

bool hasmacro(cchar *lp,int ll) noex {
	bool		f = false ;
	if (lp) ylikely {
	    if ((ll > 1) && (lp[0] == '.')) {
	        if (char tbuf[3] ; twochars(tbuf,(lp+1),(ll-1)) > 1) {
	            f = (strcmp(tbuf,"\\\"") != 0) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasmacro) */

bool hasvalidmagic(cchar *tbuf,int tlen,cchar *ms) noex {
	bool		f = false ;
	if (tbuf && ms) ylikely {
	    if (cint ml = lenstr(ms) ; tlen >= (ml + 1)) {
	        f = true ;
	        f = f && (strncmp(tbuf,ms,ml) == 0) ;
	        f = f && (tbuf[ml] == '\n') ;
	    } /* end if */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasvalidmagic) */

bool hasfnamespecial(cchar *fp,int fl) noex {
	return (getstdfname(fp,fl) >= 0) ;
} /* end subroutine (hasfnamespecial) */


/* local subroutines */

local int twochars(char *tbuf,cchar *sp,int sl) noex {
	int		c = 0 ;
	for ( ; sl-- && (c < 2) && *sp ; sp += 1) {
	    if (! ISWHT(*sp)) {
		tbuf[c++] = *sp ;
	    } /* end if */
	} /* end for */
	tbuf[c] = '\0' ;
	return c ;
} /* end subroutine (twochars) */


