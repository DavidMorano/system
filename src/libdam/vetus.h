/* vetus HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* VETU (Version-Endian-Type-Unused) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VETUS_INCLUDE
#define	VETUS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


enum vetus {
    vetu_version,
    vetu_endian,
    vetu_type,
    vetu_unused,
    vetu_overlast
} ; /* end enum (vetus) */


#endif /* VETUS_INCLUDE */


