/* projectent HEADER (obsoleted) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutines for simple PROJECT object (from UNIX® library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	projectent

	Descruption:
	These subroutines manage some simple tasks for the PROJECT
	object, referenced as 'struct project'.  This object is
	defined by UNIX® (really Solaris®) standards.

	Notes:
	This group is obsoleted and should be replaced by UCENTPJ.

*******************************************************************************/

#ifndef	PROJECTENT_INCLUDE
#define	PROJECTENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	PROJECTENT	PROJECT
#define	CPROJECTENT	const PROJECT

typedef	PROJECTENT	projectent ;

EXTERNC_begin

extern int projectent_load	(projectent *,char *,int,
					con projectent *) noex ;
extern int projectent_parse	(projectent *,char *,int,cchar *,int) noex ;
extern int projectent_size	(con projectent *) noex ;
extern int projectent_format	(con projectent *,char *,int) noex ;

EXTERNC_end


#endif /* projectentENT_INCLUDE */


