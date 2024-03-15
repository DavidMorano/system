/* field_word SUPPORT */
/* lang=C++20 */

/* routine to parse a line into words (word field) */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-05-01, David A­D­ Morano
	This was created (from 'field_get()') to deal with parsing
	keys that are supposed to be English words in prose.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	field_word

	Description:
	This subroutine finds the next "word" in the buffer. This
	is different than 'field_get()' since no default quoting
	is assumed.

	Synopsis:
	int field_word(field *fsbp,cchar *terms,cchar **fpp) noex

	Arguments:
	fsbp	address of return status block
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
#include	<usystem.h>
#include	<baops.h>
#include	<char.h>
#include	<field.h>
#include	<mkchar.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */

static constexpr cchar	wterms[] = {
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

int field_word(field *fsbp,cchar *terms,cchar **fpp) noex {
	int		ll, fl ;
	int		ch ;
	int		chterm = '\0' ;
	cchar		*lp, *fp ;

	if (fsbp == NULL) return SR_FAULT ;

/* get the parameters */

	lp = fsbp->lp ;
	ll = fsbp->ll ;

	if (terms == NULL)
	    terms = wterms ;

/* skip all initial white space */

	while ((ll > 0) && CHAR_ISWHITE(*lp)) {
	    lp += 1 ;
	    ll -= 1 ;
	} /* end while */

	fp = NULL ;
	fl = -1 ;
	if (ll > 0) {

	fl = 0 ;
	ch = mkchar(*lp) ;
	if (! BATST(terms,ch)) {

	    fp = lp ;	 		/* save field address */
	    while (ll > 0) {

		ch = mkchar(*lp) ;
	        if (BATST(terms,ch))
	            break ;

	        if (CHAR_ISWHITE(ch))
	            break ;

	        lp += 1 ;
	        ll -= 1 ;

	    } /* end while */

	    fl = (lp - fp) ;

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

/* update the return status block */

	fsbp->lp = lp ;
	fsbp->ll = ll ;
	fsbp->fp = (fl >= 0) ? fp : NULL ;
	fsbp->fl = fl ;
	fsbp->term = chterm ;

	if (fpp) {
	    *fpp = fsbp->fp ;
	}

	return fl ;
}
/* end subroutine (field_word) */


