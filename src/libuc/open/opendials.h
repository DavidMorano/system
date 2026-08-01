/* opendials HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Open-Dial */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	OPENDIALS_INCLUDE
#define	OPENDIALS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


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
	opendial_ticotsordmux,
	opendial_pass,
	opendial_open,
	opendial_prog,
	opendial_finger,
	opendial_overlast
} ; /* end enum (opendials) */

#ifdef	__cplusplus

namespace libuc {
    struct opendialer {
        cchar	*name	[opendial_overlast + 1] ;
        consteval void mkname() noex ;
	consteval opendialer() noex ;
	int get(cchar *,int) const noex ;
    } ; /* end struct (opendialer) */
} /* end namespace (libuc) */

namespace libuc {
    extern const opendialer	opendial_data ;
} /* end namespace (libuc) */

#endif /* __cplusplus */


#endif /* OPENDIALS_INCLUDE */


