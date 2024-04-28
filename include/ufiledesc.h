/* ufiledesc SUPPORT (UNIX® file operations) */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UFILEDESC_INCLUDE
#define	UFILEDESC_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>		/* |SOCKADDR| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>		/* |intptr_t| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<clanguage.h>


#ifdef	__cplusplus

namespace ufiledesc {
    struct ufiledescbase {
	cvoid		*wbuf ;
	void		*rbuf ;
	int		wlen ;
	int		rlen ;
	ufiledescbase() noex { } ;
	int operator () (int) noex ;
	virtual int callstd(int) noex = 0 ;
    } ; /* end struct (ufiledescbase) */
}

#endif /* __cplusplus */

EXTERNC_begin

extern int u_bind(int,cvoid *,int) noex ;
extern int u_listen(int,int) noex ;
extern int u_setsockopt(int,int,int,cvoid *,int) noex ;
extern int u_getsockopt(int,int,int,void *,int *) noex ;
extern int u_getpeername(int,void *,int *) noex ;
extern int u_getsockname(int,void *,int *) noex ;
extern int u_send(int,cvoid *,int,int) noex ;
extern int u_sendmsg(int,CMSGHDR *,int) noex ;
extern int u_sendto(int,cvoid *,int,int,cvoid *,int) noex ;
extern int u_recv(int,void *,int,int) noex ;
extern int u_recvmsg(int,MSGHDR *,int) noex ;
extern int u_recvfrom(int,void *,int,int,void *,int *) noex ;
extern int u_connect(int,cvoid *,int) noex ;
extern int u_shutdown(int,int) noex ;

EXTERNC_end


#endif /* UFILEDESC_INCLUDE */


