/* ufiledesc HEADER (UNIX® file operations) */
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
	ufiledesc

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UFILEDESC_INCLUDE
#define	UFILEDESC_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

#include	<uatx.h>
#include	<uconnect.h>
#include	<ufcntl.h>
#include	<ugetdents.h>
#include	<ulockfile.h>
#include	<ureade.h>
#include	<ureadext.h>
#include	<uregular.h>
#include	<uselect.h>
#include	<usocket.h>
#include	<usockaddr.h>
#include	<uterminal.h>


namespace libu {
    extern int ufstype(int fd,char *rbuf,int rlen) noex ;
    extern int ulockf(int fd,int cmd,off_t sz) noex ;
    extern int ureadp(int fd,void *rbuf,int rlen,off_t off) noex ;
    extern int uwritep(int fd,cvoid *wbuf,int wlen,off_t off) noex ;
    extern int uread(int fd,void *rbuf,int rlen) noex ;
    extern int ureadv(int fd,IOVEC *iop,int n) noex ;
    extern int ureade(int,void *,int,int,int) noex ;
    extern int useeko(int fd,off_t wo,int w,off_t *offp) noex ;
    extern int uwrite(int fd,cvoid *wbuf,int wlen) noex ;
    extern int uwritev(int fd,CIOVEC *iop,int n) noex ;
    extern int upoll(POLLFD *,int,int) noex ;
    extern int uselect(int,fdset *,fdset *,fdset *,TIMEVAL *) noex ;
    extern int uterminal(int) noex ;
    extern int utermattrget(int,TERMIOS *) noex ;
    extern int utermattrset(int,int,const TERMIOS *) noex ;
} /* end namespace (libu) */


#endif /* UFILEDESC_INCLUDE */


