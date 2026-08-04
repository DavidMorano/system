/* logfile_copylock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform logging operations on a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	logfile

	Description:
	This is a module to operate on a logfile.  The subroutines
	in this module are:

		logfile_open
		logfile_write
		logfile_printf
		logfile_vprintf
		logfile_setid
		logfile_check
		logfile_checksize
		logfile_close

	Although masking interrupts during the locking of the logfile
	should NOT be necessary (since a terminated process releases
	all file locks that it may have), it is a good precaution
	against some stupid implementations of the NFS helper lock
	manager daemon.  We want to guard against a program terminating
	abruptly while its lock is retained (for a time) back at
	the server.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<sigblocker.h>		/* LIBUC */
#include	<ascii.h>		/* LIBUC */
#include	<lockfile.h>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<strw.h>		/* LIBUC |strwset(3uc)| */
#include	<strwcpy.h>		/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<mklogid.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |COLUMNS| */

#include	"logfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */
import logfile_mag ;

/* local defines */

#define	TO_OPEN		(10 * 60)
#define	TO_DATA		9
#define	TO_IDLE		(2*60)
#define	TO_LOCK		4


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_getpid() noex ;
} /* end extern */


/* external variables */


/* local structures */


/* forward references */

namespace {
    struct copier {
	logfile		*op ;
	int		logsz ;
	copier(logfile *o,int l) noex : op(o), logsz(l) { } ;
	operator int () noex ;
	int tmpfile() noex ;
	int lineup(char *,int) noex ;
	int copy(int) noex ;
    } ; /* end struct */
} /* end namespace */


/* local variables */

constexpr int		sigblockers[] = {
	SIGUSR1,
	SIGUSR2,
	SIGHUP,
	SIGTERM,
	SIGINT,
	SIGQUIT,
	SIGPIPE,
	0
} ; /* end array (sigblockers) */

cint		colbuflen = (COLUMNS + 2) ;


/* exported variables */


/* exported subroutines */

namespace logfile_util {
    int logfile_copylock(logfile *op,int logsz) noex {
	copier co(op,logsz) ;
	return co ;
    } /* end subroutine (logfile_copylock) */
} /* end namespace */


/* private subroutines */

copier::operator int () noex {
	int		rs ;
	int		rs1 ;
	if (sigblocker b ; (rs = b.start(sigblockers)) >= 0) {
	    if ((rs = lockfile(op->lfd,F_WLOCK,0z,0z,TO_LOCK)) >= 0) {
		{
		    rs = tmpfile() ;
		}	
	        rs1 = lockfile(op->lfd,F_ULOCK,0z,0z,TO_LOCK) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (lock-file) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return rs ;
} /* end method (copier::operator) */

int copier::tmpfile() noex {
    	cint		clen = colbuflen ;
    	int		rs ;
	int		rs1 ;
	if (char *cbuf ; (rs = lm_mall((clen + 1),&cbuf)) >= 0) {
            if ((rs = opentmp(nullptr,0,0644)) >= 0) {
                cint	fd = rs ;
		if ((rs = lineup(cbuf,clen)) >= 0) {
                    rs = copy(fd) ;
                } /* end if (ok) */
                rs1 = uc_close(fd) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (open-tmp) */
	    rs1 = lm_free(cbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (copier::tmpfile) */

int copier::lineup(char *cbuf,int clen) noex {
	coff		uoff = off_t(- logsz) ;
    	int		rs ;
	if ((rs = uc_seek(op->lfd,uoff,SEEK_END)) >= 0) {
	    while ((rs = uc_readln(op->lfd,cbuf,clen)) > 0) {
		if (cbuf[rs - 1] == '\n') break ;
	    } /* end while */
	} /* end if (seek) */
	return rs ;
} /* end method (copier::lineup) */

int copier::copy(int fd) noex {
    	int		rs ;
	if ((rs = uc_writedesc(fd,op->lfd,-1)) >= 0) {
	    uc_rewind(op->lfd) ;
	    uc_rewind(fd) ;
	    /* shut off any APPEND mode if there is any */
	    if ((rs = uc_setappend(op->lfd,false)) >= 0) {
		bool	f_append = (rs > 0) ;
		/* copy the temporary data back to the original file */
		if ((rs = uc_writedesc(op->lfd,fd,-1)) > 0) {
		    coff foff = off_t(rs) ;
	            rs = uc_ftruncate(op->lfd,foff) ;
		} /* end if (uc_writedesc) */
		/* turn on any APPEND mode if there it was on */
		if ((rs >= 0) && f_append) {
		    rs = uc_setappend(op->lfd,true) ;
		}
	    } /* end if (append-mode) */
	} /* end if (uc_writedesc) */
	return rs ;
} /* end method (copier::copy) */


