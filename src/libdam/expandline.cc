/* expandline SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to expand a line out for TROFF rendering */
/* version %I% last-modified %G% */

#define	CF_PREFIX	1

/*******************************************************************************

	= 1987-09-05, David A-D- Morano
	This code was originally written for the
	TEXTSET program.

*******************************************************************************/

/* Copyright © 1987 David A-D- Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Name:
	expandline

	Description:
	This subroutine will take an input line and expand it out
	so that it will render correctly under TROFF processing.
	This subroutine also peforms standard tab expansion to
	tabstop at every eight (8) columns.

	Synopsis:
	int expandline(cchar *ibp,int il,char *obp,int ol,int *flagp) noex

	Arguments:
	- ibp		input buffer pointer
	- ilen		input buffer length
	- obp		output buffer pointer
	- olen		output buffer length
	- flags		flags to control the operation

	Returns:
	>=0		length of output line processed
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"expandline.h"


/* local defines */

#ifndef	CF_PREFIX
#define	CF_PREFIX	1
#endif


/* external subroutines */


/* extern variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool 		f_prefix = CF_PREFIX ;


/* exported variables */


/* exported subroutines */

int expandline(cchar *ibp,int il,char *obp,int ol,int *flagp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ibp && obp && flagp) {
	    rs = SR_INVALID ;
	    if (ol < 2) {
	        int	op = 0  ;
	        int	j, k ;
	        int	cur_l, last_l ;
	        char	*cur_obp, *last_obp ;
		rs = SR_OK ;
		*flagp = false ;
	        if (il && ((*ibp == '.') || (*ibp == '\''))) {
		    *flagp = true ;
		    if_constexpr (f_prefix) {
		        *obp++ = '\\' ;
		        *obp++ = '&' ;
		        ol -= 2 ;
		    } /* end if_constexpr (f_prefix) */
	        }
	        for (int i = 0 ; (i < il) && (len < (ol - 8)) ; i += 1) {
	            int		c = *ibp ;
	            last_obp = cur_obp ;
	            cur_obp = obp ;
	            last_l = cur_l ;
	            cur_l = len ;
	            if (c == 0)  {
		        ibp += 1 ;
	            } else if (c == '\\') {
	                *obp++ = *ibp++ ;
	                *obp++ = '\\' ;
	                len += 2 ;
	                op += 1 ;
	            } else if (c == '\t') {
	                k = 8 - (op & 7) ;
	                for (j = 0 ; j < k ; j += 1) {
	                    *obp++ = ' ' ;
	                }
	                len += k ;
	                ibp += 1 ;
	                op += k ;
	            } else if ((c & 0x7F) == '\010') {
	                ibp += 1 ;
	                if (len > 0) {
	                    len = last_l ;
	                    op -= 1 ;
	                    obp = last_obp ;
	                }
	            } else if (c == '\n') {
	                *obp++ = *ibp++ ;
	                len += 1 ;
	                op += 1 ;
	            } else if (((c & 0x7F) < 0x20) || ((c & 0x7F) == 0x7F)) {
	                *obp++ = '\\' ;
	                *obp++ = '(' ;
	                *obp++ = 's' ;
	                *obp++ = 'q' ;
	                ibp += 1 ;
	                len += 4 ;
	                op += 1 ;
	            } else {
	                *obp++ = *ibp++ ;
	                len += 1 ;
	                op += 1 ;
	            }
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (expandline) */


