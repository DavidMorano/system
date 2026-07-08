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
#include	<sys/types.h>		/* POSIX */
#include	<poll.h>		/* POSIX */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */


#define	POLLER		struct poller_head
#define	POLLER_SPEC	struct pollfd
#define	POLLER_CUR	struct poller_cursor
#define	POLLER_MAGIC	0x09854123


struct poller_cursor {
	int		i ;
} ; /* end struct */

struct poller_head {
	vecobj		*rlp ;		/* register-list-pointer */
	POLLFD		*pa ;
	uint		magval ;
	int		n ;		/* array number */
	int		e ;		/* array extent */
	int		nready ;
} ; /* end struct */

typedef	POLLER		poller ;
typedef	POLLER_SPEC	poller_spec ;
typedef	POLLER_CUR	poller_cur ;

EXTERNC_begin

extern int poller_start		(poller*) noex ;
extern int poller_reg		(poller*,poller_spec *) noex ;
extern int poller_cancel	(poller*,poller_spec *) noex ;
extern int poller_cancelfd	(poller*,int) noex ;
extern int poller_wait		(poller*,poller_spec *,int) noex ;
extern int poller_get		(poller*,poller_spec *) noex ;
extern int poller_curbegin	(poller*,poller_cur *) noex ;
extern int poller_curenum	(poller*,poller_cur *,poller_spec *) noex ;
extern int poller_curend	(poller*,poller_cur *) noex ;
extern int poller_finish	(poller*) noex ;

EXTERNC_end


#endif /* POLLER_INCLUDE */


