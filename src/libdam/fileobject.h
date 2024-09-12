/* fileobject HEADER */
/* lang=C20 */

/* determine if named file is an object file */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-11-24, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Given a file-name we determine if it is an object file (or
	core file).

*******************************************************************************/

#ifndef	FILEOBJECT_INCLUDE
#define	FILEOBJECT_INCLUDE


#include	<envstandards.h>	/* must be before others */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	fileobject(cchar *) noex ;

EXTERNC_end


#endif /* FILEOBJECT_INCLUDE */


