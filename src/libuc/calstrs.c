/* calstrs */
/* lang=C20 */

/* calendar strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This subroutine was originally written.

	= 2018-03-13, David A�D� Morano
	I prefixed the exported symbols w/ 'calstrs_'.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	We provide a database of various common calendar strings.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cchar	*calstrs_months[] = {
	"January", "February", "March", "April", "May", "June", "July", 
	"August", "September", "October", "November", "December", NULL
} ;

cchar	*calstrs_days[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", 
	"Friday", "Saturday", NULL
} ;


/* exported subroutines */


