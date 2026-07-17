/* digsmall SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* format a num for a three-column-wide field for the VMAIL program */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-03-01, Walter Pitio
	This code module was originally written (to bring some
	sanity to the way large nums of lines are displayed
	within the VMAIL program).

	= 1992-03-10, David A­D­ Morano
	This code module was modified for integration for use within
	PCS-VMAIL (as was originally intended).

*/

/* Copyright © 1992 Walter Pitio and David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	digsmall

	Description:
	This subroutine creates a little string (stored in a
	caller-supplied buffer) that indicates the specified num
	passed.  The num is supposed to be the num of lines
	in a message, but that does not really matter to us what
	it represents.

	Synopsis:
	int digsmall(char *dbuf,int num) noex

	Arguments:
	dbuf		result buffer pointer (to store resulting string)
	num		num to represent in buffer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<snx.h>			/* LIBUC |snwprintf(3uc)| */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"digsmall.h"


/* local defines */

#define	DIGSTRLEN	3		/* field-width (columns) in VMAIL */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		dlen = DIGSTRLEN ;


/* exported variables */


/* exported subroutines */

int digsmall(char *dbuf,int num) noex {
	int		rs = SR_FAULT ;
	if (dbuf) {
	    int		temp = num ;
	    int		digits = 0 ;
	    /* Handle negative and zero */
	    if (num < 0) {
	        rs = sncpy1(dbuf,dlen,"---") ;
	    } else if (num == 0) {
	        rs = sncpy1(dbuf,dlen,"  0") ;
	    } else {
	        /* Count the num of digits in the num */
	        while (temp > 0) {
	            temp /= 10 ;
	            digits += 1 ;
	        } /* end while */
	        /* process output based on num of digits in num */
	        switch (digits) {
	        /*	0 - 999:			"nnn" */
	        case 1:
	        case 2:
	        case 3:
	            rs = snwprintf(dbuf,dlen,"%3u",num) ;
	            break ;
	        /*	1,000 - 9,999:			" nK" */
	        /*	10,000 - 99,999:		"nnK" */
	        case 4:
	        case 5:
	            rs = snwprintf(dbuf,dlen,"%2dk",(num/1000)) ;
	            break ;
	        /* 	100,000 - 999,999:		".nM" */
	        case 6:
	            rs = snwprintf(dbuf,dlen,".%1dm",(num/100000)) ;
	            break ;
    
	        /*	1,000,000 - 9,999,999:		" nM" */
	        /*	10,000,000 - 99,999,999:	"nnM" */
	        case 7:
	        case 8:
	            rs = snwprintf(dbuf,dlen,"%2dm",(num/1000000)) ;
	            break ;
	        /*	100,000,000 - 999,999,999:	".nG" */
	        case 9:
	            rs = snwprintf(dbuf,dlen,".%1dg",(num/100000000)) ;
	            break ;
	        /*	1,000,000,000 - max long int:	" nG" */
	        case 10:
	            rs = snwprintf(dbuf,dlen,"%2dg",(num/1000000000)) ;
	            break ;
	        default:
	            rs = sncpy1(dbuf,dlen,"***") ;
	            break ;
	        } /* end switch */
	    } /* end if */
	    if (rs < 0) {
	        dbuf[0] = '\0' ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (digsmall) */


