/* pcsopendircache SUPPORT */
/* lang=C++20 */

/* open a channel (file-descriptor) to the directory-cache of a directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	We open a directory-cache that is associated with a directory.
	A file-descriptor is returned (and must eventually be closed)
	to the directory-cache.

	Synopsis:
	int pcsopendircache(cc *pr,cc *dname,int of,mode_t om,int ttl) noex

	Arguments:
	pr		program-root
	dname		directory for which we access its directory-cache
	of		open-flags
	om		open-mode
	ttl		time-out

	Returns:
	>=0		FD of dir-cache
	<0		error (system-return)

	Notes:
	- open flags:
		O_NOTTY
		O_EXCL		
		O_SYNC
		O_NDELAY
		O_NONBLOCK

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecpstr.h>
#include	<bfile.h>
#include	<fsdirtree.h>
#include	<storebuf.h>
#include	<mktmp.h>
#include	<mkpathx.h>
#include	<strwcmp.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"pcsopendircache.h"


/* local defines */

#define	DIRCACHE_MAGICSTR	"DIRCACHE"
#define	DIRCACHE_CFNAME		".dircache"

#ifndef	USERNAMELEN
#ifndef	LOGNAME_MAX
#define	USERNAMELEN	LOGNAME_MAX
#else
#define	USERNAMELEN	32
#endif
#endif

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	2048
#endif

#define	BUFLEN		100

