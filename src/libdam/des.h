/* des HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Data-Encryption-Standard pass-word check */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DES_INCLUDE
#define	DES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern cchar *des_fcrypt(cchar *,cchar *,char *) noex ;

EXTERNC_end


#endif /* DES_INCLUDE */


