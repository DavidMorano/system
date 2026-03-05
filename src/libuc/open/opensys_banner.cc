/* opensys_banner SUPPORT (opem-system-banner) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Open the System Banner */
/* version %I% last-modified %G% */

#define	CF_FILEMAP	1		/* use |filemap(3uc)| */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	opsys_banner

	Description:
	Write ("date") the current date on the system banner.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<tmtime.hh>
#include	<sntmtime.h>
#include	<filer.h>
#include	<filemap.h>
#include	<rmx.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| + |COLUMNS| */

#include	"opensys.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	SYSBANNER	"/etc/banner"

#ifndef	CF_FILEMAP
#define	CF_FILEMAP	0		/* use |filemap(3uc)| */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


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
    extern int uc_fsize(int) noex ;
    extern int uc_fchown(int,uid_t,gid_t) noex ;
    extern int uc_fminmod(int,mode_t) noex ;
    extern int uc_connect(int,cvoid *,int) noex ;
    extern int uc_connecte(int,cvoid *,int,int) noex ;
    extern int uc_lockf(int,int,off_t) noex ;
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

static int	process(cchar *,int) noex ;
static int	procfiler(filer *,cchar *,int) noex ;
static int	procfile_map(filer *,cchar *,int) noex ;
static int	procfile_reg(filer *,cchar *,int) noex ;
static int	printend(filer *,cchar *,cchar *,int) noex ;
static int	printsub(filer *,cchar *,cchar *,int) noex ;

static int	filer_char(filer *,int) noex ;

static bool	isreadable(int) noex ;


/* local variables */

constexpr cchar		sysbanner[] = SYSBANNER ;

constexpr bool		f_filemap = CF_FILEMAP ;


/* exported variables */


/* exported subroutines */

