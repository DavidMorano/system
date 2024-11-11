/* getsyslogx HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* get a sys-log (SYSLOG) facility or priority */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-10-14, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	GETSYSLOGX_INCLUDE
#define	GETSYSLOGX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	getlogfac(cchar *,int) noex ;
extern int	getsyslogpri(cchar *,int) noex ;
extern cchar	*strfacname(int) noex ;

EXTERNC_end


#endif /* GETSYSLOGX_INCLUDE */


