/* vecpstr_avmkstr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make the Array-Vector and the String-table */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	vecpstr_avmkstr

	Description:
	These routines are used when the caller wants to store a
	COPY of the passed string data into a vector.  These routines
	will copy and store the copied data in the list.  The
	advantage is that the caller does not have to keep the
	orginal data around in order for the list data to be accessed
	later.  String data (unlike "element" data) can not contain
	nullptr characters-bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<intceil.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"vecpstr.h"


/* local defines */


/* external subroutines */


/* forward references */

static int vecpstr_cksize(vecpstr *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int vecpstr_avmkstr(vecpstr *op,cchar **av,int avs,char *tab,int tabs) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && av && tab) {
	    if ((rs = vecpstr_cksize(op)) >= 0) {
		int	sz = iceil(op->stsize,sizeof(int)) ;
		rs = SR_OVERFLOW ;
	        if (tabs >= sz) {
	            sz = (op->c + 1) * sizeof(int) ;
	            if (avs >= sz) {
		        char	*bp = tab ;
		        rs = SR_OK ;
	                *bp++ = '\0' ;
	                for (int i = 0 ; i < op->i ; i += 1) {
		            cchar	*ep = op->va[i] ;
	                    if (ep) {
	                        av[c++] = bp ;
	                        bp = strwcpy(bp,ep,-1) + 1 ;
		            }
	                } /* end for */
	                av[c] = nullptr ;
	            }
	        }
	    } /* end if (vecpstr_cksize) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_avmkstr) */


/* local subroutines */

static int vecpstr_cksize(vecpstr *op) noex {
	int		rs = SR_OK ;
	if (op->stsize == 0) {
	    rs = vecpstr_strsize(op) ;
	}
	return rs ;
}
/* end subroutine (vecpstr_cksize) */


