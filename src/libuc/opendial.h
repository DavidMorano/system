/* opendial HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENDIAL_INCLUDE
#define	OPENDIAL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* options */
#define	OPENDIAL_OLONG		(1<<0)		/* FINGER "long" option */


enum opendials {
	opendial_unspec,
	opendial_udp,
	opendial_tcp,
	opendial_tcpmux,
	opendial_tcpnls,
	opendial_uss,
	opendial_ussmux,
	opendial_ussnls,
	opendial_ticotsord,
	opendial_ticotsordnls,
	opendial_pass,
	opendial_open,
	opendial_prog,
	opendial_finger,
	opendial_overlast
} ;

EXTERNC_begin

extern int opendial(int,int,cc *,cc *,cc *,mainv,mainv,int,int) noex ;

EXTERNC_end


#endif /* OPENDIAL_INCLUDE */


