/* dialtcpnls HEADER */
/* lang=C20 */

/* dial out to a TCP port and using the NLS connect protocol */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIALTCPNLS_INCLUDE
#define	DIALTCPNLS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<dialopts.h>


EXTERNC_begin

extern int dialtcpnls(cc *,cc *,int,cc *,int,int) noex ;

EXTERNC_end


#endif /* DIALTCPNLS_INCLUDE */


