/* strnndictcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string compare using dictionary order */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnndictcmp

	Description:
	This subroutine is similar to the |strcmp(3c)| subroutine
	except that with this subroutine all comparisions are done
	using "dictionary" order.  Dictionary order only compares
	characters that are:
		letters
		digits
		spaces
	The dictionary order used below is (currently) the same as 
	defined by the outcome of using the SORT program with the
	'-d' option (specifying "dictionnary" order).  This is
	*different* than many -- or even most -- of the current
	(English) dictionaries that are out there.

	Also (in accordance w/ "dictionnary" order), upper and lower
	case are mostly ignored except that upper case still comes
	before lower case.

	Synopsis:
	int strnndictcmp(cchar *s1,int s1len,cchar *s2,int s2len) noex

	Arguments:
	s1	c-string -1- pointer
	s1len	c-string -1- length
	s2	c-string -2- pointer
	s2len	c-string -2- length

	Returns:
	>0	the first string is bigger than the second
	0	both strings are equal (as compared)
	<0	first string is less than the second

	Example:
	Below is an example of tthe dictionary order as used by
	this subroutine (and as determined by the UNIX® SORT program):
		Oak
		Oak Hill
		Oak Ridge
		Oakley Park
		Oakley River
		oak antelers

	Future:
	The future definition or implementation of "dictionary"
	order may (or may likely) change.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| + |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"strnxcmp.h"

import libutil ;			/* lenstr(3u)| */

/* local defines */

#define	DCH	struct dictchar


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct dictchar {
	const uchar	*sp ;
	int		sl ;
} ;


/* forward references */

static int	dch_start(DCH *,cchar *,int) noex ;
static int	dch_finish(DCH *) noex ;
static int	dch_get(DCH *) noex ;

static int	strdocmp(cchar *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int strnndictcmp(cchar *s1,int s1len,cchar *s2,int s2len) noex {
	int		rs ;
	int		rc = 0 ;
	if (s1len < 0) s1len = lenstr(s1) ;
	if (s2len < 0) s2len = lenstr(s2) ;
	if (DCH dc1 ; (rs = dch_start(&dc1,s1,s1len)) >= 0) {
	    if (DCH dc2 ; (rs = dch_start(&dc2,s2,s2len)) >= 0) {
		int	fch1 ;
		int	fch2 ;
		int	i{} ; /* used-afterwards */
	        for (i = 0 ; rc == 0 ; i += 1) {
	            cint	ch1 = dch_get(&dc1) ;
	            cint	ch2 = dch_get(&dc2) ;
	            fch1 = CHAR_TOFC(ch1) ;
	            fch2 = CHAR_TOFC(ch2) ;
	            rc = (fch1 - fch2) ;
	            if ((ch1 == 0) && (ch2 == 0)) /* end-of-string for both */
	                break ;
	        } /* end while */
	        if (rc == 0) {
	            rc = (s1len - s2len) ; /* more determinism */
		}
	        if (rc == 0) {
	            rc = strdocmp(s1,s2,i) ; /* break ties so far */
		}
	        if (rc == 0) {
	            while (s1len && (! isdict(s1[s1len-1]))) {
			s1len -= 1 ;
		    }
	            while (s2len && (! isdict(s2[s2len-1]))) {
			s2len -= 1 ;
		    }
	            rc = (s1len - s2len) ; /* more determinism */
	        }
	        if (rc == 0) {
	            rc = strncmp(s1,s2,s2len) ; /* absolute determinism (?) */
		}
	        dch_finish(&dc2) ;
	    } /* end if (dc2) */
	    dch_finish(&dc1) ;
	} /* end if (dc1) */
	return rc ;
}
/* end subroutine (strnndictcmp) */


/* local subroutines */

static int dch_start(DCH *dcp,cchar *sp,int sl) noex {
	dcp->sp = (const uchar *) sp ;
	dcp->sl = sl ;
	return 0 ;
}
/* end subroutine (dch_start) */

static int dch_finish(DCH *dcp) noex {
	int		sl = dcp->sl ;
	return sl ;
}
/* end subroutine (dch_finish) */

static int dch_get(DCH *dcp) noex {
	int		doch = 0 ;
	forever {
	    doch = 0 ;
	    if (dcp->sl == 0) break ;
	    doch = mkchar(dcp->sp[0]) ;
	    if (doch == 0) break ;
	    dcp->sp += 1 ;
	    dcp->sl -= 1 ;
	    if (isdict(doch)) break ;
	} /* end forever */
	return doch ;
}
/* end subroutine (dch_get) */

static int strdocmp(cchar *s1,cchar *s2,int slen) noex {
	int		do1 ;
	int		do2 ;
	int		rc = 0 ;
	for (int i = 0 ; (i < slen) && *s1 && *s2 ; i += 1) {
	    do1 = 0 ;
	    while (*s1 && ((do1 = CHAR_DICTORDER(*s1)) == 0)) {
		s1 += 1 ;
	    }
	    do2 = 0 ;
	    while ((i < slen) && *s2 && ((do2 = CHAR_DICTORDER(*s2)) == 0)) {
	        s2 += 1 ;
	        i += 1 ;
	    }
	    rc = (do1 - do2) ;
	    if (rc != 0) break ;
	    if (*s1) s1 += 1 ;
	    if (*s2) s2 += 1 ;
	} /* end for */
	return rc ;
}
/* end subroutine (strdocmp) */


