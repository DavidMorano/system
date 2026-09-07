/* uctermos HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-index operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCTERMIOS_INCLUDE
#define	UCTERMIOS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int uc_cfsetispeed(TERMIOS *,speed_t) noex ;
extern int uc_cfsetospeed(TERMIOS *,speed_t) noex ;

EXTERNC_end


#endif /* UCTERMIOS_INCLUDE */


