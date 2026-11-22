/* usys_getxid HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_getxid

	Description:
	These subroutines are not part of either the original UNIX®
	not the POSIX® knock-off.  But some other systems had these.

*******************************************************************************/

#ifndef	USYSGETXID_INCLUDE
#define	USYSGETXID_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifndef	DECLARATION_GETXID
#define	DECLARATION_GETXID
EXTERNC_begin
    local inline uid_t getruid() noex {
	return getuid() ;
    } /* end sub */
    local inline gid_t getrgid() noex {
	return getgid() ;
    } /* end sub */
EXTERNC_end
#endif /* DECLARATION_GETXID */


#endif /* USYSGETXID_INCLUDE */


