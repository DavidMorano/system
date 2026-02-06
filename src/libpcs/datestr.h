/* datestr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PCS date-string creation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-01-01, David A­D­ Morano
	This subroutine was written to replace the previous one.
	This version simply calls |timestr_edate()|.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DATESTR_INCLUDE
#define	DATESTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern char *datestr_envelope(time_t,char *) noex ;
extern char *datestr_header(time_t,char *) noex ;

EXTERNC_end


#endif /* DATESTR_INCLUDE */


