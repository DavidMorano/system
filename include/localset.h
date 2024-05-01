/* localset HEADER */
/* lang=C20 */

/* LOCAL get values */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	LOCALSET_INCLUDE
#define	LOCALSET_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int localsetnetload(cchar *,cchar *,int) noex ;
extern int localsetsystat(cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* LOCALSET_INCLUDE */


