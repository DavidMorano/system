/* field_srvarg SUPPORT */
/* lang=C++20 */

/* subroutine to field out (parse) the next "server" argument */
/* version %I% last-modified %G% */

#define	CF_TRAILWHITE	1		/* remove trailing whitespace? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code module was originally written in C language modeled
	(roughly) from a prior VAX assembly language version (circa
	1980 perhaps).  This is why is looks so "ugly"!  This stuff
	comes from stuff dated back to almost the pre-dawm era of
	computer languages!  I wrote the original VAX assembly stuff
	also.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	field_srvarg

	Description:
	This subroutine is very similar to |field_sharg| in that
	it will parse a string of arguments similar to SHELL command
	line arguments.  It is different in that a comma character
	will terminate the list of argumuents (along with an EOL).
	This function is needed for gathering up arguments to a key
	string in things like server table files.  Note that
	generally, this function will skip over white space in order
	to get a group of items that are all considered one argument
	in the outer parsing context.

	Synopsis:
	int field_srvarg(field *fsbp,cchar *terms,char *abuf,int alen) noex

	Arguments:
	fsbp		field status block pointer
	terms		bit array of terminating characters
	abuf		buffer to store result
	alen		length of buffer to hold result

	Returns:
	>0		length of returned field, possibly zero
	==0		no additional fields
	<0		error (system-return)

	The return status block outputs are:
	- length remaining in string
	- address of reminaing string
	- len of substring
	- address of substring
	- terminator character

	Note:
	This subroutine will do quote interpretation like the SHELL
	when inside double or single quotes!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<usystem.h>
#include	<ascii.h>
#include	<baops.h>
#include	<char.h>
#include	<mkchar.h>
#include	<field.h>
#include	<localmisc.h>


/* local defines */

#ifndef	CF_TRAILWHITE
#define	CF_TRAILWHITE	0		/* default safety definition */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static void fieldinit_terms() noex ;


/* local structures */

namespace {
    struct fieldinit {
	fieldinit() noex {
	   fieldinit_terms() ;
	} ;
    } ; /* end struct (fieldinit) */
}


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

/* 'double quote', 'back-slash', 'pound', 'back-accent', et cetera */
static char		doubles[termsize] = {} ;

/* default parse terminators for server arguments! */
static char		dterms[termsize] = {} ;

constexpr bool		f_trailwhite = CF_TRAILWHITE ;


/* exported variables */


/* exported subroutines */

int field_srvarg(field *fsbp,cchar *terms,char *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	int		fl = 0 ;
	if (terms == nullptr) terms = dterms ;
	if (fsbp && abuf) {
	    int		ch, nch ;
	    int		qe ;
	    int		chterm = '\0' ;
	    int		ll = fsbp->ll ;
	    cchar	*lp = fsbp->lp ;
	    char	*bp = abuf ;
	    abuf[0] = '\0' ;
	    while ((ll > 0) && char_iswhite(*lp)) {
	        lp += 1 ;
	        ll -= 1 ;
	    }
	    while (ll > 0) {
	        ch = mkchar(*lp) ;
	        if (batst(terms,ch)) break ;
	        if (ch == '\"') {
	            if (alen > 0) {
	                *bp++ = ch ;
	                alen -= 1 ;
	            }
	            qe = ch ;
		    lp += 1 ;
	            ll -= 1 ;
	            while (ll > 0) {
	                ch = mkchar(*lp) ;
		        if (ll > 1) nch = mkchar(lp[1]) ;
	                if ((ch == '\\') && (ll > 1) && batst(doubles,nch)) {
	                    if (alen > 0) {
	                        *bp++ = ch ;
	                        alen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                    ch = mkchar(*lp) ;
	                    if (alen > 0) {
	                        *bp++ = ch ;
	                        alen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                } else if (ch == qe) {
	                    if (alen > 0) {
	                        *bp++ = ch ;
	                        alen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                    break ;
	                } else {
	                    if (alen > 0) {
	                        *bp++ = ch ;
	                        alen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                } /* end if */
	            } /* end while (processing the quoted portion) */
	        } else if (ch == '\'') {
	            if (alen > 0) {
	                *bp++ = ch ;
	                alen -= 1 ;
	            }
	            qe = ch ;
		    lp += 1 ;
	            ll -= 1 ;
	            while (ll > 0) {
	                ch = mkchar(*lp) ;
	                if (ch == qe) {
	                    if (alen > 0) {
	                        *bp++ = ch ;
	                        alen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                    break ;
	                } else {
	                    if (alen > 0) {
	                        *bp++ = ch ;
	                        alen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                } /* end if */
	            } /* end while (processing the quoted portion) */
	        } else if (ch == '<') {
	            qe = '>' ;
	            lp += 1 ;
	            ll -= 1 ;
	            while (ll > 0) {
	                ch = mkchar(*lp) ;
		        if (ll > 1) nch = mkchar(lp[1]) ;
	                if ((ch == '\\') && (ll > 1) && batst(doubles,nch)) {
	                    if (alen > 0) {
	                        *bp++ = ch ;
	                        alen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                    ch = mkchar(*lp) ;
	                    if (alen > 0) {
	                        *bp++ = ch ;
	                        alen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                } else if (ch == qe) {
	                    lp += 1 ;
	                    ll -= 1 ;
	                    break ;
	                } else {
	                    if (alen > 0) {
	                        *bp++ = ch ;
	                        alen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                } /* end if */
	            } /* end while (processing the quoted portion) */
	        } else if ((ch == '\\') && (ll > 1)) {
	            if (alen > 0) {
	                *bp++ = ch ;
	                alen -= 1 ;
	            }
	            lp += 1 ;
	            ll -= 1 ;
	            ch = mkchar(*lp) ;
	            if (alen > 0) {
	                *bp++ = ch ;
	                alen -= 1 ;
	            }
	            lp += 1 ;
	            ll -= 1 ;
	        } else {
	            if (alen > 0) {
	                *bp++ = ch ;
	                alen -= 1 ;
	            }
	            lp += 1 ;
	            ll -= 1 ;
	        } /* end if */
	    } /* end while */
	    if (ll > 0) {
	        chterm = ' ' ;
	        ch = mkchar(*lp) ;
	        if (batst(terms,ch)) {
	            chterm = ch ;
	            lp += 1 ;
	            ll -= 1 ;
	        } /* end if (it was a terminator) */
	    } /* end if (some remaining length) */
	    while ((ll > 0) && char_iswhite(*lp)) {
	        lp += 1 ;
	        ll -= 1 ;
	    } /* end while */
	    fl = (bp - abuf) ;
	    if constexpr (f_trailwhite) {
	        while ((fl > 0) && char_iswhite(abuf[fl - 1])) {
		    fl -= 1 ;
	        }
	    } /* end if-constexpr (f_trailwhite) */
	    fsbp->lp = lp ;
	    fsbp->ll = ll ;
	    fsbp->fp = (fl >= 0) ? abuf : nullptr ;
	    fsbp->fl = fl ;
	    fsbp->term = chterm ;
	} /* end if (non-null) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (field_srvarg) */


/* local subroutines */

static void fieldinit_terms() noex {
	static constexpr bool	f_retain = true ;
	fieldterms(dterms,f_retain,",") ;
	fieldterms(doubles,f_retain,"\"$\\`") ;
}
/* end subroutine (fieldinit_terms) */


