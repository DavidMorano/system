/* openusd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open UNIX®-Socket-Datagram */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:

	Description:
	This subroutine opens a UNIX® socket of the datagram variety.

	Synopsis:
	int openusd(cchar *sfn,int of,mode_t om)

	Arguments:
	sfn		socket file (in the UNIX® file-system)
	of		open-mode (O_CREAT will create the socket)
	om		permissions on socket-file

	Returns:
	>=0		file-descriptor
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<sockaddress.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_mkdir(cchar *,mode_t) noex ;
    extern int uc_mkfifo(cchar *,mode_t) noex ;
    extern int uc_chmod(cchar *,mode_t) noex ;
    extern int uc_stat(cchar *,ustat *) noex ;
    extern int uc_unlink(cchar *) noex ;
    extern int uc_unlinkshm(cchar *) noex ;
    extern int uc_open(cchar *,int,mode_t) noex ;
    extern int uc_socket(int,int,int) noex ;
    extern int uc_sockjoin(int,SOCKADDR *,int,int,mode_t) noex ;
    extern int uc_openshm(cchar *,int,mode_t) noex ;
    extern int uc_duper(int,int) noex ;
    extern int uc_pipe(int *) noex ;
    extern int uc_bind(int,cvoid *,int) noex ;
    extern int uc_fstat(int,ustat *) noex ;
    extern int uc_fchown(int,uid_t,gid_t) noex ;
    extern int uc_fminmod(int,mode_t) noex ;
    extern int uc_connect(int,cvoid *,int) noex ;
    extern int uc_connecte(int,cvoid *,int,int) noex ;
    extern int uc_read(int,void *,int) noex ;
    extern int uc_write(int,cvoid *,int) noex ;
    extern int uc_writen(int,cvoid *,int) noex ;
    extern int uc_iocctl(int,int,...) noex ;
    extern int uc_rewind(int) noex ;
    extern int uc_ftruncate(int,off_t ) noex ;
    extern int uc_closeonexec(int,int) noex ;
    extern int uc_fpathconf(int,int,char *) noex ;
    extern int uc_setsockopt(int,int,int,int *,int) noex ;
    extern int uc_linger(int,int) noex ;
    extern int uc_close(int) ;
} /* end extern */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int openusd(cchar *sfn,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (sfn) ylikely {
	    rs = SR_INVALID ;
	    if (sfn[0] && (of >= 0)) ylikely {
	        cint	pf = PF_UNIX ;
	        cint	st = SOCK_DGRAM ;
	        if ((rs = uc_socket(pf,st,0)) >= 0) ylikely {
	            sockaddress	sa ;
	            cint	af = AF_UNIX ;
	            fd = rs ;
	            if ((rs = sa.start(af,sfn,0,0)) >= 0) {
		        SOCKADDR	*sap = (SOCKADDR *) &sa ;
		        cint		sal = rs ;
		        {
		            rs = uc_sockjoin(fd,sap,sal,of,om) ;
		        }
	  	        rs1 = sa.finish ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (sockaddress) */
	            if (rs < 0) {
	                uc_close(fd) ;
		        fd = -1 ;
	            }
	        } /* end if (socket) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openusd) */