int opensys_banner(cchar *fname,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	(void) om ;
	if (fname) {
	    rs = SR_INVALID ;
	    if ((of >= 0) && isreadable(of)) {
	        const time_t	dt = time(nullptr) ;
	        int		f_top = true ;
	        cchar		*tspec = "%e %b %T" ;
	        if (TMTIME tmd ; (rs = tmtime_timegm(&tmd,dt)) >= 0) {
	            cint	tlen = TIMEBUFLEN ;
	            char	tbuf[TIMEBUFLEN+1] ;
	            if ((rs = sntmtime(tbuf,tlen,&tmd,tspec)) >= 0) {
	                rs = process(tbuf,f_top) ;
	                fd = rs ;
	            }
	        } /* end if (tmtime_timegm) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opensys_banner) */


/* local subroutines */

static int process(cchar *ds,int f_top) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	int		pipes[2] ;
	if ((rs = uc_pipe(pipes)) >= 0) {
	    filer	wfile, *wfp = &wfile ;
	    cint	wfd = pipes[1] ;
	    fd = pipes[0] ;
	    if ((rs = filer_start(wfp,wfd,0z,0,0)) >= 0) {
		{
	            rs = procfiler(wfp,ds,f_top) ;
		}
	        rs1 = filer_finish(wfp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    rs1 = uc_close(wfd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pipes) */
	if ((rs < 0) && (fd >= 0)) {
	    uc_close(fd) ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (process) */

static int procfiler(filer *wfp,cchar *ds,int f_top) noex {
	int		rs ;
	if_constexpr (f_filemap) {
	    rs = procfile_map(wfp,ds,f_top) ;
	} else {
	    rs = procfile_reg(wfp,ds,f_top) ;
	}
	return rs ;
}

static int procfile_map(filer *wfp,cchar *ds,int f_top) noex {
	filemap		sysban, *sfp = &sysban ;
	cint		maxsize = (5*1024) ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = filemap_open(sfp,sysbanner,maxsize)) >= 0) {
	    int		line = 0 ;
	    cchar	*lbuf{} ;
	    while ((rs = filemap_getln(sfp,&lbuf)) > 0) {
	        cint	len = rmeol(lbuf,rs) ;
	        if (f_top && (line == 0)) {
	            rs = printend(wfp,ds,lbuf,len) ;
		    wlen += rs ;
	        } else if ((! f_top) && (line == 5)) {
	            rs = printsub(wfp,ds,lbuf,len) ;
	            wlen += rs ;
	        } else {
	            rs = filer_println(wfp,lbuf,len) ;
	            wlen += rs ;
	        }
	        line += 1 ;
	        if (rs < 0) break ;
	    } /* end while (reading lines) */
	    rs1 = filemap_close(sfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filemap) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procfile_map) */

static int procfile_reg(filer *wfp,cchar *ds,int f_top) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (char *lbuf ; (rs = lm_mp(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    cint	of = O_RDONLY ;
	    cint	to = -1 ;
	    cmode	om = 0666 ;
	    if ((rs = uc_open(sysbanner,of,om)) >= 0) {
	        filer	sysban, *sfp = &sysban ;
	        cint	fd = rs ;
	        if ((rs = filer_start(sfp,fd,0z,0,0)) >= 0) {
	            int		line = 0 ;
	            while ((rs = filer_readln(sfp,lbuf,llen,to)) > 0) {
	                cint	len = rmeol(lbuf,rs) ;
	                if (f_top && (line == 0)) {
	                    rs = printend(wfp,ds,lbuf,len) ;
		    	    wlen += rs ;
	                } else if ((! f_top) && (line == 5)) {
	                    rs = printsub(wfp,ds,lbuf,len) ;
	                    wlen += rs ;
	                } else {
	                    rs = filer_println(wfp,lbuf,len) ;
	                    wlen += rs ;
	                }
	                line += 1 ;
	                if (rs < 0) break ;
	            } /* end while (reading lines) */
	            rs1 = filer_finish(sfp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (filer) */
	        rs1 = uc_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (output-file) */
	    rs1 = lm_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procfile_reg) */

static int printend(filer *wfp,cchar *ds,cchar *lbuf,int len) noex {
	cint		cols = COLUMNS ;
	cint		dl = lenstr(ds) ;
	int		rs = SR_OK ;
	int		breaklen ;
	int		ml ;
	int		i = 0 ;
	int		wlen = 0 ;
	breaklen = (cols - dl) ;
	if ((rs >= 0) && (i < breaklen)) {
	    ml = min(len,breaklen) ;
	    rs = filer_write(wfp,(lbuf+i),ml) ;
	    wlen += rs ;
	    i += rs ;
	}
	if ((rs >= 0) && (i < breaklen)) {
	    ml = (breaklen-i) ;
	    rs = filer_writeblanks(wfp,ml) ;
	    wlen += rs ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = filer_write(wfp,ds,dl) ;
	    wlen += rs ;
	}
	if (rs >= 0) {
	    rs = filer_char(wfp,'\n') ;
	    wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printend) */

static int printsub(filer *wfp,cchar *ds,cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		di = 0 ;
	int		wlen = 0 ;
	for (int i = 0 ; (i < llen) && lbuf[i] ; i += 1) {
	    int		ch = mkchar(lbuf[i]) ;
	    if ((! CHAR_ISWHITE(ch)) && (ds[di] != '\0')) {
	        if (ds[di] != ' ') ch = ds[di] ;
	        di += 1 ;
	    }
	    rs = filer_char(wfp,ch) ;
	    wlen += rs ;
	} /* end for */
	if (rs >= 0) {
	    rs = filer_char(wfp,'\n') ;
	    wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printsub) */

static int filer_char(filer *wfp,int ch) noex {
	char		wbuf[2] ;
	wbuf[0] = charconv(ch) ;
	wbuf[1] = '\0' ;
	return filer_write(wfp,wbuf,1) ;
}
/* end subroutine (filer_char) */

static bool isreadable(int of) noex {
	bool		f = false ;
	f = f || ((of & O_RDONLY) == O_RDONLY) ;
	f = f || ((of & O_RDWR) == O_RDWR ) ;
	return f ;
}
/* end subroutine (isreadable) */


