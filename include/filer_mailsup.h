/* filer_mailsup HEADER */
/* lang=C20 */

/* support some buffered file operations */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-04-01, David A­D­ Morano
	This code was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#ifndef	FILEMAILSUP_INCLUDE
#define	FILEMAILSUP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>

#include	<filer.h>


EXTERNC_begin

extern int	filer_writehdr(filer *,cchar *,int) noex ;
extern int	filer_writehdrkey(filer *,cchar *) noex ;
extern int	filer_printlncont(filer *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* FILEMAILSUP_INCLUDE */


