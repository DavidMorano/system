/* listenconn HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutine to listen on a mounted FIFO for passed file-descriptors */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	LISTENCONN_INCLUDE
#define	LISTENCONN_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	listenconn(cchar *,mode_t,int) noex ;

EXTERNC_end


#endif /* LISTENCONN_INCLUDE */


