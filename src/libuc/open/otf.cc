/* opentmpfile SUPPORT */
/* lang=C++20 */

/* make and open a temporary file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	opentmpfile

	Description:
	Make and open a temporary file.

	Synopsis:
	int opentmpfile(cchar *inname,int of,mode_t om,char *obuf) noex

	Arguments:
	inname		input directory template
	of		open flags
	om		file type and creation mode
	obuf		output buffer to hold resultant file name

	Returns:
	>=0		file descriptor
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<sys/time.h>		/* for |gethrtime(3c)| */
#include	<unistd.h>		/* for |gethostid(3c)| */
#include	<fcntl.h>
#include	<netdb.h>
#include	<cstdlib>
#include	<ctime>
#include	<usystem.h>
#include	<ucvariables.hh>
#include	<ucgetpid.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<sigblocker.h>
#include	<sockaddress.h>
#include	<mkpathx.h>
#include	<mkx.h>
#include	<cthex.h>
#include	<strlibval.hh>
#include	<localmisc.h>

#include	"opentmp.h"


/* local defines */

#define	MAXLOOP		1000

#define	RANDBUFLEN	int(sizeof(ulong)*2)


/* local namespæces */


/* local typedefs */

typedef SOCKADDR *	sockaddrp ;


/* external subroutines */


/* external variables */


/* local structures */

struct vars {
	int		maxpathlen ;
} ;


/* forward reference */

static int	opentmpx(cchar *,int,mode_t,int,char *) noex ;
static int	opentmpxer(cchar *,int,mode_t,int,char *) noex ;
static int	randload(ulong *) noex ;
static int	mktmpname(char *,ulong,cchar *) noex ;
static int	mkvarsx() noex ;
static int	mkvars() noex ;


/* local variables */

static strlibval	val_tmpdir(strlibval_tmpdir) ;

constexpr static cchar	platename[] = "otXXXXXXXXXXXX" ;

