/* opentmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make and open a temporary file */
/* version %I% last-modified %G% */

#define	CF_SPLITFNAME	0		/* used |splitfname(3uc)| */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	opentmpfile
	opentmpusd
	opentmpuss
	opentmp

	Description:
	Make and open a temporary file.

	Synopsis:
	int opentmpfile(cchar *inname,int of,mode_t om,char *obuf) noex
	int opentmpusd(cchar *inname,int of,mode_t om,char *rbuf) noex
	int opentmpuss(cchar *inname,int of,mode_t om,char *rbuf) noex
	int opentmp(cchar *dname,int of,mode_t om) noex

	Arguments:
	inname		input directory template
	of		open flags
	om		file type and creation mode
	obuf		output buffer to hold resultant file name

	Returns:
	>=0		file descriptor
	<0		error code (system-return)

	Notes:
	Not to lecture, but these routines are a good example of
	several mechanisms to achieve multithread safety.  These
	routines are also async-signal-safe, although few really
	care about that now-a-days.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |mode_t| */
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<sys/time.h>		/* for |TIMEVAL| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ucgetpid.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<sigblocker.h>
#include	<sockaddress.h>
#include	<pathadd.h>
#include	<mkpathx.h>
#include	<mkx.h>
#include	<sfx.h>
#include	<cthex.h>
#include	<strlibval.hh>
#include	<splitfname.h>
#include	<localmisc.h>

#include	"opentmp.h"


import uconstants ;

/* local defines */

#define	MAXLOOP		1000

#define	RANDBUFLEN	(szof(ulong) * 2)

#ifndef	CF_SPLITFNAME
#define	CF_SPLITFNAME	0		/* used |splitfname(3uc)| */
#endif


/* local namespæces */


/* local typedefs */

typedef SOCKADDR *	sockaddrp ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct openmgr ;
    typedef int (openmgr::*openmgr_m)() noex ;
    struct openmgr {
	openmgr_m	m ;
	cchar		*dirp ;
	cchar		*basep ;
	char		*obuf ;
	ulong		rv ;
	int		dirl ;
	int		basel ;
	int		of ;
	int		stype ;
	int		pl = 0 ;	/* partially filled 'obuf' */
	int		fd = -1 ;
	mode_t		am ;
	mode_t		om ;
	bool		falloc = false ;
	openmgr(int f,mode_t aom,char *o) noex : of(f), om(aom) { 
	    obuf = o ;
	    am = (om & S_IAMB) ;	/* isolate access-mode */
	} ;
	int operator () (cchar *,int) noex ;
	int obufbegin() noex ;
	int obufend() noex ;
	int typeinit(int) noex ;
	int setft() noex ;
	int split(cchar *) noex ;
	int dirload() noex ;
	int loop() noex ;
	int mkofname() noex ;
	int ofifo() noex ;
	int odir() noex ;
	int oreg() noex ;
	int osock() noex ;
    } ; /* end struct (openmgr) */
}

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ;
}


/* forward reference */

static int	opentmpx(cchar *,int,mode_t,int,char *) noex ;
static int	opentmpxer(cchar *,int,mode_t,int,char *) noex ;
static int	randload(ulong *) noex ;
static int	substr(char *,int,ulong) noex ;
static int	mkvarsx() noex ;


/* local variables */

static strlibval	val_tmpdir(strlibval_tmpdir) ;

static vars		var ;

constexpr cchar		platename[] = "otXXXXXXXXXXXX" ;

constexpr bool		f_splitfname = CF_SPLITFNAME ;


/* exported variables */


/* exported subroutines */

int opentmpfile(cchar *inname,int of,mode_t om,char *rbuf) noex {
	cint		otm = OTM_STREAM ;
	return opentmpx(inname,of,om,otm,rbuf) ;
}
/* end subroutine (opentmpfile) */