#define	VTMPDNAME	"/var/tmp"


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int procdname(cchar *,int) noex ;
static int procdiffer(vecpstr *,cchar *) noex ;
static int procdircache(vecpstr *,cchar *) noex ;
static int procdircacher(vecpstr *,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsopendircache(cchar *pr,cchar *newsdname,int of,mode_t om,int ttl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		fd = -1 ;
	char		ndname[MAXPATHLEN+1] ;

	if (pr == NULL) return SR_FAULT ;
	if (newsdname == NULL) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;
	if (newsdname[0] == '\0') return SR_INVALID ;

	of &= O_ACCMODE ;
	of |= O_RDONLY ;

	if (newsdname[0] != '/') {
	     rs = mkpath2(ndname,pr,newsdname) ;
	     newsdname = ndname ;
	}

	if (rs >= 0) {
	    if ((rs = procdname(newsdname,ttl)) >= 0) {
	        int	fsz = 1 ;
	        cchar	*dc = DIRCACHE_CFNAME ;
	        char	*fbuf ;
	        fsz += (strlen(newsdname) + 1) ;
	        fsz += 1 ;
	        fsz += (strlen(dc) + 1) ;
	        if ((rs = uc_malloc(fsz,&fbuf)) >= 0) {
		    if ((rs = mkpath2(fbuf,newsdname,dc)) >= 0) {
	    	        if ((rs = u_open(fbuf,of,om)) >= 0) {
	    	            fd = rs ;
		            if ((rs >= 0) && (of & O_CLOEXEC)) {
			        rs = uc_closeonexec(fd,TRUE) ;
		            }
		            if (rs < 0) {
			        u_close(fd) ;
			    }
		        } /* end if (dircache-open) */
		    } /* end if (mkpath) */
		    rs1 = uc_free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (memory-allocation) */
		if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	    } /* end if (procdname) */
	} /* end if (ok) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (pcsopendircache) */


/* local subroutines */

static int procdname(cchar *newsdname,int ttl) noex {
	vecpstr		dirs ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*dc = DIRCACHE_CFNAME ;
	char		dcfname[MAXPATHLEN+1] ;

	if ((rs = mkpath2(dcfname,newsdname,dc)) >= 0) {
	    USTAT	sb ;
	    if ((rs = u_stat(dcfname,&sb)) >= 0) {
		if (ttl >= 0) {
		    time_t	dt = time(NULL) ;
		    if ((dt-sb.st_mtime) >= ttl) rs = SR_STALE ;
		}
	    }

	    if ((rs == SR_NOENT) || (rs == SR_STALE)) {
		if ((rs = vecpstr_start(&dirs,25,0,0)) >= 0) {
	    	    if ((rs = vecpstr_loaddirs(&dirs,newsdname)) >= 0) {
	        	c = rs ;
	        	if ((rs = procdiffer(&dirs,newsdname)) > 0) {
	            	    rs = procdircache(&dirs,newsdname) ;
	        	}
	    	    } /* end if (load-dirs) */
	    	    rs1 = vecpstr_finish(&dirs) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (vecpstr-dirs) */
	    } /* end (not-found or stale) */

	} /* end if (mkpath) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procdname) */

static int procdiffer(vecpstr *dlp,cchar *newsdname) noex {
	cint		ml = strlen(DIRCACHE_MAGICSTR) ;
	int		rs ;
	int		rs1 ;
	int		f = TRUE ;

	if ((rs = vecpstr_getsize(dlp)) >= 0) {
	    cint	dsize = rs ;
	    cchar	*dc = DIRCACHE_CFNAME ;
	    char	dcfname[MAXPATHLEN+1] ;
	    if ((rs = mkpath2(dcfname,newsdname,dc)) >= 0) {
	        USTAT	sb ;
	        if ((rs = uc_stat(dcfname,&sb)) >= 0) {
	            bfile	cfile, *cfp = &cfile ;
	            int	fsize = (sb.st_size & INT_MAX) ;
	            if (dsize == (fsize-ml-1)) {
	                if ((rs = bopen(cfp,dcfname,"r",0666)) >= 0) {
	                    cint	dlen = MAXPATHLEN ;
	                    int		line = 0 ;
	                    int		f_mis = FALSE ;
	                    cchar	*dp ;
	                    char	dbuf[MAXPATHLEN+1] ;
	                    while ((rs = breadln(cfp,dbuf,dlen)) > 0) {
	                        int	dl = rs ;
	                        if (line > 0) {
	                            if (dbuf[dl-1] == '\n') dl -= 1 ;
	                            if (vecpstr_get(dlp,(line-1),&dp) >= 0) {
	                                f_mis = (strwcmp(dp,dbuf,dl) != 0) ;
	                            } else {
	                                f_mis = TRUE ;
				    }
	                        }
	                        line += 1 ;
	                        if (f_mis) break ;
	                    } /* end while */
	                    if (! f_mis) f = FALSE ;
	                    rs1 = bclose(cfp) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (file-open) */
	            } /* end if (sizes were the same) */
	        } else if (isNotAccess(rs)) {
	            rs = SR_OK ;
		}
	    } /* end if (mkpath) */
	} /* end if (vecpstr-getsize) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (procdiffer) */

static int procdircache(vecpstr *dlp,cchar *newsdname) noex {
	int		rs ;
	int		c = 0 ;
	char		dcfname[MAXPATHLEN + 1] ;
	if ((rs = mkpath2(dcfname,newsdname,DIRCACHE_CFNAME)) >= 0) {
	    char	tpat[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(tpat,newsdname,".dircacheXXXXXX")) >= 0) {
		char	tbuf[MAXPATHLEN + 1] ;
	        if ((rs = mktmpfile(tbuf,0664,tpat)) >= 0) {
		     if ((rs = procdircacher(dlp,tbuf)) >= 0) {
	                rs = u_rename(tbuf,dcfname) ;
			if (rs < 0) {
	                    u_unlink(tbuf) ;
			}
	            } /* end if (renaming attempt) */
	        } /* end if (mktmpfile) */
	    } /* end if (mkpath) */
	} /* end if (mkpath) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procdircache) */

static int procdircacher(vecpstr *dlp,cchar *fn) noex {
	bfile		dcfile, *dcfp = &dcfile ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = bopen(dcfp,fn,"wct",0664)) >= 0) {
	    if ((rs = bprintln(dcfp,DIRCACHE_MAGICSTR,-1)) >= 0) {
	        int	i ;
	        int	dl ;
	        cchar	*dp ;
	        for (i = 0 ; vecpstr_get(dlp,i,&dp) >= 0 ; i += 1) {
	            if (dp != NULL) {
	                dl = strlen(dp) ;
	                c += 1 ;
	                rs = bprintln(dcfp,dp,dl) ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if */
	    rs1 = bclose(dcfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (opened replacement file) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procdircacher) */


