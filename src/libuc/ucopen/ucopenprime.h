/* ucopenprime HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucopenprime

	Description:
	No, we are not the same as UNIX® all of the time!  We return
	PFNOSUPPORT is a protocol-family (PF) is not supported!

*******************************************************************************/

#ifndef	UCOPENPRIME_INCLUDE
#define	UCOPENPRIME_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int uc_open	(cchar *,int,mode_t)			noex ;
extern int uc_opene	(cchar *,int,mode_t,int)		noex ;
extern int uc_openenv	(cchar *,int,mode_t,con mainv,int)	noex ;

extern int uc_socket	(int,int,int)				noex ;
extern int uc_dupmin	(int,int)				noex ;
extern int uc_dupmince	(int,int)				noex ;

local inline int uc_dup	(int fd) noex {
	return uc_dupmin(fd,0) ;
}

EXTERNC_end


#endif /* UCOPENPRIME_INCLUDE */


