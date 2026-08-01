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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<sntmtime.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<filemap.h>		/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| + |COLUMNS| */

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


/* external variables */


/* local structures */


/* forward references */

local int	process(cchar *,int) noex ;
local int	procfiler(filer *,cchar *,int) noex ;
local int	procfile_map(filer *,cchar *,int) noex ;
local int	procfile_reg(filer *,cchar *,int) noex ;
local int	printend(filer *,cchar *,cchar *,int) noex ;
local int	printsub(filer *,cchar *,cchar *,int) noex ;

local int	filer_char(filer *,int) noex ;

local bool	isreadable(int) noex ;


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
} /* end subroutine (opensys_banner) */


/* local subroutines */

local int process(cchar *ds,int f_top) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (int pipes[2] ; (rs = uc_pipe(pipes)) >= 0) {
	    cint	wfd = pipes[1] ;
	    fd = pipes[0] ;
	    if (filer wf ;
	    (rs = wf.start(wfd,0z,0,0)) >= 0) {
		{
	            rs = procfiler(&wf,ds,f_top) ;
		}
	        rs1 = wf.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    rs1 = uc_close(wfd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pipes) */
	if ((rs < 0) && (fd >= 0)) {
	    uc_close(fd) ;
	} /* end if (error) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (process) */

local int procfiler(filer *wfp,cchar *ds,int f_top) noex {
	int		rs ;
	if_constexpr (f_filemap) {
	    rs = procfile_map(wfp,ds,f_top) ;
	} else {
	    rs = procfile_reg(wfp,ds,f_top) ;
	}
	return rs ;
} /* end subroutine */

local int procfile_map(filer *wfp,cchar *ds,int f_top) noex {
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
} /* end subroutine (procfile_map) */

local int procfile_reg(filer *wfp,cchar *ds,int f_top) noex {
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
} /* end subroutine (procfile_reg) */

local int printend(filer *wfp,cchar *ds,cchar *lbuf,int len) noex {
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
} /* end subroutine (printend) */

local int printsub(filer *wfp,cchar *ds,cchar *lbuf,int llen) noex {
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
	} /* end if (ok) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (printsub) */

local int filer_char(filer *wfp,int ch) noex {
	char		wbuf[2] ;
	wbuf[0] = charconv(ch) ;
	wbuf[1] = '\0' ;
	return filer_write(wfp,wbuf,1) ;
} /* end subroutine (filer_char) */

local bool isreadable(int of) noex {
	bool		f = false ;
	f = f || ((of & O_RDONLY) == O_RDONLY) ;
	f = f || ((of & O_RDWR) == O_RDWR ) ;
	return f ;
} /* end subroutine (isreadable) */


