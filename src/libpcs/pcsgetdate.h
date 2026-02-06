/* pcsgetdate HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* parse a date string into its componenent values */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-02-01, David A­D­ Morano
	This subroutine parses a date string, of a specified type,
	into its componenent values.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSGETDATE_INCLUDE
#define	PCSGETDATE_INCLUDE
#ifdef	__cplusplus /* C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<tmtime.hh>


enum pcsdatetypes {
    pcsdatetype_env,
    pcsdatetype_msg,
    pcsdatetype_overlast
} ; /* end enum (pcsdatetypes) */

EXTERNC_begin

extern int pcsgetdate(cc *,tmtime *,cc *,int,pcsdatetypes) noex ;

EXTERNC_end


#endif /* __cplusplus (C++ only) */
#endif /* PCSGETDATE_INCLUDE */


