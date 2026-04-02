/* acceptpass HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* accept a passed file-descriptor (FD) from a UNIX® pipe */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	acceptpass

	Description:
	This file contains some support for the UNIX® Address-Familt
	(AF) related operations.

*******************************************************************************/

#ifndef	ACCEPTPASS_INCLUDE
#define	ACCEPTPASS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	acceptpass(int,STRRECVFD *,int) noex ;

EXTERNC_end


#endif /* ACCEPTPASS_INCLUDE */


