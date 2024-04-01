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
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<varnames.hh>
#include	<bufsizevar.hh>
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
#define	DIRCACHE_PATNAME	".dircacheXXXXXX"


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct dirhelp {
	cchar		*pr ;
	cchar		*ndname ;
	char		*nbuf = nullptr ;
	int		ttl ;
	int		of ;
	mode_t		om ;
	dirhelp(cc *p,cc *n,int f,mode_t m,int t) noex : pr(p), ndname(n) {
	    of = f ;
	    om = m ;
	    ttl = t ;
	} ; /* end ctor */
	operator int () noex ;
	int start() noex ;
	int finish() noex ;
	int proc() noex ;
    } ; /* end struct (dirhelp) */
}


/* forward references */

static int procdname(cchar *,int) noex ;
static int procdnamer(cchar *) noex ;
static int procdiffers(vecpstr *,cchar *) noex ;
static int procdiffer(vecpstr *,cchar *) noex ;
static int procdircache(vecpstr *,cchar *) noex ;
static int procdircacher(vecpstr *,cchar *) noex ;


/* local variables */

static bufsizevar		maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int pcsopendircache(cchar *pr,cchar *ndname,int of,mode_t om,int ttl) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	of &= O_ACCMODE ;
	of |= O_RDONLY ;
	if (pr && ndname) {
	    rs = SR_INVALID ;
	    if (pr[0] && ndname[0]) {
		if ((rs = maxpathlen) >= 0) {
		    dirhelp	ho(pr,ndname,of,om,ttl) ;
		    rs = ho ;
		    fd = rs ;
		} /* end if (maxpathlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (pcsopendircache) */


/* local subroutines */

dirhelp::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = start()) >= 0) {
	    {
		rs = proc() ;
		fd = rs ;
	    }
	    rs1 = finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (start-finish) */
	if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	return (rs >= 0) ? fd : rs ;
}
/* end method (dirhelp::operator) */

int dirhelp::start() noex {
	int		rs = SR_OK ;
	if (ndname[0] != '/') {
	    if ((rs = malloc_mp(&nbuf)) >= 0) {
	        rs = mkpath(nbuf,pr,ndname) ;
	        ndname = nbuf ;
		if (rs < 0) {
		    uc_free(nbuf) ;
		    nbuf = nullptr ;
		}
	    } /* end if (memory-allocation) */
	}
	return rs ;
}
/* end method (dirhelp::finish) */

int dirhelp::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (nbuf) {
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    nbuf = nullptr ;
	}
	return rs ;
}
/* end method (dirhelp::start) */

int dirhelp::proc() noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = procdname(ndname,ttl)) >= 0) {
	        int	fsz = 1 ;
	        cchar	*dc = DIRCACHE_CFNAME ;
	        char	*fbuf{} ;
	        fsz += (strlen(ndname) + 1) ;
	        fsz += 1 ;
	        fsz += (strlen(dc) + 1) ;
	        if ((rs = uc_malloc(fsz,&fbuf)) >= 0) {
		    if ((rs = mkpath2(fbuf,ndname,dc)) >= 0) {
	    	        if ((rs = u_open(fbuf,of,om)) >= 0) {
	    	            fd = rs ;
		            if ((rs >= 0) && (of & O_CLOEXEC)) {
			        rs = uc_closeonexec(fd,true) ;
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
	return (rs >= 0) ? fd : rs ;
}
/* end method (dirhelp::proc) */

static int procdname(cchar *newsdname,int ttl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*dbuf{} ;
	if ((rs = malloc_mp(&dbuf)) >= 0) {
	    cchar	*dc = DIRCACHE_CFNAME ;
	    if ((rs = mkpath2(dbuf,newsdname,dc)) >= 0) {
	        USTAT	sb ;
	        if ((rs = u_stat(dbuf,&sb)) >= 0) {
		    if (ttl >= 0) {
		        time_t	dt = time(NULL) ;
		        if ((dt-sb.st_mtime) >= ttl) rs = SR_STALE ;
		    }
	        }
	        if ((rs == SR_NOENT) || (rs == SR_STALE)) {
		    rs = procdnamer(newsdname) ;
		    c = rs ;
	        } /* end (not-found or stale) */
	    } /* end if (mkpath) */
	    rs1 = uc_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procdname) */

static int procdnamer(cchar *newsdname) noex {
	vecpstr		dirs ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
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
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procdnamer) */

static int procdiffer(vecpstr *dlp,cchar *newsdname) noex {
	cint		ml = strlen(DIRCACHE_MAGICSTR) ;
	int		rs ;
	int		rs1 ;
	int		f = true ;
	if ((rs = vecpstr_getsize(dlp)) >= 0) {
	    cint	dsize = rs ;
	    cchar	*dc = DIRCACHE_CFNAME ;
	    char	*dcfname{} ;
	    if ((rs = malloc_mp(&dcfname)) >= 0) {
	        if ((rs = mkpath(dcfname,newsdname,dc)) >= 0) {
	            USTAT	sb ;
	            if ((rs = uc_stat(dcfname,&sb)) >= 0) {
	                cint	fsize = int(sb.st_size & INT_MAX) ;
	                if (dsize == (fsize-ml-1)) {
			    rs = procdiffers(dlp,dcfname) ;
			    if (rs == 0) f = false ;
	                } /* end if (sizes were the same) */
	            } else if (isNotAccess(rs)) {
	                rs = SR_OK ;
		    }
	        } /* end if (mkpath) */
		rs1 = uc_free(dcfname) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (vecpstr-getsize) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (procdiffer) */

static int procdiffers(vecpstr *dlp,cchar *dcfname) noex {
	int		rs ;
	int		rs1 ;
	bool		fmis = false ;
	char		*dbuf{} ;
	if ((rs = malloc_mp(&dbuf)) >= 0) {
	    bfile	cfile, *cfp = &cfile ;
	    cint	dlen = rs ;
	    if ((rs = bopen(cfp,dcfname,"r",0666)) >= 0) {
                int         line = 0 ;
                cchar       *dp ;
                while ((rs = breadln(cfp,dbuf,dlen)) > 0) {
                    int     dl = rs ;
                    if (line > 0) {
                        if (dbuf[dl-1] == '\n') dl -= 1 ;
                        if (vecpstr_get(dlp,(line-1),&dp) >= 0) {
                            fmis = (strwcmp(dp,dbuf,dl) != 0) ;
                        } else {
                            fmis = true ;
                        }
                    }
                    line += 1 ;
                    if (fmis) break ;
                } /* end while */
                rs1 = bclose(cfp) ;
                if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	    rs1 = uc_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fmis : rs ;
}
/* end subroutine (procdiffers) */

static int procdircache(vecpstr *dlp,cchar *newsdname) noex {
	int		rs ;
	int		c = 0 ;
	char		dcfname[MAXPATHLEN + 1] ;
	if ((rs = mkpath2(dcfname,newsdname,DIRCACHE_CFNAME)) >= 0) {
	    cchar	*patname = DIRCACHE_PATNAME ;
	    char	tpat[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(tpat,newsdname,patname)) >= 0) {
		char	tbuf[MAXPATHLEN + 1] ;
	        if ((rs = mktmpfile(tbuf,tpat,0664)) >= 0) {
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
	    cchar	*magicstr = DIRCACHE_MAGICSTR ;
	    if ((rs = bprintln(dcfp,magicstr,-1)) >= 0) {
	        int	dl ;
	        cchar	*dp ;
	        for (int i = 0 ; vecpstr_get(dlp,i,&dp) >= 0 ; i += 1) {
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


