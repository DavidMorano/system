/* hmatch HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* dial out to a system on the UUCP network */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HMATCH_INCLUDE
#define	HMATCH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	hmatch(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* HMATCH_INCLUDE */


