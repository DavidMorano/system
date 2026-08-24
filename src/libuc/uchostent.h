/* uchostent HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutines to retrieve various system information items */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCHOSTENT_INCLUDE
#define	UCHOSTENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<netdb.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	uc_hostentfree(HOSTENT *) noex ;

EXTERNC_end


#endif /* UCHOSTENT_INCLUDE */