int opentmpusd(cchar *inname,int of,mode_t om,char *rbuf) noex {
	cint		otm = OTM_DGRAM ;
	om |= (S_IFSOCK | 0600) ;
	return opentmpx(inname,of,om,otm,rbuf) ;
}
/* end subroutine (opentmpusd) */

int opentmpuss(cchar *inname,int of,mode_t om,char *rbuf) noex {
	cint		otm = OTM_STREAM ;
	om |= (S_IFSOCK | 0600) ;
	return opentmpx(inname,of,om,otm,rbuf) ;
}
/* end subroutine (opentmpuss) */

int opentmp(cchar *dname,int of,mode_t om) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	int		fd = -1 ;
	of &= (~ O_ACCMODE) ;
	of |= O_RDWR ;
	om |= 0600 ;
	if (dname == nullptr) dname = val_tmpdir ;
	if (dname && dname[0] && (of >= 0)) {
	    if ((rs = mkvarsx()) >= 0) {
		cint	maxpath = rs ;
		cint	sz = (2 * (var.maxpathlen + 1)) ;
		if (char *a{} ; (rs = uc_malloc(sz,&a)) >= 0) {
	            char	*ibuf = (a) ;
	            char	*obuf = (a + (maxpath + 1)) ;
	            if ((rs = mkpath(ibuf,dname,platename)) >= 0) {
	                if (sigblocker b ; (rs = b.start) >= 0) {
		            cint	otm = OTM_STREAM ;
	                    if ((rs = opentmpx(ibuf,of,om,otm,obuf)) >= 0) {
		                fd = rs ;
	                        if (obuf[0] != '\0') {
			            uc_unlink(obuf) ;
			            obuf[0] = '\0' ;
			        }
	                    } /* end if (opentempx) */
	                    rs1 = b.finish ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (sigblock) */
	            } /* end if (mkpath) */
	            rs1 = uc_free(a) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (mkvarsx) */
	    if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	} /* end if (valid) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opentmp) */


/* local subroutines */

static int opentmpx(cchar *inname,int of,mode_t om,int opt,char *obuf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (inname && obuf) {
	    rs = SR_INVALID ;
	    if (inname[0] && (of >= 0) && (opt >= 0)) {
	        if (char *pbuf ; (rs = malloc_mp(&pbuf)) >= 0) {
	            if ((rs = mkpathexp(pbuf,inname,-1)) > 0) {
		        rs = opentmpxer(pbuf,of,om,opt,obuf) ;
		        fd = rs ;
	            } else if (rs == 0) {
		        rs = opentmpxer(inname,of,om,opt,obuf) ;
		        fd = rs ;
	            }
	            rs1 = uc_free(pbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
		if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opentmpx) */

static int opentmpxer(cchar *inname,int of,mode_t om,int opt,char *obuf) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (inname && obuf) {
	    rs = SR_INVALID ;
	    obuf[0] = '\0' ;
	    if (inname[0] && (of >= 0) && (opt >= 0)) {
		openmgr		oo(of,om,obuf) ;
		rs = oo(inname,opt) ;
		fd = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opentmpxer) */

int openmgr::typeinit(int opt) noex {
	int		rs = SR_OK ;
	if (S_ISSOCK(om)) {
	    if (opt & OTM_DGRAM) {
		stype = SOCK_DGRAM ;
	    } else if (opt & OTM_STREAM) {
		stype = SOCK_STREAM ;
	    } else {
		rs = SR_INVALID ;
	    }
	}
	return rs ;
}
/* end method (openmgr::typeinit) */

int openmgr::setft() noex {
	int		rs = SR_OK ;
	cint		ft = ((om >> 12) & 0x0F) ;
	switch (ft) {
	case filetype_fifo:
	    m = &openmgr::ofifo ;
	    break ;
	case filetype_dir:
	    m = &openmgr::odir ;
	    break ;
	case filetype_reg:
	    m = &openmgr::oreg ;
	    break ;
	case filetype_sock:
	    m = &openmgr::osock ;
	    break ;
	default:
	    rs = SR_NOSYS ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (openmgr::setft) */

int openmgr::split(cchar *inname) noex {
	int		rs = SR_OK ;
	if_constexpr (f_splitfname) {
	    if ((dirl = sfdirname(inname,-1,&dirp)) >= 0) {
	        rs = SR_ISDIR ;
	        if ((basel = sfbasename(inname,-1,&basep)) > 0) {
		    rs = SR_OK ;
	        } /* end if (sfbasename) */
	    } /* end if (sfdirname) */
	} else {
	    if (splitfname so ; (rs = so(inname)) >= 0) {
		dirp = so.dp ;
		dirl = so.dl ;
		basep = so.bp ;
		basel = so.bl ;
	    }
	} /* end if_constexpr (f_splitfname) */
	return rs ;
}
/* end method (openmgr::split) */

int openmgr::dirload() noex {
	int		rs = SR_OK ;
	if (dirl > 0) {
	    if ((rs = mkpathw(obuf,dirp,dirl)) >= 0) {
	        pl = rs ;
	    }
	}
	return rs ;
}
/* end method (openmgr::dirload) */

int openmgr::obufbegin() noex {
	int		rs = SR_OK ;
	if (obuf == nullptr) {
	    if ((rs = malloc_mp(&obuf)) >= 0) {
		obuf[0] = '\0' ;
		falloc = true ;
	    }
	}
	return rs ;
}
/* end method (openmgr::obufbegin) */

int openmgr::obufend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (falloc && obuf) {
	    obuf[0] = '\0' ;
	    rs1 = uc_free(obuf) ;
	    if (rs >= 0) rs = rs1 ;
	    obuf = nullptr ;
	    falloc = false ;
	}
	return rs ;
}
/* end method (openmgr::obufend) */

int openmgr::operator () (cchar *inname,int opt) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = typeinit(opt)) >= 0) {
	    if ((rs = setft()) >= 0) {
	        if ((rs = split(inname)) >= 0) {
		    if ((rs = dirload()) >= 0) {
	                if ((rs = randload(&rv)) >= 0) {
		            if ((rs = obufbegin()) >= 0) {
		 	        {
		                    rs = loop() ;
		                }
		                rs1 = obufend() ;
		                if (rs >= 0) rs = rs1 ;
		            } /* end if (obuf) */
	                } /* end if (randload) */
		    } /* end if (dirload) */
	        } /* end if (split) */
	    } /* end if (setft) */
	    if ((rs < 0) && (fd >= 0)) {
		uc_close(fd) ;
		fd = -1 ;
	    }
	} /* end if (typeinit) */
	return (rs >= 0) ? fd : rs ;
}
/* end method (openmgr::operator) */

int openmgr::mkofname() noex {
	int		rs ;
	if ((rs = pathadd(obuf,pl,basep,basel)) >= 0) {
	    cint	bl = (rs - pl) ;
	    char	*bp = (obuf + pl) ;
	    rs = substr(bp,bl,rv) ;
	}
	return rs ;
}
/* end method (openmgr::mkofname) */

int openmgr::loop() noex {
	int		rs = SR_OK ;
	for (int c = 0 ; (rs >= 0) && (c < MAXLOOP) ; c += 1) {
	    if ((rs = mkofname()) >= 0) {
		rv += 1 ;
		rs = (this->*m)() ;
	    } /* end if (mkofname) */
	    if (fd >= 0) break ;
	} /* end for */
	if ((rs >= 0) && (fd < 0)) {
	    rs = SR_ADDRINUSE ;
	}
	return rs ;
}
/* end method (openmgr::loop) */

int openmgr::ofifo() noex {
	int		rs ;
	if ((rs = uc_mkfifo(obuf,am)) >= 0) {
	    rs = uc_open(obuf,of,am) ;
	    fd = rs ;
	} else if (rs == SR_EXIST) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end method (openmgr::ofifo) */

int openmgr::odir() noex {
	int		rs ;
	if ((rs = uc_mkdir(obuf,am)) >= 0) {
	    cint	nof = O_RDONLY ;
	    if ((rs = uc_open(obuf,nof,am)) >= 0) {
	        fd = rs ;
	    }
	} else if (rs == SR_EXIST) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end method (openmgr::odir) */

int openmgr::oreg() noex {
	int		rs = SR_OK ;
	int		nof = (of | O_CREAT | O_EXCL) ;
	cbool		f = ((nof & O_WRONLY) || (nof & O_RDWR)) ;
	if (! f) {
	    nof |= O_WRONLY ;
	}
	if ((rs = uc_open(obuf,nof,am)) >= 0) {
	    fd = rs ;
	} else if (rs == SR_EXIST) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end method (openmgr::oreg) */

int openmgr::osock() noex {
	cint		pf = PF_UNIX ;
	int		rs ;
	int		rs1 ;
        if ((rs = uc_socket(pf,stype,0)) >= 0) {
            sockaddress sa ;
            cint        af = AF_UNIX ;
            fd = rs ;
            if ((rs = sockaddress_start(&sa,af,obuf,0,0)) >= 0) {
                SOCKADDR        *sap = sockaddrp(&sa) ;
                cint            sal = rs ;
                if ((rs = uc_bind(fd,sap,sal)) >= 0) {
		    cmode	nom = (om & (~ S_IFMT)) ;
                    rs = uc_chmod(obuf,nom) ;
                    if (rs < 0) {
                        uc_unlink(obuf) ;
                        obuf[0] = '\0' ;
                    }
                } /* end if (bind) */
                rs1 = sockaddress_finish(&sa) ;
		if (rs >= 0) rs = rs1 ;
            } /* end if (sockaddress) */
            if (rs < 0) {
                uc_close(fd) ;
                fd = -1 ;
            }
        } /* end if */
	return rs ;
}
/* end method (openmgr::osock) */

static int randload(ulong *rvp) noex {
	int		rs = SR_FAULT ;
	if (rvp) {
	    cnullptr	np{} ;
	    cuint	sid = getsid(0) ;
	    cuint	uid = getuid() ;
	    if ((rs = ucpid) >= 0) {
		cuint	pid = rs ;
	        ulong	rv = 0 ;
	        ulong	v = sid ;	/* mix in |sid| */
	        rv += (v << 48) ;
	        v = pid ;		/* mix in |pid| */
	        rv += (v << 32) ;
	        v = uid ;		/* mix in |uid| */
	        rv += (v << 16) ;
	        if (TIMEVAL tod ; (rs = uc_gettimeofday(&tod,np)) >= 0) {
	            v = tod.tv_sec ;
	            rv += (v << 32) ;
	            rv += (v << 12) ;
	            rv += tod.tv_usec ;
	        } /* end block */
	        *rvp = rv ;
	    } /* end if (uc_getpid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (randload) */

/* load buffer w/ random HEX digits (16 bytes) from random variable (8 bytes) */
static int substr(char *dp,int dl,ulong rv) noex {
	cint		randlen = RANDBUFLEN ;
	int		rs ;
	char		randbuf[RANDBUFLEN+1] ;
	if ((rs = cthex(randbuf,randlen,rv)) >= 0) {
	    int		j = rs ;
	    for (int i = 0 ; i < dl ; i += 1) {
		if (dp[i] == 'X') {
		    if (j > 0) {
		        dp[i] = randbuf[--j] ;
		    }
	        } /* end if */
	    } /* end for */
	} /* end if (cthex) */
	return rs ;
}
/* end subroutine (substr) */

static int mkvarsx() noex {
	static cint	rsv = var ;
	return rsv ;
}
/* end subroutine (mkvarsx) */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	}
	return rs ;
}
/* end method (vars::operator) */


