/* usys_pathpid HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* Apple Darwin operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_pathpid

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSPATHPID_INCLUDE
#define	USYSPATHPID_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


namespace usys {
    extern sysret_t usys_pathpid(char *,int,pid_t) noex ;
    extern sysret_t usys_namepid(char *,int,pid_t) noex ;
}


#endif /* __cplusplus */
#endif /* USYSPATHPID_INCLUDE */


