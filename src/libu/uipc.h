/* uipc SUPPORT (UNIX® System V System C Inter-Process-Communication) */
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
	u_shmget
	u_shmat
	u_shmctl
	u_shmdt
	u_msgget
	u_msgsnd
	u_msgrcv
	u_msgctl

	Description:
	UNIX® System V Inter-Process-Communication (IPC)

*******************************************************************************/

#ifndef	UIPC_INCLUDE
#define	UIPC_INCLUDE

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/shm.h>
#include	<sys/msg.h>
#include	<sys/sem.h>
#include	<unistd.h>
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

namespace uipc {
    struct uipc ;
    typedef int (uipc::*uipc_m)(int,void *) noex ;
    struct uipc {
	uipc_m		m ;
	char		*vec ;
	void		*vp ;
	void		*arg ;
	off_t		off ;
	int		pr ;
	int		fl ;
	int		fd ;
	int		attr ;
	int		mask ;
	uipc() noex { } ;
	uipc(int p,int f,int d,off_t o,void *v) noex {
	    pr = p ;
	    fl = f ;
	    fd = d ;
	    off = o ;
	    vp = v ;
	} ;
	uipc(int f) noex : fl(f) { } ;
	uipc(char *v) noex : vec(v) { } ;
	uipc(int c,void *a,int t,int m) noex {
	    fl = c ;
	    arg = a ;
	    attr = t ;
	    mask = m ;
	} ;
	int operator () (int,void *) noex ;
    } ; /* end struct (uipc) */
}

#endif /* __cplusplus */

EXTERNC_begin

extern int u_shmget(key_t,size_t,int) noex ;
extern int u_shmat(int,void *,int,void **) noex ;
extern int u_shmctl(int,int,SHMIDDS *) noex ;
extern int u_shmdt(void *) noex ;

extern int u_msgget(key_t,int) noex ;
extern int u_msgsnd(int,void *,int,int) noex ;
extern int u_msgrcv(int,void *,int,long,int) noex ;
extern int u_msgctl(int,int,MSQIDDS *) noex ;

EXTERNC_end


#endif /* UIPC_INCLUDE */


