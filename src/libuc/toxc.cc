/* toxc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
        This code was originally written.

	= 2014-01-13, David A�D� Morano
	I removed the option of compilling a so-called computed
	type of conversion.  It is way too complicated when considering
	the full ISO-8859-1 (Latin-1) character set (our primary
	focus).  The translation-conversions using the table-lookup
	method (using |CHAR(3uc)|) are fast and trivially simple.
	The computed type of conversions were not ever chosen.
	Hence their removal.  Why was a computed version availble
	in the first place?  I really do not know.  Someone says
	that it was used for small embedded applications where space
	was at a super citical premium.  Well, just include the
	|CHAR(3uc)| translation tables!  They do not really take
	up a lot of space, and available space is not so critically
	small as it used to be (over 30 years ago now)!

*/

/* Copyright � 1998,2014 David A�D� Morano.  All rights reserved. */

/******************************************************************************* 
  	Group:
	to{x}c

	Description:
	These subroutines perform character case conversions
	(cheaply).  Yes, case-conversion is used a lot in embedded
	systems!

	Implementation note:
	The |CHAR(3uc)| translations (conversions) return an
	unsigned character ('uchar') type.  This means that when
	cajoled into becoming an 'int' type on return, it will not
	be sign-extended.

	= 2014-01-13
	We now use the |CHAR(3uc)| translation tables only (no
	computed conversion compile-time option).  Deal with it.
	See my note above under the revision history.  Look how
	trivial the code is now!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<char.h>		/* for table-lookup conversions */
#include	<localmisc.h>

#include	"toxc.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


