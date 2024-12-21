/* ucgetx HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* subroutines to retrieve various system information items */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCGETX_INCLUDE
#define	UCGETX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<netdb.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<ucgetloadavg.h>
#include	<ucgetpid.h>
#include	<ucgetrandom.h>
#include	<ucgethost.h>


EXTERNC_begin

extern int	uc_gethostid(ulong *) noex ;
extern int	uc_gethostname(char *,int) noex ;
extern int	uc_getarchitecture(char *,int) noex ;
extern int	uc_getnisdomain(char *,int) noex ;
extern int	uc_getloadavg(double *,int) noex ;

extern int	uc_gettimeofday(TIMEVAL *,void *) noex ;
extern int	uc_getcwd(char *,int) noex ;
extern int	uc_getauid() noex ;
extern int	uc_getpriority(int,id_t,int *) noex ;
extern int	uc_getpuid(pid_t) noex ;

/* NETWORK IPNODE database */
extern int	uc_getipnodebyname(HOSTENT **,cchar *,int,int) noex ;
extern int	uc_getipnodebyaddr(HOSTENT **,cvoid *,int,int) noex ;

extern int	uc_getnetname(char *) noex ;

/* NETWORK SOCKADDR combined database */
extern int	uc_getnameinfo(const SOCKADDR *,int,
			char *,int,char *,int,int) noex ;

/* why was this missing? */
extern int	uc_getlogin(char *,int) noex ;

static inline int uc_getarch(char *rb,int rl) noex {
    	return uc_getarchitecture(rb,rl) ;
}

EXTERNC_end


#endif /* UCGETX_INCLUDE */


