/* umsg HEADER */
/* lang=C20 */

/* auxillary operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the subroutine declarations for the
	UNIX® system STREAMS message interface.

	Names:
	u_msgctl
	u_msgget
	u_msgrcv
	u_msgsnd

*******************************************************************************/

#ifndef	UMSG_INCLUDE
#define	UMSG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/msg.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	<usys.h>


#ifndef	TYPEDEF_SYSVMSGTYPE
#define	TYPEDEF_SYSVMSGTYPE
typedef long		sysvmsgtype ;
#endif

EXTERNC_begin

extern int u_msgget(key_t,int) noex ;
extern int u_msgctl(int,int,MSQIDDS *) noex ;
extern int u_msgrcv(int,void *,int,sysvmsgtype,int) noex ;
extern int u_msgsnd(int,void *,int,int) noex ;

EXTERNC_end


#endif /* UMSG_INCLUDE */


