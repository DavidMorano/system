/* field_wordphrase SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to parse a line into word-fields */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code module was originally written in C language modeled
	(roughly) from a prior VAX assembly language version (written
	circa 1980 perhaps).  This is why this looks so "ugly"!
	This code comes from stuff dated back to almost the pre-dawn
	era of modern computer languages!  I wrote the original VAX
	assembly stuff also.  This code below was then written into
	C-language perhaps around 1983 (at AT&T Bell Laboratories).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	field_wordphrase

	Description:

	Synopsis:
	int field_wordphrase(field *op,cchar *terms,char *fbuf,int flen) noex

	Arguments:
	op		field status block pointer
	terms		bit array of terminating characters
	fbuf		buffer to store result
	flen		length of buffer to hold result

	Returns:
	>=0	length of field just parsed out (length of FIELD!)
	<0	invalid field block pointer was passed (system-return)

	The return status block outputs are:
	- length remaining in string
	- address of reminaing string
	- len of substring
	- address of substring
	- terminator character

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<baops.h>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>

#include	"field.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */

/* quote character '\"' */
constexpr char		dquote[] = {
	0x00, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

/* 'double quote', 'back slash', 'pound', 'back accent', et cetera */
constexpr char		doubles[] = {
	0x00, 0x00, 0x00, 0x00,
	0x14, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x10,
	0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

constexpr char		shterms[] = {
	0x00, 0x00, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int field_wordphrase(field *op,cchar *terms,char *fbuf,int flen) noex {
	int		rs = SR_FAULT ;
	int		fl = -1 ;
	if (op && fbuf) {
	    if (op->lp) {
	        int	chterm = '\0' ;
	        int	ll = op->ll ;
	        cchar	*lp = op->lp ;
	        if (terms == nullptr) terms = shterms ;
	        while ((ll > 0) && CHAR_ISWHITE(*lp)) {
	            lp += 1 ;
	            ll -= 1 ;
	        }
	        fl = -1 ;		/* end-of-arguments indicator */
	        if (ll > 0)  {
	            int		ch = 0 ;
	            char	*bp = fbuf ;
	            while (ll > 0) {
			int	nch = 0 ;
			int	qe ;
	                ch = mkchar(*lp) ;
	                if (BATST(terms,ch) & (! BATST(dquote,ch))) break ;
	                if (CHAR_ISWHITE(ch)) break ;
	                if (ch == '\"') {
	                    qe = ch ;
		            lp += 1 ;
	                    ll -= 1 ;
	                    while (ll > 0) {
				bool	f = true ;
		                ch = mkchar(*lp) ;
		                if (ll > 1) {
			            nch = mkchar(lp[1]) ;
				}
	                        f = f && (ch == '\\') ;
				f = f && (ll > 1) ;
				f = f && BATST(doubles,nch) ;
				if (f) {
	                            lp += 1 ;
	                            ll -= 1 ;
		                    ch = mkchar(*lp) ;
	                            if (flen > 0) {
	                                *bp++ = charconv(ch) ;
	                                flen -= 1 ;
	                            }
	                            lp += 1 ;
	                            ll -= 1 ;
	                        } else if (ch == qe) {
	                            lp += 1 ;
	                            ll -= 1 ;
	                            break ;
	                        } else {
	                            if (flen > 0) {
	                                *bp++ = charconv(ch) ;
	                                flen -= 1 ;
	                            }
	                            lp += 1 ;
	                            ll -= 1 ;
	                        } /* end if */
	                    } /* end while (processing quoted portion) */
	                } else if (ch == '\'') {
		            qe = ch ;
		            lp += 1 ;
	                    ll -= 1 ;
	                    while (ll > 0) {
		                ch = mkchar(*lp) ;
	                        if (ch == qe) {
	                            lp += 1 ;
	                            ll -= 1 ;
	                            break ;
	                        } else {
	                            if (flen > 0) {
	                                *bp++ = charconv(ch) ;
	                                flen -= 1 ;
	                            }
	                            lp += 1 ;
	                            ll -= 1 ;
	                        } /* end if */
	                    } /* end while (processing the quoted portion) */
	                } else if ((ch == '\\') && (ll > 1)) {
	                    lp += 1 ;
	                    ll -= 1 ;
		            ch = mkchar(*lp) ;
	                    if (flen > 0) {
	                        *bp++ = charconv(ch) ;
	                        flen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                } else {
	                    if (flen > 0) {
	                        *bp++ = charconv(ch) ;
	                        flen -= 1 ;
	                    }
	                    lp += 1 ;
	                    ll -= 1 ;
	                } /* end if */
	            } /* end while (main loop) */
    		    /* do the terminator processing */
	            while ((ll > 0) && CHAR_ISWHITE(*lp)) {
	                lp += 1 ;
	                ll -= 1 ;
	            } /* end while */
            	    /* we are at the end */
	            chterm = ' ' ;
	            ch = mkchar(*lp) ;
	            if (BATST(terms,ch) && (! BATST(dquote,ch))) {
	                chterm = ch ;		/* save terminator */
	                lp += 1 ;
	                ll -= 1 ;		/* skip over the terminator */
	            } /* end if */
	            fl = intconv(bp - fbuf) ;
	        } /* end if (positive) */
	        op->ll = ll ;
	        op->lp = lp ;
	        op->fl = fl ;
	        op->fp = fbuf ;
	        op->term = chterm ;
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (field_wordphrase) */


