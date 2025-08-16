/* progpeername HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrive perr-name */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progpeername

	Description:
	Retrive the perr-name (of something).

*******************************************************************************/

#ifndef	PROGPEERNAME_INCLUDE
#define	PROGPEERNAME_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* must be before others */
#include	<cstdarg>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<clientinfo.h>

#include	<proginfo.hh>


EXTERNC_begin

extern int progpeername(proginfo *,clientinfo *,char *,int) noex ;
 
EXTERNC_end


#endif /* __cplusplus */
#endif /* PROGPEERNAME_INCLUDE */


