/* sntmtime HEADER */
/* lang=C20 */

/* counted c-string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	SNTMTIME_INCLUDE
#define	SNTMTIME_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


EXTERNC_begin

extern int sntmtime(char *,int,TMTIME *,cchar *) noex ;

EXTERNC_end


#endif /* SNTMTIME_INCLUDE */


