/* ha HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* address type headers */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HA_INCLUDE
#define	HA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum has {
	ha_errorsto,
	ha_replyto,
	ha_sender,
	ha_from,
	ha_to,
	ha_cc,
	ha_bcc,
	ha_overlast
} ; /* end enum (has) */


extern const char	*ha[] ;


#endif /* HA_INCLUDE */


