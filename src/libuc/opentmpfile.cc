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
#include	<sys/types.h>
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
#include	<ucgetpid.h>
#include	<sigblocker.h>
#include	<sockaddress.h>
#include	<mkpathx.h>
#include	<mkx.h>
#include	<cthex.h>
#include	<localmisc.h>


/* local defines */

#define	MAXLOOP		1000

#define	OTM_DGRAM	(1<<0)		/* open-type-mask DGRAM */
#define	OTM_STREAM	(1<<1)		/* open-type-mask STREAM */

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#undef	RANDBUFLEN
#define	RANDBUFLEN	(sizeof(ulong)*2)


/* local namespæces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward reference */

static int	opentmpx(cchar *,int,mode_t,int,char *) noex ;
static int	opentmpxer(cchar *,int,mode_t,int,char *) noex ;
static int	randload(ulong *) noex ;
static int	mktmpname(char *,ulong,cchar *) noex ;


/* local variables */


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
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	cchar		*platename = "otXXXXXXXXXXXX" ;
	char		inname[MAXPATHLEN + 1] ;

	of &= (~ O_ACCMODE) ;
	of |= O_RDWR ;

	om |= 0600 ;
	if (dname == nullptr) dname = getenv(VARTMPDNAME) ;
	if ((dname == nullptr) || (dname[0] == '\0')) dname = TMPDNAME ;

	if ((rs = mkpath2(inname,dname,platename)) >= 0) {
	    cint	olen = MAXPATHLEN ;
	    char	*obuf ;
	    if ((rs = uc_malloc((olen+1),&obuf)) >= 0) {
	        sigblocker	blocker ;
	        if ((rs = sigblocker_start(&blocker,nullptr)) >= 0) {
		    cint		otm = OTM_STREAM ;
	            if ((rs = opentmpx(inname,of,om,otm,obuf)) >= 0) {
		        fd = rs ;
	                if (obuf[0] != '\0') uc_unlink(obuf) ;
	            }
	            rs1 = sigblocker_finish(&blocker) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblock) */
		rs1 = uc_free(obuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a) */
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (mkpath) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opentmp) */


/* local subroutines */

static int opentmpx(cchar *inname,int of,mode_t om,int opt,char *obuf) noex {
	cint		plen = MAXPATHLEN ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	char		*pbuf ;
	if ((rs = uc_malloc((plen+1),&pbuf)) >= 0) {
	    if ((rs = mkexpandpath(pbuf,inname,-1)) > 0) {
		rs = opentmpxer(pbuf,of,om,opt,obuf) ;
		fd = rs ;
	    } else if (rs == 0) {
		rs = opentmpxer(inname,of,om,opt,obuf) ;
		fd = rs ;
	    }
	    rs1 = uc_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opentmpx) */

static int opentmpxer(cchar *inname,int of,mode_t om,int opt,char *obuf) noex {
	ulong		rv ;
	cmode		operm = (om & S_IAMB) ;
	int		rs = SR_OK ;
	int		stype = 0 ;
	int		loop = 0 ;
	int		fd = -1 ;
	int		f_exit = FALSE ;
	int		f_abuf = FALSE ;
	int		f ;

	if (inname == nullptr) return SR_FAULT ;
	if (obuf == nullptr) return SR_FAULT ;

	if (inname[0] == '\0') return SR_INVALID ;

	obuf[0] = '\0' ;

	if (S_ISSOCK(om)) {
	    if (opt & OTM_DGRAM) {
		stype = SOCK_DGRAM ;
	    } else if (opt & OTM_STREAM) {
		stype = SOCK_STREAM ;
	    } else
		rs = SR_INVALID ;
	}

	if ((rs >= 0) && (obuf == nullptr)) {
	    cint		olen = MAXPATHLEN ;
	    if ((rs = uc_malloc((olen+1),&obuf)) >= 0) {
		obuf[0] = '\0' ;
		f_abuf = TRUE ;
	    }
	}

	if (rs >= 0) rs = randload(&rv) ;

/* loop trying to create a file */

	while ((rs >= 0) && (loop < MAXLOOP) && (! f_exit)) {

	    f_exit = TRUE ;

/* put the file name together */

	    rv += loop ;
	    rs = mktmpname(obuf,rv,inname) ;
	    if (rs < 0) break ;

/* go ahead and make the thing */

	    if (S_ISDIR(om)) {

	        rs = u_mkdir(obuf,operm) ;
		if (rs < 0) {
	            if ((rs == SR_EXIST) || (rs == SR_INTR))
	                f_exit = FALSE ;
	        } 

	        if (rs >= 0) {
	            rs = u_open(obuf,O_RDONLY,operm) ;
		    fd = rs ;
	        }

	    } else if (S_ISREG(om) || ((om & (~ S_IAMB)) == 0)) {
	        int	ooflags = (of | O_CREAT | O_EXCL) ;

		f = ((ooflags & O_WRONLY) || (ooflags & O_RDWR)) ;
		if (! f)
		    ooflags |= O_WRONLY ;

	        rs = u_open(obuf,ooflags,operm) ;
	        fd = rs ;
		if (rs < 0) {
	            if ((rs == SR_EXIST) || (rs == SR_INTR)) f_exit = FALSE ;
	        }

	    } else if (S_ISFIFO(om)) {

	        if ((rs = u_mknod(obuf,operm,0)) >= 0) {
	            rs = u_open(obuf,of,operm) ;
	            fd = rs ;
		} else {
	            if ((rs == SR_EXIST) || (rs == SR_INTR)) {
			f_exit = FALSE ;
		    }
	        }

	    } else if (S_ISSOCK(om)) {
	        cint		pf = PF_UNIX ;
	        if ((rs = u_socket(pf,stype,0)) >= 0) {
	            SOCKADDRESS	sa ;
		    cint	af = AF_UNIX ;
	            fd = rs ;
	            if ((rs = sockaddress_start(&sa,af,obuf,0,0)) >= 0) {
		        struct sockaddr	*sap = (struct sockaddr *) &sa ;
		        cint		sal = rs ;
	                if ((rs = u_bind(fd,sap,sal)) >= 0) {
			    rs = u_chmod(obuf,operm) ;
			    if (rs < 0) {
				u_unlink(obuf) ;
				obuf[0] = '\0' ;
			    }
			} /* end if (bind) */
	                sockaddress_finish(&sa) ;
		    } /* end if (sockaddress) */
		    if (rs < 0) {
			u_close(fd) ;
			fd = -1 ;
		    }
	        } /* end if (socket) */
		if (rs < 0) {
	            f_exit = FALSE ;
		}
	    } else {
		rs = SR_INVALID ;
	        f_exit = TRUE ;
	    } /* end if */

	    loop += 1 ;
	} /* end while */

	if ((rs >= 0) && (loop >= MAXLOOP)) {
	    rs = SR_ADDRINUSE ;
	}

	if (f_abuf) {
	    if (obuf[0] != '\0') {
		u_unlink(obuf) ;
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
	    const pid_t	pid = uc_getpid() ;
	    const pid_t	sid = getsid(0) ;
	    const uid_t	uid = getuid() ;
	    ulong	rv = 0 ;
	    ulong	v ;
	    rs = SR_OK ;
#ifdef	COMMENT
	v = gethostid() ;
	rv ^= (v << 32) ;
#endif

	v = sid ;
	rv += (v << 48) ;

	v = pid ;
	rv += (v << 32) ;

	v = uid ;
	rv += (v << 16) ;

	{
	    TIMEVAL	tod ;
	    uc_gettimeofday(&tod,nullptr) ; /* cannot fail?! */
	        v = tod.tv_sec ;
	        rv += (v << 32) ;
	        rv += (v << 12) ;
	        rv += tod.tv_usec ;
	} /* end block */

#if	SYSHAS_HRTIME
	{
	    hrtime_t	ht = gethrtime() ;
	    rv += ht ;
	}
#endif /* SYSHAS_GRTIME */

	*rvp = rv ;

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
	    if ((rs = mkpath1(obuf,inname)) >= 0) {
	        int	j = rs ;
	        int	i = randlen ;
	        while (j > 0) {
		    j -= 1 ;
	            if (i > 0) {
		        if (obuf[j] == 'X') obuf[j] = randbuf[--i] ;
	            }
	        } /* end while */
	    } /* end if (mkpath) */
	} /* end if (cthexul) */
	return rs ;
}
/* end subroutine (mktmpname) */


