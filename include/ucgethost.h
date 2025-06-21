/* ucgethost HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* auxillary operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_gethostbegin
	uc_gethostent
	uc_gethostend
	uc_gethostbyname
	uc_gethostbyaddr

	Description:
	These subroutines retrive a HOSTENT object from the system (name
	service).

*******************************************************************************/

#ifndef	UCGETHOST_INCLUDE
#define	UCGETHOST_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<netdb.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int uc_gethostbegin(int) noex ;
extern int uc_gethostent(HOSTENT *,char *,int) noex ;
extern int uc_gethostend() noex ;
extern int uc_gethostbyname(HOSTENT *,char *,int,cc *) noex ;
extern int uc_gethostbyaddr(HOSTENT *,char *,int,int,cc *,int) noex ;

EXTERNC_end


#endif /* UCGETHOST_INCLUDE */


