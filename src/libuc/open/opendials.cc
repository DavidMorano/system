/* opendials SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open a channel using a dialer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	opendials

	Description:
	We brought several dialers together here to make it more
	convenient to allow for a range of dialers to be used for
	connecting to some service.

	Synopsis:
	int opendial(int dialer ;int af,int *hn,cc *ps,cc *svc,
		mainv av ;mainv ev,int to,int opts) noex

	Arguments:
	dialer		dialer to use
	af		socket address-family
	hn		host (string)
	ps		port (string)
	svc		service (string)
	av		argument string vector
	ev		argument string vector
	opts		options
	to		time-out

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<matstr.h>		/* LIBUC |matstr(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"opendials.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

namespace libuc {
    consteval void opendialer::mkname() noex {
	name[opendial_unspec]		= "unspec" ; 
	name[opendial_udp] 		= "udp" ; 
	name[opendial_tcp] 		= "tcp" ; 
	name[opendial_tcpmux]		= "tcpmux" ; 
	name[opendial_tcpnls]		= "tcpnls" ; 
	name[opendial_uss] 		= "uss" ; 
	name[opendial_ussmux]		= "ussmux" ; 
	name[opendial_ussnls]		= "ussnls" ; 
	name[opendial_ticotsord]	= "ticotsord" ;
	name[opendial_ticotsordnls]	= "ticotsordnls" ;
	name[opendial_ticotsordmux]	= "ticotsordmux" ; 
	name[opendial_pass] 		= "pass" ; 
	name[opendial_open]		= "open" ; 
	name[opendial_prog]		= "prog" ; 
	name[opendial_finger]		= "finger" ; 
	name[opendial_overlast] 	= nullptr ;
    } ; /* end metbod (opendialer::mkname) */
    consteval opendialer::opendialer() noex {
	mkname() ;
    } ; /* end ctor */
    int opendialer::get(cchar *sp,int sl) const noex {
    	return matstr(name,sp,sl) ;
    } ; /* end method (get) */
} /* end namespace (libuc) */

namespace libuc {
    constexpr opendialer	opendial_data ;
} /* end namespace (libuc) */


/* exported subroutines */


/* local subroutines */


