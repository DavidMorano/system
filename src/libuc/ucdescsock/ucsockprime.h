/* ucsockprime HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-index operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCSOCKPRIME_INCLUDE
#define	UCSOCKPRIME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

EXTERNC_begin

extern int	uc_bind(int,cvoid *,int) noex ;
extern int	uc_getpeername(int,void *,int *) noex ;
extern int	uc_getsockname(int,void *,int *) noex ;
extern int	uc_sockoptget(int,int,int,void *,int *) noex ;
extern int	uc_sockoptget(int,int,int,void *,int *) noex ;
extern int	uc_getsocktype(int) noex ;
extern int	uc_linger(int,int) noex ;
extern int	uc_listen(int,int) noex ;
extern int	uc_recv(int,void *,int,int) noex ;
extern int	uc_recve(int,void *,int,int,int,int) noex ;
extern int	uc_recvfrom(int,void *,int,int,void *,int *) noex ;
extern int	uc_recvfrome(int,void *,int,int,void *,int *,int,int) noex ;
extern int	uc_recvmsg(int,MSGHDR *,int) noex ;
extern int	uc_send(int,cvoid *,int,int) noex ;
extern int	uc_sendfiler(int,int,off_t,size_t) noex ;
extern int	uc_sendmsg(int,CMSGHDR *,int) noex ;
extern int	uc_sendto(int,cvoid *,int,int,cvoid *,int) noex ;
extern int	uc_sockoptset(int,int,int,cvoid *,int) noex ;
extern int	uc_shutdown(int,int) noex ;
extern int	uc_sockatmark(int) noex ;

EXTERNC_end


#endif /* UCSOCKPRIME_INCLUDE */


