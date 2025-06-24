/* poller HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* poll manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2006-09-10, David A­D­ Morano
	I created this from hacking something that was similar that
	was originally created for a PCS program.

*/

/* Copyright © 2006 David A­D­ Morano.  All rights reserved. */

#ifndef	POLLER_INCLUDE
#define	POLLER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<poll.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>


#define	POLLER_MAGIC	0x09854123
#define	POLLER		struct poller_head
#define	POLLER_SPEC	struct pollfd
#define	POLLER_CUR	struct poller_cursor


struct poller_cursor {
	int		i ;
} ;

struct poller_head {
	uint		magic ;
	vecobj		*rlp ;		/* register-list-pointer */
	POLLFD		*pa ;
	int		n ;		/* array number */
	int		e ;		/* array extent */
	int		nready ;
} ;

typedef	POLLER		poller ;
typedef	POLLER_SPEC	poller_spec ;
typedef	POLLER_CUR	poller_cur ;

EXTERNC_begin

extern int poller_start(poller*) noex ;
extern int poller_reg(poller*,poller_spec *) noex ;
extern int poller_cancel(poller*,poller_spec *) noex ;
extern int poller_cancelfd(poller*,int) noex ;
extern int poller_wait(poller*,poller_spec *,int) noex ;
extern int poller_get(poller*,poller_spec *) noex ;
extern int poller_curbegin(poller*,poller_cur *) noex ;
extern int poller_enum(poller*,poller_cur *,poller_spec *) noex ;
extern int poller_curend(poller*,poller_cur *) noex ;
extern int poller_finish(poller*) noex ;

EXTERNC_end


#endif /* POLLER_INCLUDE */


