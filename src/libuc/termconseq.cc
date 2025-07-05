/* termconseq SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Terminal Control Sequence */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This object package is finally finished!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	termconseqa

	Description:
	This subroutine formulates a simple string that constitutes
	a terminal command sequence (of characters), but we accommodate
	an intermediate character-string.

	Synopsis:
	int termconseqa(char *dp,int dl,int name,cchar *is,int na,...) noex

	Arguments:
	dp		result buffer
	dl		result buffer length
	name		control sequence name
	is		intermediate character-string
	na		number-arguments (including zero)
	a[1-4]		control sequence arguments

	Returns:
	>=0		length of resulting string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<climits>
#include	<cstdarg>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"termconseq.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int termconseqa(char *dp,int dl,int name,cchar *is,int na,...) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    va_list	ap ;
	    va_begin(ap,na) ;
	    rs = termconseqva(dp,dl,name,is,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termconseqa) */

int termconseqva(char *dp,int dl,int name,cchar *is,int na,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dp) {
	    rs = SR_INVALID ;
	    if (dl < 0) dl = INT_MAX ;
	    if (name > 0) {
		rs = SR_OK ;
	        if (rs >= 0) {
	            cchar	*sp = "\033[" ;
	            rs = storebuf_strw(dp,dl,i,sp,2) ;
	            i += rs ;
	        }
	        if ((rs >= 0) && is) {
	            rs = storebuf_strw(dp,dl,i,is,-1) ;
	            i += rs ;
	        }
		if ((rs >= 0) && (na > 0)) {
		    int		c = 0 ;
	            for (int ai = 0 ; ai < na ; ai += 1) {
	                cint	a = (int) va_arg(ap,int) ;
	                if (a >= 0) {
	                    if ((rs >= 0) && (c++ > 0)) {
	                        rs = storebuf_chr(dp,dl,i,';') ;
	                        i += rs ;
	                    }
	                    if (rs >= 0) {
	                        rs = storebuf_deci(dp,dl,i,a) ;
	                        i += rs ;
	                    }
	                } /* end if (non-negative) */
			if (rs < 0) break ;
	            } /* end for */
	        } /* end if */
	        if (rs >= 0) {
	            rs = storebuf_chr(dp,dl,i,name) ;
	            i += rs ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (termconseqva) */


