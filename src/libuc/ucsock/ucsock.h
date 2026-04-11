/* ucsock HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-index operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCSOCK_INCLUDE
#define	UCSOCK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

EXTERNC_begin

extern int	uc_bind(int,cvoid *,int) noex ;
extern int	uc_connect(int,cvoid *,int) noex ;
extern int	uc_connecte(int,cvoid *,int,int) noex ;
extern int	uc_setsockopt(int,int,int,int *,int) noex ;
extern int	uc_linger(int,int) noex ;
extern int	uc_recve(int,void *,int,int,int,int) noex ;
extern int	uc_recvfrome(int,void *,int,int,void *,int *,int,int) noex ;
extern int	uc_recvmsge(int,MSGHDR *,int,int,int) noex ;
extern int	uc_sockatmark(int) noex ;
extern int	uc_getsocktype(int) noex ;

extern int uc_sockjoin(int,SOCKADDR *,int,int,mode_t) noex ;
extern int uc_sockreuse(int) noex ;

static inline int uc_reuseaddr(int fd) noex {
    	return uc_sockreuse(fd) ;
}

EXTERNC_end


#endif /* UCSOCK_INCLUDE */


