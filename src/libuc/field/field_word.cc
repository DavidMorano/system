/* field_word SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* routine to parse a line into words (word field) */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-05-01, David A­D­ Morano
	This was created (from |field_get(3uc)|) to deal with parsing
	keys that are supposed to be English words in prose.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	field_word

	Description:
	This subroutine finds the next "word" in the buffer.  This
	is different than |field_get()| since no default quoting
	is assumed.

	Synopsis:
	int field_word(field *op,cchar *terms,cchar **fpp) noex

	Arguments:
	op	address of return status block
	terms	address of terminator block
	fpp	pointer to result pointer

	Returns:
	>=0	length of field just parsed out
	<0	invalid field block pointer was passwd (system-return)

	The return status block outputs are:
	- length remaining in string
	- address of reminaing string
	- len of substring
	- address of substring
	- terminator character

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
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


/* local structures */


/* forward references */


/* local variables */

constexpr char		wterms[] = {
	0x00, 0x1A, 0x00, 0x00,
	0x3F, 0x40, 0x00, 0x7C,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int field_word(field *op,cchar *terms,cchar **fpp) noex {
	int		rs = SR_FAULT ;
	int		fl = -1 ;
	if (op) {
	    int		ll = op->ll ;
	    int		chterm = '\0' ;
	    cchar	*lp = op->lp ;
	    cchar	*fp = nullptr ;
	    rs = SR_OK ;
	    if (terms == nullptr) terms = wterms ;
	    while ((ll > 0) && CHAR_ISWHITE(*lp)) {
	        lp += 1 ;
	        ll -= 1 ;
	    } /* end while */
	    if (ll > 0) {
		int	ch = mkchar(*lp) ;
	        fl = 0 ;
	        if (! BATST(terms,ch)) {
	            fp = lp ;	 		/* save field address */
	            while (ll > 0) {
		        ch = mkchar(*lp) ;
	                if (BATST(terms,ch)) break ;
	                if (CHAR_ISWHITE(ch)) break ;
	                lp += 1 ;
	                ll -= 1 ;
	            } /* end while */
	            fl = intconv(lp - fp) ;
	            if ((ll > 0) && CHAR_ISWHITE(*lp)) {
	                chterm = ' ' ;
	                while ((ll > 0) && CHAR_ISWHITE(*lp)) {
	                    lp += 1 ;
	                    ll -= 1 ;
	                } /* end while */
	            } /* end if */
	        } /* end if (processing a field) */
	        ch = mkchar(*lp) ;
	        if ((ll > 0) && BATST(terms,ch)) {
	            chterm = ch ;
	            lp += 1 ;
	            ll -= 1 ;
	        } /* end if */
	    } /* end if (positive) */
	    op->lp = lp ;
	    op->ll = ll ;
	    op->fp = (fl >= 0) ? fp : nullptr ;
	    op->fl = fl ;
	    op->term = chterm ;
	    if (fpp) {
	        *fpp = op->fp ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (field_word) */


