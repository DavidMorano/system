/* progvmerr HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* VMAIL error-print subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VMAILERR_INCLUDE
#define	VMAILERR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<stddef.h>		/* POSIX® */
#include	<stdlib.h>		/* POSIX® */
#include	<stdarg.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	"proginfo.hh"


#ifndef	PI
#define	PI	proginfo
#endif


EXTERNC_begin
extern int progvmerr_printf	(PI *,cchar *,...) noex ;
extern int progvmerr_vprintf	(PI *,cchar *,va_list) noex ;
extern int progvmerr_print	(PI *,cchar *,int) noex ;
EXTERNC_end


#endif /* VMAILERR_INCLUDE */



