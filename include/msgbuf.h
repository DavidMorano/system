/* msgbuf HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* message-buffering */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MSGBUF_INCLUDE
#define	MSGBUF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


#define	MSGBUF		struct msgbuf_head


struct msgbuf_head {
	char		*mbuf ;
	char		*bp ;
	int		fd ;
	int		to ;
	int		mlen ;
	int		bl ;
	int		neof ;
} ;

typedef	MSGBUF		msgbuf ;

EXTERNC_begin

extern int	msgbuf_start(msgbuf *,int,int,int) noex ;
extern int	msgbuf_read(msgbuf *,cchar **) noex ;
extern int	msgbuf_adv(msgbuf *,int) noex ;
extern int	msgbuf_update(msgbuf *,int) noex ;
extern int	msgbuf_finish(msgbuf *) noex ;

EXTERNC_end


#endif /* MSGBUF_INCLUDE */


