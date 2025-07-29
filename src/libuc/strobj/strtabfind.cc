/* strtabfind SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a string in tables created by a STRTAB object */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strtabfind

	Description:
	This subroutine finds a specified string in tables that
	were (assumed) created by a STRTAB object.

	Synopsis:
	typedef int (*it_t)[3] ;
	int strtabfind(cc *tab,it_t it,int itlen,int nskip,cc *sp,int sl) noex

	Arguments:
	tab		the string table
	it		the index table
	itlen		the length (number of entries) of index table
	nskip		the "skip" factor
	sp		lookup c-string pointer
	sl		lookup c-string length

	Returns:
	>=0		collisions
	<0		error code (SR_NOTFOUND)


	Note: 
	This subroutine uses -> hash-linking <- to track down key
	matches in the index table.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<nleadstr.h>
#include	<hash.h>		/* |hash_elf(3uc)| + ... */
#include	<hashindex.h>
#include	<localmisc.h>

#include	"strtabfind.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef int (*it_t)[3] ;


/* external subroutines */


/* external variables */


/* exported variables */


/* local structures */


/* forward references */

static bool	ismatkey(cchar *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int strtabfind(cc *tab,it_t it,int itlen,int nskip,cc *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	int		sc = 0 ; /* return-value Skip-Count */
	if (tab && it && sp) ylikely {
	    uint	khash = hash_elf(sp,sl) ;
	    uint	nhash ;
	    uint	chash ;
	    cint	nmax = (itlen + nskip) ;
	    int		nhi ;
	    int		hi ;
	    int		j ; /* used-afterwards */
	    int		si = -1 ;
	    bool	f_mathash = false ;
	    bool	f_mat = false ;
	    rs =  SR_OK ;
	    chash = (khash & INT_MAX) ;
	    nhash = khash ;
	    hi = hashindex(nhash,itlen) ;
            for (j = 0 ; (j < nmax) && ((si = it[hi][0]) > 0) ; j += 1) {
	        f_mathash = ((it[hi][1] & INT_MAX) == chash) ;
	        if (f_mathash) break ;
	        if ((it[hi][1] & (~ INT_MAX)) == 0) {
		    break ;
	        }
	        nhash = hash_again(nhash,j,nskip) ;
	        hi = hashindex(nhash,itlen) ;
	    } /* end for */
	    sc += j ;
	    if ((si > 0) && f_mathash) {
	        while ((si = it[hi][0]) > 0) {
	            cchar	*cp = (tab + si) ;
	            f_mat = (cp[0] == sp[0]) && ismatkey(cp,sp,sl) ;
	            if (f_mat) break ;
		    nhi = it[hi][2] ;
		    if (nhi == 0) break ;
		    hi = nhi ;
		    sc += 1 ;
	        } /* end while */
	    } /* end if */
	    if (! f_mat) rs = SR_NOTFOUND ;
	} /* end if (non-null) */
	return (rs >= 0) ? sc : rs ;
}
/* end subroutine (strtabfind) */


/* local subroutines */

static bool ismatkey(cchar *key,cchar *kp,int kl) noex {
	bool		f = (key[0] == kp[0]) ;
	if (f) {
	    cint	m = nleadstr(key,kp,kl) ;
	    f = (m == kl) && (key[m] == '\0') ;
	}
	return f ;
}
/* end subroutine (ismatkey) */


