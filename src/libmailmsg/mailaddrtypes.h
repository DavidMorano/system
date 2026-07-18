/* mailaddrtypes HEADER (Mail-Address-Types) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Mail_Address_Types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-12, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILADDRTYPES_INCLUDE
#define	MAILADDRTYPES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum mailaddrtypes {
    mailaddrtype_local,
    mailaddrtype_uucp,
    mailaddrtype_arpa,
    mailaddrtype_arparoute,
    mailaddrtype_overlast
} ; /* end enum (mailaddrtypes) */


#endif /* MAILADDRTYPES_INCLUDE */


