/* ematypes HEADER (E-Mail-Address-Types) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* E-Mail_Address Types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-12, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	EMATYPES_INCLUDE
#define	EMATYPES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


enum ematypes {
    ematype_local,
    ematype_uucp,
    ematype_arpa,
    ematype_arparoute,
    ematype_overlast
} ; /* end enum (ematypes) */


#endif /* EMATYPES_INCLUDE */


