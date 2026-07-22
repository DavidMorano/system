/* sfnext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a sub-string within a larger string (given some criteria) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	sfnext
	sfnextchr
	sfnextbrk
	sfnextterm

	Philosophy:
	An explicit terminator (given) is *optional* in order for
	a non-empty result string to be returned.  A new-line (NL)
	character and white-space are always implied terminators
	(this is historic).

	Description:
	This subroutine will extract the next white-space-separated
	field from the input buffer.  If an NL is encountered, it
	is treated like a delimiter and all characters before it
	(including zero characters) are returned as the delimited
	field.  The caller should test for zero characters being
	returned.  This condition could indicate either that the
	line has no additional characters, or that a NL was
	encountered.  If the caller determines that there are
	characters left in the line but that zero was returned, a
	check by the caller for a NL character is indicated.  If
	the caller determines that a NL was present, the caller
	should arrange to "step over" the NL before making new calls
	in order to get the fields beyond the NL.
	The returned string is white-space shrunken.

	Synopsis:
	int sfnext(cchar *sp,int sl,cchar **spp) noex
	int sfnextchr(cchar *sp,int sl,int sch,cchar **spp) noex
	int sfnextbrk(cchar *sp,int sl,cchar *str,cchar **spp) noex
	int sfnextterm(cchar *sp,int sl,cchar *terms,cchar **spp) noex

	Arguments:
	sp		pointer to start of user supplied buffer
	sl		length of user supplied buffer
	sch		search character
	str		search string
	terms		terminator characters
	spp		pointer to pointer of the found field

	Returns:
	>0		length of found field
	==0		no field found or a NL character was encountered
	<0		does not happen


	Name:
	sfnextbrk

	Description:
	This subroutine will extract the next white-space-separated
	or a combination of white-space and a trailing termination
	character c-string from a counted source c-string.  The
	returned string is white-space shrunken.

	Synopsis:
	int sfnextbrk(cchar *sp,int sl,cchar *terms,cchar **spp) noex

	Arguments:
	sp		pointer to start of user supplied buffer
	sl		length of user supplied buffer
	terms		terminator characters (in a c-string)
	spp		pointer to pointer of the found field

	Returns:
	>0		length of found field
	==0		no field found or a NL character was encountered
	<0		address-fault

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<baops.h>		/* LIBU |batst(2uc)| */
#include	<char.h>		/* LIBUC |CHAR_ISWHITE(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"sfnext.h"


/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
   struct sfnextx {
	cchar		**rpp ;
	cchar		*sp ;
	int		sl ;
	sfnextx(cchar *p,int l,cchar **r) noex : rpp(r), sp(p), sl(l) { } ;
	operator int () noex ;
	virtual bool termx(int ch) const noex {
	    return (ch == 0) ;
	} ;
	bool isterm(int ch) const noex {
	    bool	f = false ;
	    ch &= UCHAR_MAX ;
	    f = f || (ch == '\n') ;
	    f = f || ISWHT(ch) ;
	    f = f || termx(ch) ;
	    return f ;
	} ;
   } ; /* end struct (sfnextx) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfnext(cchar *sp,int sl,cchar **rpp) noex {
	sfnextx sf(sp,sl,rpp) ;
	return sf ;
} /* end subroutine (sfnext) */

int sfnextchr(cchar *sp,int sl,int sch,cchar **rpp) noex {
    	int		rl = -1 ;
	struct esfx : sfnextx {
	    int		sch ; /* <- initialized separately */
	    esfx(cchar *p,int l,cchar **r) noex : sfnextx(p,l,r) { } ;
	    bool termx(int ch) const noex override final {
		return (ch == sch) ;
	    } ;
	} ; /* end struct (esfx) */
	if (sch) ylikely {
	    esfx sf(sp,sl,rpp) ;
	    sf.sch = sch ;		/* <- separate initialization */
	    rl = sf ;
	} /* end if */
	return rl ;
} /* end subroutine (sfnextchr) */

int sfnextbrk(cchar *sp,int sl,cchar *bstr,cchar **rpp) noex {
	int		rl = -1 ;
	struct esfx : sfnextx {
	    cchar	*bstr ;		/* <- initialized separately */
	    esfx(cchar *p,int l,cchar **r) noex : sfnextx(p,l,r) { } ;
	    bool termx(int ch) const noex override final {
		return (strchr(bstr,ch) != nullptr) ;
	    } ;
	} ; /* end struct */
	if (bstr) ylikely {
	    esfx sf(sp,sl,rpp) ;
	    sf.bstr = bstr ;		/* <- separate initializing */
	    rl = sf ;
	} /* end if (non-null) */
	return rl ;
} /* end subroutine (sfnextbrk) */

int sfnextterm(cchar *sp,int sl,cchar *terms,cchar **rpp) noex {
	int		rl = -1 ;
	struct esfx : sfnextx {
	    cchar	*terms ;	/* <- initialized separately */
	    esfx(cchar *p,int l,cchar **r) noex : sfnextx(p,l,r) { } ;
	    bool termx(int ch) const noex override final {
		return batst(terms,ch) ;
	    } ;
	} ; /* end struct */
	if (terms) ylikely {
	    esfx sf(sp,sl,rpp) ;
	    sf.terms = terms ;		/* <- separate initializing */
	    rl = sf ;
	} /* end if (non-null) */
	return rl ;
} /* end subroutine (sfnextterm) */


/* local subroutines */

sfnextx::operator int () noex {
	int		rl = -1 ;
	cchar		*rp = nullptr ;
	if (sp && rpp) ylikely {
	    while (sl && ISWHT(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    rp = sp ;
	    while (sl && *sp && (! isterm(*sp))) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    rl = intconv(sp - rp) ;
	    *rpp = rp ;
	} /* end if (non-null) */
	return rl ;
} /* end subroutine (sfnextx::operator) */


