/* contentencodings SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* content-encodings for mail message headers */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The program was written from scratch to do what the previous
	program by the same name did.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	contentencodings

	Description:
	This code piece supplies the constants needed for content
	encoding of a message body.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"contentencodings.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

cpcchar		contentencodings[] = {
	"none",
	"7bit",
	"8bit",
	"binary",
	"base64",
	"quoted-printable",
	nullptr
} ;


