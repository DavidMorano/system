/* filer_mailsup HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* support some buffered file operations for email purposes */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-04-01, David A­D­ Morano
	This code was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	filer_mailsup

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#ifndef	FILEMAILSUP_INCLUDE
#define	FILEMAILSUP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<filer.h>


EXTERNC_begin

extern int	filer_writehdr(filer *,cchar *,int) noex ;
extern int	filer_writehdrkey(filer *,cchar *) noex ;
extern int	filer_printlncont(filer *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* FILEMAILSUP_INCLUDE */


