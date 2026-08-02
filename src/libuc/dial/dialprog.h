/* dialprog HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* set options on dialer file-descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIALPROG_INCLUDE
#define	DIALPROG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<dialopts.h>		/* LIBUC */


EXTERNC_begin

extern int dialprog(cchar *,int,mainv,mainv,int *) noex ;

EXTERNC_end


#endif /* DIALPROG_INCLUDE */


