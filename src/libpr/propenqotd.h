/* prmaintqotd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* maintenance of the QOTD facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PRMAINTQOTD_INCLUDE
#define	PRMAINTQOTD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	prmaintqotd(cchar *,int,int,int) noex ;

EXTERNC_end


#endif /* PRMAINTQOTD_INCLUDE */


