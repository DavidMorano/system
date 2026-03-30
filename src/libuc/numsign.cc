/* numsign SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* number sign access */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
  	Group:
	numsign

	Description:
	Retrieve the numeric sign of an integer variable (of various
	sizes).

	Synopsis:
	bool numsign(int v) noex
	bool numsign(long v) noex
	bool numsign(longlong int v) noex

	Arguments:
	v	variable to examine

	Returns:
	false 	sign bit was '0'
	true	sign but was '1'

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<clanguage.h>

#include	"numsign.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

bool numsigni(int v) noex {
	return numsign(v) ;
}

bool numsignl(long v) noex {
	return numsign(v) ;
}

bool numsignul(longlong v) noex {
	return numsign(v) ;
}

bool numsignui(uint v) noex {
	return numsign(v) ;
}

bool numsignul(ulong v) noex {
	return numsign(v) ;
}

bool numsignull(ulonglong v) noex {
	return numsign(v) ;
}


