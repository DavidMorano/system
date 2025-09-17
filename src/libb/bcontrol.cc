/* bcontrol SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package similiar to the standard UNIX® "stdio" package */
/* version %I% last-modified %G% */

#define	CF_UNIXAPPEND	1		/* file append works? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine performs various control functions on the
	file. Some of these are:

	+ getting the file status from the kernel
	+ locking and unlocking the file
	+ truncating the file
	+ switching file buffering modes
	+ setting or getting the kernel file flags

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>		/* USTAT */
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstdarg>		/* |va_list(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	bcontrol_lock(bfile *,FLOCK *,int,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int bcontrol(bfile *op,int cmd,...) noex {
    	va_list		ap ;
	int		rs ;
	{
	    va_begin(ap,cmd) ;
	    rs = bcontrolv(op,cmd,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}

int bcontrolv(bfile *op,int cmd,va_list ap) noex {
	int		rs ;
	if ((rs = bfile_magic(op)) > 0) {
	    if ((rs = bfile_flush(op)) >= 0) {
	        FLOCK	fl, *fsp ;
	        int	fcmd ;
	        int	f_tc = FALSE ;
	        switch (cmd) {
	        case BC_TELL:
	            {
		        off_t	*bop ;
	                bop = (off_t *) va_arg(ap,void *) ;
		        if (bop != nullptr) {
	                    *bop = op->offset ;
		        } else {
		            rs = SR_FAULT ;
		        }
	            }
	            break ;
	        case BC_BUF:
	            op->bm = bfilebm_reg ;
	            break ;
	        case BC_LINEBUF:
	            op->bm = bfilebm_line ;
	            break ;
	        case BC_UNBUF:
	            op->bm = bfilebm_none ;
	            break ;
	        case BC_FD:
	            {
	                int *rip = (int *) va_arg(ap,int *) ;
	                rs = op->fd ;
		        if (rip != nullptr) *rip = op->fd ;
	            }
	            break ;
	        case BC_STAT:
	            {
	    	        USTAT	*ssp = (USTAT *) va_arg(ap,void *) ;
		        if (ssp != nullptr) {
	                    rs = u_fstat(op->fd,ssp) ;
	                } else {
		            rs = SR_FAULT ;
			}
	            }
	            break ;
	        case BC_TRUNCATE:
	            {
	                off_t toff = (off_t) va_arg(ap,off_t) ;
	                rs = uc_ftruncate(op->fd,toff) ;
	            }
	            break ;
	        case BC_CHMOD:
	            {
		        mode_t fm = (mode_t) va_arg(ap,uint) ;
	                rs = u_fchmod(op->fd,fm) ;
	            }
	            break ;
	        case BC_MINMOD:
	            {
		        mode_t fm = (mode_t) va_arg(ap,uint) ;
	                rs = uc_fminmod(op->fd,fm) ;
	            }
	            break ;
        /* file record locking functions */
	        case BC_UNLOCK:
	        case BC_LOCKREAD:
	        case BC_LOCKWRITE:
	            fsp = &fl ;
	            switch (cmd) {
	            case BC_LOCKREAD:
	                fsp->l_type = F_RDLCK ;
	                f_tc = TRUE ;
	                break ;
	            case BC_LOCKWRITE:
	                fsp->l_type = F_WRLCK ;
	                f_tc = TRUE ;
	                break ;
	            case BC_UNLOCK:
	            default:
	                fsp->l_type = F_UNLCK ;
	                break ;
	            } /* end switch */
	            fsp->l_whence = SEEK_SET ;
	            fsp->l_start = 0L ;
	            fsp->l_len = 0L ;
		    fallthrough ;
        /* fall through to next case */
        /* FALLTHROUGH */
        /* enter here for a lock (or unlock) of a requested area of the file */
	        case BC_SETLK:
	        case BC_SETLKW:
	            if (op->fl.writing && (op->len > 0)) {
	                rs = bfile_flush(op) ;
	            }
		    fallthrough ;
        /* fall through to next case */
        /* FALLTHROUGH */
	        case BC_GETLK:
	            switch (cmd) {
	            case BC_LOCKWRITE:
	            case BC_LOCKREAD:
	            case BC_SETLK:
	                fcmd = F_SETLK ;
	                break ;
	            case BC_UNLOCK:
	            case BC_SETLKW:
	                fcmd = F_SETLKW ;
	                break ;
	            case BC_GETLK:
	            default:
	                fcmd = F_GETLK ;
	                break ;
	            } /* end switch */
	            {
		        int	to = 0 ;
	                if (f_tc) {
		            to = (int) va_arg(ap,int) ;
	                    if (to < 0) {
	                        to = INT_MAX ;
	                        fcmd = F_SETLKW ;
	                    }
	                }
	                rs = bcontrol_lock(op,fsp,fcmd,f_tc,to) ;
	            }
	            break ;
	        case BC_INPARTLINE:
	            {
	                int	f = (int) va_arg(ap,int) ;
	                op->fl.inpartline = MKBOOL(f) ;
	            }
	            break ;
	        case BC_GETFL:
	            {
		        int	oflags ;
		        int *rip = (int *) va_arg(ap,int *) ;
	                rs = u_fcntl(op->fd,F_GETFL,0) ;
	                oflags = rs ;
	                if (rip != nullptr) {
	                    *rip = (rs >= 0) ? oflags : 0 ;
		        }
	            }
	            break ;
	        case BC_SETFL:
	            {
		        int *rip = (int *) va_arg(ap,int *) ;
		        if (rip != nullptr) {
		            int	v = *rip ;
	                    rs = u_fcntl(op->fd,F_SETFL,v) ;
	 	        } else {
		            rs = SR_FAULT ;
		        }
	            }
	            break ;
	        case BC_GETFDFL:
	            {
		        int *rip = (int *) va_arg(ap,int *) ;
		        if (rip != nullptr) {
		            int	v ;
	                    rs = u_fcntl(op->fd,F_GETFD,0) ;
		            v = rs ;
		            *rip = (rs >= 0) ? v : 0 ;
	 	        } else {
		            rs = SR_FAULT ;
		        }
	            }
	            break ;
	        case BC_SETFDFL:
	            {
		        int *rip = (int *) va_arg(ap,int *) ;
		        if (rip != nullptr) {
		            int v = *rip ;
	                    rs = u_fcntl(op->fd,F_SETFD,v) ;
		        } else {
		            rs = SR_FAULT ;
		        }
	            }
	            break ;
	        case BC_CLOSEONEXEC:
	            {
		        int v = (int) va_arg(ap,int) ;
	                rs = uc_closeonexec(op->fd,v) ;
	            }
	            break ;
	        case BC_BUFSIZE:
	            if ((rs = bfile_flush(op)) >= 0) {
	 	        int	bsize = (int) va_arg(ap,int) ;
	                char	*bdata ;
		        if (bsize <= 1024) {
		            bsize = op->pagesize ;
		        }
	                if ((rs = uc_valloc(bsize,&bdata)) >= 0) {
	                    if (op->bdata) {
	                        uc_free(op->bdata) ;
		            }
	                    op->bsize = bsize ;
	                    op->bdata = bdata ;
		            op->bbp = op->bdata ;
	                    op->bp = op->bdata ;
	                } /* end if (successful) */
	            } /* end if (was able to flush) */
	            break ;
	        case BC_DSYNC:
	            if ((rs = bfile_flush(op)) >= 0) {
	                rs = uc_fdatasync(op->fd) ;
	            } /* end if (was able to flush) */
	            break ;
	        case BC_SYNC:
	            if ((rs = bfile_flush(op)) >= 0) {
	                rs = uc_fsync(op->fd) ;
	            } /* end if (was able to flush) */
	            break ;
	        case BC_ISLINEBUF:
	            rs = (op->bm == bfilebm_line) ;
	            break ;
	        case BC_ISTERMINAL:
	            rs = op->fl.terminal ;
	            break ;
	        case BC_SETBUFLINE:
	            {
		        int f = (int) va_arg(ap,int) ;
		        op->bm = (f) ? bfilebm_line : bfilebm_reg ;
	            }
	            break ;
	        case BC_SETBUFNONE:
	            {
		        int f = (int) va_arg(ap,int) ;
	                op->bm = bfilebm_reg ;
		        if (f) op->bm = bfilebm_none ;
	            }
	            break ;
	        case BC_SETBUFDEF:
	            {
		        int f = (int) va_arg(ap,int) ;
	                op->bm = bfilebm_reg ;
		        if (f && op->fl.terminal) op->bm = bfilebm_line ;
	            }
	            break ;
	        case BC_GETBUFFLAGS:
	            {
		        int *rip = (int *) va_arg(ap,int *) ;
		        if (rip != nullptr) {
		            int 	v = 0 ;
		            if (op->fl.inpartline) v |= BFILE_FINPARTLINE ;
		            if (op->fl.terminal) v |= BFILE_FTERMINAL ;
		            switch (op->bm) {
		            case bfilebm_reg:
		                v |= BFILE_FBUFDEF ;
			        break ;
		            case bfilebm_atomic:
		                v |= BFILE_FBUFATOMIC ;
			        break ;
		            case bfilebm_line:
		                v |= BFILE_FBUFLINE ;
			        break ;
		            case bfilebm_none:
		                v |= BFILE_FBUFNONE ;
			        break ;
		            } /* end switch */
		        } else {
		            rs = SR_FAULT ;
		        }
	            }
	            break ;
	        case BC_SETBUFFLAGS:
	            {
		        int v = (int) va_arg(ap,int) ;
		        if (v & BFILE_FBUFNONE) {
		            op->bm = bfilebm_none ;
		        } else if (v & BFILE_FBUFLINE) {
		            op->bm = bfilebm_line ;
		        } else if (v & BFILE_FBUFATOMIC) {
		            op->bm = bfilebm_atomic ;
		        } else if (v & BFILE_FBUFDEF) {
		            int		bm = bfilebm_reg ;
		            if (op->fl.terminal) bm = bfilebm_line ;
		            op->bm = bm ;
		        }
		        op->fl.inpartline = MKBOOL(v & BFILE_FINPARTLINE) ;
	            }
	            break ;
	        case BC_NONBLOCK:
	            {
		        int v = (int) va_arg(ap,int) ;
		        int	f ;
		        f = (v > 0) ;
		        rs = uc_nonblock(op->fd,f) ;
	            }
	            break ;
        /* handle all other commands */
	        default:
	            rs = SR_INVALID ;
	            break ;
	        } /* end switch (handling different "cmd"s) */
	    } /* end if (bflush) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (bcontrol) */

