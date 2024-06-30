/* digit3 SUPPORT */
/* lang=C++20 */

/* format a number for a three-column-wide field fpr the VMAIL program */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-03-01, Walter Pitio
	This code module was originally written (to bring some
	sanity to the way large numbers of lines are displayed).

	= 1992-03-10, David A�D� Morano
	This code module was modified for integration for use within
	PCS-VMAIL (as was originally intended).

*/

/* Copyright � 1992 Walter Pitio and David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	digit3

	Description:
	This subroutine creates a little string (stored in a
	caller-supplied buffer) that indicates the specified number
	passed. The number is supposed to be the number of lines
	in a message, but that does not really matter to us what
	it represents.

	Synopsis:
	int digit3(char *str_ptr,int number) noex

	Arguments:
	str_ptr		result buffer pointer (to store resulting string)
	number		number to represent in buffer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"digit3.h"


/* local defines */

#define	DIGSTRLEN	3


/* external subroutines */

extern "C" {
    extern int	bufprintf(char *,int,cchar *,...) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int digit3(char *str_ptr,int number) noex {
	int	rs = SR_OK ;
	int	temp = number ;
	int	digits ;

/* Handle negative and zero */

	if (number < 0) {

	    rs = sncpy1(str_ptr,DIGSTRLEN,"---") ;

	} else if (number == 0) {

	    rs = sncpy1(str_ptr,DIGSTRLEN,"  0") ;

	} else {

/* Count the number of digits in the number */

	    digits = 0;		/* Initialize number of digits */
	    while (temp > 0) {
	        temp /= 10 ;
	        ++digits ;
	    }

/* No process output based on number of digits in number */

	    switch (digits) {

/*	0 - 999:			"nnn" */
	    case 1:
	    case 2:
	    case 3:
	        rs = bufprintf(str_ptr,DIGSTRLEN,"%3u",number) ;
	        break ;

/*	1,000 - 9,999:			" nK" */
/*	10,000 - 99,999:		"nnK" */
	    case 4:
	    case 5:
	        rs = bufprintf(str_ptr,DIGSTRLEN,"%2uk",(number/1000)) ;
	        break ;

/* 	100,000 - 999,999:		".nM" */
	    case 6:
	        rs = bufprintf(str_ptr,DIGSTRLEN,".%1um",(number/100000)) ;
	        break ;

/*	1,000,000 - 9,999,999:		" nM" */
/*	10,000,000 - 99,999,999:	"nnM" */
	    case 7:
	    case 8:
	        rs = bufprintf(str_ptr,DIGSTRLEN,"%2um",(number/1000000)) ;
	        break ;

/*	100,000,000 - 999,999,999:	".nG" */
	    case 9:
	        rs = bufprintf(str_ptr,DIGSTRLEN,".%1dg",(number/100000000)) ;
	        break ;

/*	1,000,000,000 - max long int:	" nG" */
	    case 10:
	        rs = bufprintf(str_ptr,DIGSTRLEN,"%2ug",(number/1000000000)) ;
	        break ;

	    default:
	        rs = sncpy1(str_ptr,DIGSTRLEN,"***") ;
	        break ;

	    } /* end switch */

	} /* end if */

	if (rs < 0)
	    str_ptr[0] = '\0' ;

	return rs ;
}
/* end subroutine (digit3) */


