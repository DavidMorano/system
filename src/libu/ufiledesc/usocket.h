/* usocket HEADER (UNIX® file operations) */
/* charset=ISO8859-1 */
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

#ifndef	USOCKET_INCLUDE
#define	USOCKET_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<clanguage.h>
#include	<usysbase.h>

EXTERNC_begin

extern int u_bind		(int,cvoid *,int) noex ;
extern int u_connect		(int,cvoid *,int) noex ;
extern int u_connecte		(int,cvoid *,int,int) noex ;
extern int u_getpeername	(int,void *,int *) noex ;
extern int u_getsockname	(int,void *,int *) noex ;
extern int u_getsockopt		(int,int,int,void *,int *) noex ;
extern int u_getsocktype	(int) noex ;
extern int u_linger		(int,int) noex ;
extern int u_listen		(int,int) noex ;
extern int u_recv		(int,void *,int,int) noex ;
extern int u_recve		(int,void *,int,int,int,int) noex ;
extern int u_recvfrom		(int,void *,int,int,void *,int *) noex ;
extern int u_recvmsg		(int,MSGHDR *,int) noex ;
extern int u_recvmsge		(int,MSGHDR *,int,int,int) noex ;
extern int u_send		(int,cvoid *,int,int) noex ;
extern int u_sendfile		(int,int,off_t,size_t) noex ;
extern int u_sendmsg		(int,CMSGHDR *,int) noex ;
extern int u_sendto		(int,cvoid *,int,int,cvoid *,int) noex ;
extern int u_setsockopt		(int,int,int,cvoid *,int) noex ;
extern int u_shutdown		(int,int) noex ;
extern int u_sockatmark		(int) noex ;

EXTERNC_end


#endif /* USOCKET_INCLUDE */


