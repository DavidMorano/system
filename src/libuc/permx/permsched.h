/* permsched HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* stat-perm (Status-Permissions) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-05, David A­D­ Morano
	This module was adapted from assembly lanauge.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Names:

	Description:
	We check for specified permissions on a file given an IDS
	object (for the user to check permissions for) and the
	|stat(2)| of the file in question.

******************************************************************************/

#ifndef	PERMSCHED_INCLUDE
#define	PERMSCHED_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* UNIX® system types */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecstr.h>


EXTERNC_begin

extern int permsched(mainv,vecstr *,char *,int,cc *,mode_t) noex ;

EXTERNC_end


#endif /* PERMSCHED_INCLUDE */


