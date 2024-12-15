/* ecmsg HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* email-cover subchannel message */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-13, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	ECMSG_INCLUDE
#define	ECMSG_INCLUDE


#define	ECMSG			struct ecmsg_head
#define	ECMSG_MAXBUFLEN		(8 * 1024)


struct ecmsg_head {
	char		*ebuf ;
	int		elen ;
} ;

typedef	ECMSG		ecmsg ;

EXTERNC_begin

extern int ecmsg_start(ecmsg *) noex ;
extern int ecmsg_loadbuf(ecmsg *,cchar *,int) noex ;
extern int ecmsg_already(ecmsg *) noex ;
extern int ecmsg_finish(ecmsg *) noex ;

EXTERNC_end


#endif /* ECMSG_INCLUDE */


