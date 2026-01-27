/* mksofname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a program-root file-path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKSOFNAME_INCLUDE
#define	MKSOFNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	mksofname(char *rbuf,cchar *dn,cchar *name,cchar *ext) noex ;

EXTERNC_end


#endif /* MKSOFNAME_INCLUDE */