int bsize(bfile *op) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = bcontrol(op,BC_STAT,&sb)) >= 0) {
	    rs = (sb.st_size & INT_MAX) ;
	}
	return rs ;
}
/* end subroutine (bsize) */

int bstat(bfile *op,USTAT *sbp) noex {
	return bcontrol(op,BC_STAT,sbp) ;
}
/* end subroutine (bstat) */


/* private subroutines */

static int bcontrol_lock(bfile *op,FLOCK *fsp,int fcmd,int f_tc,int to) noex {
	int		rs = SR_OK ;
	if (to < 0) to = INT_MAX ;
	for (int i = 0 ; i < to ; i += 1) {
	    if ((i > 0) && f_tc) {
	        sleep(1) ;
	    }
	    if ((rs = u_fcntl(op->fd,fcmd,fsp)) > 0) {
	        switch (rs) {
	        case SR_ACCES:
	        case SR_AGAIN:
	            rs = SR_LOCKED ;
	            break ;
	        case SR_DEADLK:
	            rs = SR_DEADLOCK ;
	            break ;
	        default:
	            rs = SR_NOTOPEN ;
		    break ;
	        } /* end switch */
	    } /* end if */
	    if ((! f_tc) || (rs != SR_LOCKED)) break ;
	} /* end for (looping-timing on fcntl) */
	return rs ;
}
/* end subroutine (bcontrol_lock) */


