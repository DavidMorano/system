/* hname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* header-names */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The program was written from scratch to do what the previous
	program by the same name did.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	hname

	Description:
	This code piece supplies the mail message header names that
	are used for some (important?) purposes in the rest of the
	code. :-)

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"hname.h"


const char	*hname[] = {
	"errorsto",
	"replyto",
	"sender",
	"from",
	"to",
	"cc",
	"bcc",
	"org",
	"msgid",
	"subject",
	nullptr
} ; /* end array (hname) */


