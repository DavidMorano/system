/* strnset HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perform variable counted c-string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNSET_INCLUDE
#define	STRNSET_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern char *strnset(char *dp,int ch,int n) noex ;

EXTERNC_end


#endif /* STRNSET_INCLUDE */


