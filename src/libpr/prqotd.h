/* prqotd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* maintenance of the QOTD facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PRQOTD_INCLUDE
#define	PRQOTD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	<prqotd_config.hh>


#ifndef	PRQOTD_SN
#define	PRQOTD_SN	"maintqotd"
#endif
#ifndef	PRQOTD_PROGEXEC
#define	PRQOTD_PROGEXEC	"qotd"
#endif
#ifndef	PRQOTD_VARSPOOL
#define	PRQOTD_VARSPOOL	"var/spool"
#endif

#define	PRQOTD_PRNAME	"LOCAL"
#define	PRQOTD_PROG	"helloworld"


EXTERNC_begin

extern int	prqotd_open	(cchar *,int,int,int) noex ;
extern int	prqotd_maint	(cchar *,int,int,int) noex ;

EXTERNC_end


#endif /* PRQOTD_INCLUDE */


