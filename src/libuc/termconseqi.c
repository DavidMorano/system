/* termconseqi SPPORT */
/* lang=C20 */

/* Terminal Control Sequence w/ intermediate character-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This object package is finally finished!

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	termconseqi

	Description:
	This subroutine formulates a simple string that constitutes
	a terminal command sequence (of characters), but we accommodate
	an intermediate character-string.

	Synopsis:
	int termconseqi(char *dp,int dl,int name,cc *is,i a1,i a2,i a3, a4) noex

	Arguments:
	dp		result buffer
	dl		result buffer length
	name		control sequence name
	is		intermediate character-string
	a[1-4]		control sequence arguments

	Returns:
	>=0		length of resulting string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<storebuf.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int termconseqi(char *dp,int dl,int name,cchar *is,
		int a1,int a2,int a3,int a4) noex {
	int		rs = SR_OK ;
	int		ai ;
	int		a ;
	int		c = 0 ;
	int		i = 0 ;

	if (dp == NULL) return SR_FAULT ;

	if (dl < 0) dl = INT_MAX ;

	if (name != 0) {

	    if (rs >= 0) {
	        cchar 	*sp = "\033[" ;
	        rs = storebuf_strw(dp,dl,i,sp,2) ;
	        i += rs ;
	    }

	    if ((rs >= 0) && (is != NULL)) {
	        rs = storebuf_strw(dp,dl,i,is,-1) ;
	        i += rs ;
	    }

	    for (ai = 1 ; (rs >= 0) && (ai <= 4) ; ai += 1) {
	        a = -1 ;
	        switch (ai) {
	        case 1:
	            a = a1 ;
	            break ;
	        case 2:
	            a = a2 ;
	            break ;
	        case 3:
	            a = a3 ;
	            break ;
	        case 4:
	            a = a4 ;
	            break ;
	        } /* end switch */
	        if (a >= 0) {
	            if ((rs >= 0) && (c > 0)) {
	                rs = storebuf_char(dp,dl,i,';') ;
	                i += rs ;
	            }
	            if (rs >= 0) {
	                rs = storebuf_deci(dp,dl,i,a) ;
	                i += rs ;
	            }
	            c += 1 ;
	        }
	    } /* end for */

	    if (rs >= 0) {
	        rs = storebuf_char(dp,dl,i,name) ;
	        i += rs ;
	    }

	} else
	    rs = SR_INVALID ;

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (termconseqi) */


