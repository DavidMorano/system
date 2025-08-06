/* freadln HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* read a coded line from the STDIO stream */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FREADLN_INCLUDE
#define	FREADLN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdio.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	freadln(FILE *,char *,int) noex ;
extern int	fgetline(FILE *,char *,int) noex ;

EXTERNC_end


#endif /* FREADLN_INCLUDE */


