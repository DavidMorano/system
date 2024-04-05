/* opensys_banner SUPPORT (opem-system-banner) */
/* lang=C++20 */

/* Open the System Banner */
/* version %I% last-modified %G% */

#define	CF_filemap	1		/* use |filemap(3uc)| */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Write ("date") the current date on the system banner.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<tmtime.h>
#include	<sntmtime.h>
#include	<filebuf.h>
#include	<filemap.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */


/* local defines */

#define	SYSBANNER	"/etc/banner"

#ifndef	COLUMNS
#define	COLUMNS		80
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	process(int,mode_t,cchar *,int) noex ;
static int	procfile(filebuf *,int,mode_t,cchar *,int) noex ;
static int	printend(filebuf *,cchar *,cchar *,int) noex ;
static int	printsub(filebuf *,cchar *,cchar *,int) noex ;
static int	filebuf_char(filebuf *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int opensys_banner(cchar *fname,int of,mode_t om) noex {
	TMTIME		tm ;
	const time_t	dt = time(NULL) ;
	int		rs ;
	int		fd = -1 ;
	int		f_top = TRUE ;
	cchar		*tspec = "%e %b %T" ;

	if (fname == NULL) return SR_FAULT ;

	if ((rs = tmtime_gmtime(&tm,dt)) >= 0) {
	    char	ds[TIMEBUFLEN+1] ;
	    if ((rs = sntmtime(ds,TIMEBUFLEN,&tm,tspec)) >= 0) {
	        rs = process(of,om,ds,f_top) ;
	        fd = rs ;
	    }
	} /* end if (tmtime_gmtime) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opensys_banner) */


/* local subroutines */

static int process(int of,mode_t om,cchar *ds,int f_top) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	int		pipes[2] ;
	if ((rs = u_pipe(pipes)) >= 0) {
	    filebuf	wfile, *wfp = &wfile ;
	    cint	wfd = pipes[1] ;
	    fd = pipes[0] ;
	    if ((rs = filebuf_start(wfp,wfd,0L,0,0)) >= 0) {
	        rs = procfile(wfp,of,om,ds,f_top) ;
	        rs1 = filebuf_finish(wfp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filebuf) */
	    rs1 = u_close(wfd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pipes) */
	if ((rs < 0) && (fd >= 0)) {
	    u_close(fd) ;
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (process) */

#if	CF_filemap
/* ARGSUSED */
static int procfile(filebuf *wfp,int of,mode_t om,cchar *ds,int f_top) noex {
	filemap		sysban, *sfp = &sysban ;
	cint		maxsize = (5*1024) ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*sysbanner = SYSBANNER ;
	if ((rs = filemap_open(sfp,sysbanner,of,maxsize)) >= 0) {
	    int		line = 0 ;
	    cchar	*lbuf{} ;
	    while ((rs = filemap_getln(sfp,&lbuf)) > 0) {
	        int	len = rs ;

	        if (lbuf[len-1] == '\n') len -= 1 ;

	        if (f_top && (line == 0)) {
	            rs = printend(wfp,ds,lbuf,len) ;
		    wlen += rs ;
	        } else if ((! f_top) && (line == 5)) {
	            rs = printsub(wfp,ds,lbuf,len) ;
	            wlen += rs ;
	        } else {
	            rs = filebuf_println(wfp,lbuf,len) ;
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
/* end subroutine (procfile) */
#else /* CF_filemap */
static int procfile(filebuf *wfp,int of,mode_t om,cchar *ds,int f_top) noex {
	cint		of = O_RDONLY ;
	cint		to = -1 ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*sysbanner = SYSBANNER ;
	if ((rs = uc_open(sysbanner,of,0666)) >= 0) {
	    filebuf	sysban, *sfp = &sysban ;
	    cint	fd = rs ;
	    if ((rs = filebuf_start(sfp,fd,0L,0,0)) >= 0) {
	        cint		llen = LINEBUFLEN ;
	        char		*lbuf{} ;
	        if ((rs = uc_malloc((llen+1),&lbuf)) >= 0) {
	            int		line = 0 ;
	            while ((rs = filebuf_readln(sfp,lbuf,llen,to)) > 0) {
	                int		len = rs ;

	        	if (lbuf[len-1] == '\n') len -= 1 ;

	                if (f_top && (line == 0)) {
	                    rs = printend(wfp,ds,lbuf,len) ;
		    	    wlen += rs ;
	                } else if ((! f_top) && (line == 5)) {
	                    rs = printsub(wfp,ds,lbuf,len) ;
	                    wlen += rs ;
	                } else {
	                    rs = filebuf_println(wfp,lbuf,len) ;
	                    wlen += rs ;
	                }

	                line += 1 ;
	                if (rs < 0) break ;
	            } /* end while (reading lines) */

	            rs1 = uc_free(lbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a) */
	        rs1 = filebuf_finish(sfp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filebuf) */
	    rs1 = uc_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (output-file) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procfile) */
#endif /* CF_filemap */

static int printend(filebuf *wfp,cchar *ds,cchar *lbuf,int len) noex {
	cint		cols = COLUMNS ;
	cint		dl = strlen(ds) ;
	int		rs = SR_OK ;
	int		breaklen ;
	int		ml ;
	int		i = 0 ;
	int		wlen = 0 ;

	breaklen = (cols - dl) ;

	if ((rs >= 0) && (i < breaklen)) {
	    ml = MIN(len,breaklen) ;
	    rs = filebuf_write(wfp,(lbuf+i),ml) ;
	    wlen += rs ;
	    i += rs ;
	}

	if ((rs >= 0) && (i < breaklen)) {
	    ml = (breaklen-i) ;
	    rs = filebuf_writeblanks(wfp,ml) ;
	    wlen += rs ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = filebuf_write(wfp,ds,dl) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    rs = filebuf_char(wfp,'\n') ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printend) */

static int printsub(filebuf *wfp,cchar *ds,cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		di = 0 ;
	int		wlen = 0 ;
	for (int i = 0 ; (i < llen) && lbuf[i] ; i += 1) {
	    int		ch = mkchar(lbuf[i]) ;
	    if ((! CHAR_ISWHITE(ch)) && (ds[di] != '\0')) {
	        if (ds[di] != ' ') ch = ds[di] ;
	        di += 1 ;
	    }
	    rs = filebuf_char(wfp,ch) ;
	    wlen += rs ;
	} /* end for */
	if (rs >= 0) {
	    rs = filebuf_char(wfp,'\n') ;
	    wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printsub) */

static int filebuf_char(filebuf *wfp,int ch) noex {
	char		wbuf[2] ;
	wbuf[0] = ch ;
	wbuf[1] = '\0' ;
	return filebuf_write(wfp,wbuf,1) ;
}
/* end subroutine (filebuf_char) */


