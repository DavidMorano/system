/* strnnlen HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perform variable counted c-string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNNLEN_INCLUDE
#define	STRNNLEN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int strnnlen(cchar *,int,int) noex ;

EXTERNC_end


#endif /* STRNNLEN_INCLUDE */


