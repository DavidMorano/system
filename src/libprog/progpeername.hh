/* proglog HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program-logging */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Manage program logging operations.

*******************************************************************************/

#ifndef	PROGLOG_INCLUDE
#define	PROGLOG_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* must be before others */
#include	<stdarg.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<clientinfo.h>

#include	<proginfo.hh>


EXTERNC_begin

extern int progpeername(proginfo *,clientinfo *,char *,int) noex ;
 
EXTERNC_end


#endif /* __cplusplus */
#endif /* PROGLOG_INCLUDE */


