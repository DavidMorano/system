/* ha SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* header-addresses - part of the MKMSG program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
        The program was written from scratch to do what the previous program by
        the same name did.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ha

	Description:
	This code piece supplies the mail message header key-names
	that are used to specified addresses (header-Addresses).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ha.h"


const char	*ha[] = {
	"errorsto",
	"replyto",
	"sender",
	"from",
	"to",
	"cc",
	"bcc",
	nullptr
} ; /* end array (ha) */


