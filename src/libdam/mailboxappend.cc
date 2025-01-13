/* mailboxappend SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* append some data (presumably a mail-message) to the end of a mailbox */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This is new code to replace some previous miscellaneous
	code used for the same purpose.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mailboxappend

	Description:
	This subroutine appends some data to the end of a mailbox
	file.

	Synopsis:
	int mailboxappend(cchar *mbfname,int sfd,int slen,int to) noex

	Arguments:
	- mbfname	file-name of mailbox
	- sfd		source file-descriptor
	- slen		source length to write (starting from current offset)
	- to		timeout waiting for mailbox lock

	Returns:
	>=0		amount written
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sigblocker.h>
#include	<lockfile.h>
#include	<localmisc.h>

#include	"mailboxappend.h"


/* local defines */

#ifndef	TO_LOCK
#define	TO_LOCK		4
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	mailboxappender(cchar *,int,int,int) noex ;
static int	mblock(int,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailboxappend(cchar *mbfname,int sfd,int slen,int to) noex {
	cint		nrs = SR_ACCESS ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (mbfname) {
	    rs = SR_INVALID ;
	    if (mbfname[0]) {
	    	rs = SR_BADF ;
		if (sfd >= 0) {
	            if ((rs = mailboxappender(mbfname,sfd,slen,to)) == nrs) {
	                if ((rs = u_access(mbfname,W_OK)) >= 0) {
	                    const uid_t	uid = getuid() ;
	                    const uid_t	euid = geteuid() ;
	                    if (uid != euid) {
		                if ((rs = u_seteuid(uid)) >= 0) {
		                    rs = mailboxappender(mbfname,sfd,slen,to) ;
		                    wlen = rs ;
		                    rs1 = u_seteuid(euid) ;
		                    if (rs >= 0) rs = rs1 ;
		                } else
		                    rs = nrs ;
	                    } else
		                rs = nrs ;
	                } /* end if (u_access) */
	            } else {
	                wlen = rs ;
		    }
	        } /* end if (good-FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailboxappend) */


/* local subroutines */

static int mailboxappender(cchar *mbfname,int sfd,int slen,int to) noex {
	cmode		om = 0666 ;
	cint		of = (O_CREAT|O_WRONLY|O_APPEND) ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = uc_open(mbfname,of,om)) >= 0) {
	    cint	tfd = rs ;
	    if ((rs = mblock(tfd,true,to)) >= 0) {
		cint	w = SEEK_END ;
		if (off_t offend ; (rs = uc_seeko(tfd,0L,w,&offend)) >= 0) {
		    int	cmd ;
		    if (sigblocker sb ; (rs = sb.start) >= 0) {
		        {
	                    rs = uc_writedesc(tfd,sfd,slen) ;
	                    wlen = rs ;
		        }
	                if (rs < 0) {
	                    uc_ftruncate(tfd,offend) ;
		        }
	                rs1 = sb.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end block (sigblock) */
	            cmd = F_UNLOCK ;
	            rs1 = lockfile(tfd,cmd,offend,0z,0) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (uc_seeko) */
	    } /* end if (lock) */
	    rs1 = u_close(tfd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailboxappender) */

static int mblock(int fd,int f_lock,int to) noex {
	FLOCK		fl{} ;
	int		rs = SR_OK ;
	fl.l_type = (f_lock) ? F_WRLCK : F_UNLCK ;
	fl.l_whence = SEEK_END ;
	fl.l_start = 0L ;
	fl.l_len = 0 ;
	if (to < 0) to = TO_LOCK ;
	for (int i = 0 ; i < (to + 1) ; i += 1) {
	    rs = u_fcntl(fd,F_SETLK,&fl) ;
	    if ((rs != SR_AGAIN) && (rs != SR_ACCES))
	        break ;
	    if (i < to) {
	        uc_safesleep(1) ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (mblock) */


