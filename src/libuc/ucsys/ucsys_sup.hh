/* ucsys_sup HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UCSYSSUP object management */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Super-Group:
	ucsys

	Description:
	These subroutine groups provide access to the UNIX® system
	databases for UNIX® accounts and network entities.  The
	following groups are provied:
		ucsyspw		UNIX® 'passwd'		
		ucsyssp		UNIX® 'shadow'		
		ucsysgr		UNIX® 'group'		
		ucsyspj		UNIX® 'project'		
		ucsyspr		INET 'protocols'	
		ucsysnw		INET 'networks'
		ucsysho		INET 'hosts'		
		ucsyssv		INET 'services'

*******************************************************************************/

#ifndef	UCSYSSUP_INCLUDE
#define	UCSYSSUP_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace ucsys {
    extern errno_t getresolvec(int) noex ;
}


#endif /* __cplusplus (C++ only) */
#endif /* UCSYSSUP_INCLUDE */