static vars		var ;


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
		char	*a{} ;
	        if ((rs = uc_malloc(sz,&a)) >= 0) {
	            char	*ibuf = (a) ;
	            char	*obuf = (a + (maxpath + 1)) ;
	            if ((rs = mkpath(ibuf,dname,platename)) >= 0) {
	                sigblocker	b ;
	                if ((rs = b.start) >= 0) {
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
	    } /* end if (mkvarx) */
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
	        char	*pbuf{} ;
	        if ((rs = malloc_mp(&pbuf)) >= 0) {
	            if ((rs = mkexpandpath(pbuf,inname,-1)) > 0) {
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
	    /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opentmpx) */

namespace {
    struct openmgr {
	cchar		*dirp
	cchar		*basep ;
	char		*obuf ;
	ulong		rv ;
	int		dirl ;
	int		basel ;
	int		of ;
	int		stype ;
	mode_t		operm ;
	mode_t		om ;
	openmgr(int f,mode_t m,char *o) noex : of(f), om(m) { 
	    obuf = o ;
	} ;
	operator () (cchar *) noex ;
	int typeinit(int) noex ;
	int split(cchar *) noex ;
	int loop() noex ;
    } ; /* end struct (openmgr) */
}

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
	    /* end if (valid) */
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

int openmgr::split(cchar *inname) noex {
	int		rs = SR_OK ;
	if ((dirl = sfdirname(inname,-1,&dirp)) >= 0) {
	    rs = SR_ISDIR ;
	    if ((basel = sfbasename(inname,-1,&basep)) > 0) {
		rs = SR_OK ;
	    } /* end if (sfbasename) */
	} /* end if (sfdirname) */
	return rs ;
}
/* end method (openmgr::split) */

int openmgr::operator () (cchar *inname,int opt) noex {
	int		rs ;
	int		fd = -1 ;
	if ((rs = typeinit(opt)) >= 0) {
	    if ((rs = split(inname)) >= 0) {
	        if ((rs = randload(&rv)) >= 0) {
		    rs = loop() ;
		    fd = rs ;
	        } /* end if (randload) */
	    } /* end if (split) */
	} /* end if (typeinit) */
	return (rs >= 0) ? fd : rs ;
}
/* end method (openmgr::operator) */


	ulong		rv ;
	cmode		operm = (om & S_IAMB) ;
	int		rs = SR_OK ;
	int		stype = 0 ;
	int		loop = 0 ;
	int		fd = -1 ;
	int		f_exit = false ;
	int		f_abuf = false ;
	int		f ;



	if ((rs >= 0) && (obuf == nullptr)) {
	    cint		olen = MAXPATHLEN ;
	    if ((rs = uc_malloc((olen+1),&obuf)) >= 0) {
		obuf[0] = '\0' ;
		f_abuf = true ;
	    }
	}

	if (rs >= 0) rs = randload(&rv) ;

/* loop trying to create a file */

	while ((rs >= 0) && (loop < MAXLOOP) && (! f_exit)) {

	    f_exit = true ;

/* put the file name together */

	    rv += loop ;
	    rs = mktmpname(obuf,rv,inname) ;
	    if (rs < 0) break ;

/* go ahead and make the thing */

	    if (S_ISDIR(om)) {

	        rs = uc_mkdir(obuf,operm) ;
		if (rs < 0) {
	            if ((rs == SR_EXIST) || (rs == SR_INTR))
	                f_exit = false ;
	        } 

	        if (rs >= 0) {
	            rs = uc_open(obuf,O_RDONLY,operm) ;
		    fd = rs ;
	        }

	    } else if (S_ISREG(om) || ((om & (~ S_IAMB)) == 0)) {
	        int	ooflags = (of | O_CREAT | O_EXCL) ;

		f = ((ooflags & O_WRONLY) || (ooflags & O_RDWR)) ;
		if (! f) {
		    ooflags |= O_WRONLY ;
		}

	        rs = uc_open(obuf,ooflags,operm) ;
	        fd = rs ;
		if (rs < 0) {
	            if ((rs == SR_EXIST) || (rs == SR_INTR)) f_exit = false ;
	        }

	    } else if (S_ISFIFO(om)) {

	        if ((rs = uc_mknod(obuf,operm,0)) >= 0) {
	            rs = uc_open(obuf,of,operm) ;
	            fd = rs ;
		} else {
	            if ((rs == SR_EXIST) || (rs == SR_INTR)) {
			f_exit = false ;
		    }
	        }

	    } else if (S_ISSOCK(om)) {
	        cint		pf = PF_UNIX ;
	        if ((rs = uc_socket(pf,stype,0)) >= 0) {
	            sockaddress	sa ;
		    cint	af = AF_UNIX ;
	            fd = rs ;
	            if ((rs = sockaddress_start(&sa,af,obuf,0,0)) >= 0) {
		        SOCKADDR	*sap = sockaddrp(&sa) ;
		        cint		sal = rs ;
	                if ((rs = uc_bind(fd,sap,sal)) >= 0) {
			    rs = uc_chmod(obuf,operm) ;
			    if (rs < 0) {
				uc_unlink(obuf) ;
				obuf[0] = '\0' ;
			    }
			} /* end if (bind) */
	                sockaddress_finish(&sa) ;
		    } /* end if (sockaddress) */
		    if (rs < 0) {
			uc_close(fd) ;
			fd = -1 ;
		    }
	        } /* end if (socket) */
		if (rs < 0) {
	            f_exit = false ;
		}
	    } else {
		rs = SR_INVALID ;
	        f_exit = true ;
	    } /* end if */

	    loop += 1 ;
	} /* end while */

	if ((rs >= 0) && (loop >= MAXLOOP)) {
	    rs = SR_ADDRINUSE ;
	}

	if (f_abuf) {
	    if (obuf[0] != '\0') {
		uc_unlink(obuf) ;
	    }
	    uc_free(obuf) ;
	} else if (rs < 0) {
	    if (obuf != nullptr) {
		obuf[0] = '\0' ;
 	    }
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opentmpxer) */

static int randload(ulong *rvp) noex {
	int		rs = SR_FAULT ;
	if (rvp) {
	    cnullptr	np{} ;
	    cuint	sid = getsid(0) ;
	    cuint	uid = getuid() ;
	    if ((rs = uc_getpid()) >= 0) {
	        TIMEVAL	tod ;
		cuint	pid = rs ;
	        ulong	rv = 0 ;
	        ulong	v = sid ;
	        rv += (v << 48) ;
	        v = pid ;
	        rv += (v << 32) ;
	        v = uid ;
	        rv += (v << 16) ;
	        if ((rs = uc_gettimeofday(&tod,np)) >= 0) {
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
static int mktmpname(char *obuf,ulong rv,cchar *inname) noex {
	cint		randlen = RANDBUFLEN ;
	int		rs ;
	char		randbuf[RANDBUFLEN+1] ;
	if ((rs = cthex(randbuf,randlen,rv)) >= 0) {
	    if ((rs = mkpath(obuf,inname)) >= 0) {
	        int	j = rs ;
	        int	i = randlen ;
	        while (j > 0) {
		    j -= 1 ;
	            if (i > 0) {
		        if (obuf[j] == 'X') {
			    obuf[j] = randbuf[--i] ;
			}
	            }
	        } /* end while */
	    } /* end if (mkpath) */
	} /* end if (cthex) */
	return rs ;
}
/* end subroutine (mktmpname) */

static int mkvarsx() noex {
	static int	rsv = mkvars() ;
	return rsv ;
}
/* end subroutine (mkvarsx) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    var.maxpathlen = rs ;
	}
	return rs ;
}
/* end subroutine (mkvars) */


