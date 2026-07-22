/* opentmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make and open a temporary file */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_SPLITFNAME	1		/* use |splitfname(3uc)| */

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

	File-Types:
	S_IFMT          0170000
	S_IFIFO         0010000
	S_IFCHR         0020000
	S_IFDIR         0040000
	S_IFBLK         0060000
	S_IFREG         0100000
	S_IFLNK         0120000
	S_IFSOCK        0140000
	S_IFWHT         0160000

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX |mode_t| */
#include	<sys/stat.h>		/* POSIX */
#include	<sys/socket.h>		/* POSIX */
#include	<sys/time.h>		/* for |TIMEVAL| */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC |uc_gettimeofday(3uc)| */
#include	<ucgetpid.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<ucgetx.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<aflag.hh>		/* LIBU */
#include	<filetypes.h>		/* LIBU */
#include	<sigblocker.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<sockaddress.h>		/* LIBUC */
#include	<pathadd.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<snflags.h>		/* LIBUC */
#include	<cthex.h>		/* LIBUC */
#include	<strlibval.hh>		/* LIBUC */
#include	<splitfname.h>		/* LIBUC */
#include	<ismisc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"opentmp.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;

/* local defines */

#define	MAXLOOP		1000

#define	RANDBUFLEN	(szof(ulong) * 2)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_SPLITFNAME
#define	CF_SPLITFNAME	0		/* used |splitfname(3uc)| */
#endif


/* local namespæces */

using  libuc::libmem ;			/* variable */


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
	} ; /* end ctor */
	int operator () (cchar *,int) noex ;
	int obufbegin	() noex ;
	int obufend	() noex ;
	int typeinit	(int) noex ;
	int setft	() noex ;
	int split	(cchar *) noex ;
	int dirload	() noex ;
	int loop	() noex ;
	int mkofname	() noex ;
	int ofifo	() noex ;
	int odir	() noex ;
	int oreg	() noex ;
	int osock	() noex ;
    } ; /* end struct (openmgr) */
} /* end namespace */

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward reference */

local int	opentmpx	(cchar *,int,mode_t,int,char *) noex ;
local int	opentmpxer	(cchar *,int,mode_t,int,char *) noex ;
local int	randload	(ulong *) noex ;
local int	substr		(char *,int,ulong) noex ;

[[maybe_unused]] local int	debflags	(int) noex ;
[[maybe_unused]] local int	debdesc		(int) noex ;


/* local variables */

static strlibval	val_tmpdir(strlibval_tmpdir) ;
static vars		var ;
constexpr cchar		platename[]	= "otXXXXXXXXXXXX" ;
constexpr bool		f_comment	= false ;
constexpr bool		f_debug		= CF_DEBUG ;
constexpr bool		f_splitfname	= CF_SPLITFNAME ;


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
	DPRINTF("ent dname=%p om=%04x\n",dname,om) ;
	if_constexpr (f_debug) {
	    debflags(of) ;
	}
	if_constexpr (f_comment) {
	    of &= (compl O_ACCMODE) ;
	    of |= O_RDWR ;
	    om |= 0600 ;
	} /* end if_constepxr (f_comment) */
	if (dname == nullptr) {
	    dname = val_tmpdir ;
	    DPRINTF("alt dname=%s\n",dname) ;
	} /* end if (alternative) */
	DPRINTF("adj dname=%p\n",dname) ;
	if (dname && dname[0] && (of >= 0)) {
	    int		ai = 2 ; /* allocated two (2) file-path buffers */
	    DPRINTF("-> var\n") ;
	    if (static cint rsv = var ; (rs = rsv) >= 0) {
		cint	maxpath = rs ;
		cint	sz = (ai * (var.maxpathlen + 1)) ;
	        DPRINTF("-> mall sz=%d\n",sz) ;
		if (char *a ; (rs = lm_mall(sz,&a)) >= 0) {
	            char	*ibuf = (a + (--ai * (maxpath + 1))) ;
	            char	*obuf = (a + (--ai * (maxpath + 1))) ;
	            DPRINTF("mall() rs=%d\n",rs) ;
	            if ((rs = mkpath(ibuf,dname,platename)) >= 0) {
	                DPRINTF("mkpath() rs=%d\n",rs) ;
	                if (sigblocker b ; (rs = b.start) >= 0) {
		            cint	otm = OTM_STREAM ;
	                    DPRINTF("sigblocker_start() rs=%d\n",rs) ;
	                    if ((rs = opentmpx(ibuf,of,om,otm,obuf)) >= 0) {
		                fd = rs ;
	                        DPRINTF("opentmpx() rs=%d\n",rs) ;
	                        if (obuf[0] != '\0') {
			            uc_unlink(obuf) ;
			            obuf[0] = '\0' ;
			        } /* end if (uc_unlink) */
	                    } /* end if (opentempx) */
	                    DPRINTF("sigblocker-out rs=%d\n",rs) ;
	                    rs1 = b.finish ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (sigblock) */
	            } /* end if (mkpath) */
	            rs1 = lm_free(a) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (varsx) */
	    DPRINTF("vars-out rs=%d\n",rs) ;
	    if ((rs < 0) && (fd >= 0)) {
		uc_close(fd) ;
	    } /* end if (error) */
	} /* end if (valid) */
	if_constexpr (f_debug) {
	    if ((rs >= 0) && (fd >= 0)) {
		rs = debdesc(fd) ;
	    }
	} /* end if_constexpr (f_debug) */
	DPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (opentmp) */


/* local subroutines */

local int opentmpx(cchar *inname,int of,mode_t om,int opt,char *obuf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	DPRINTF("ent inname=%s\n",inname) ;
	if (inname && obuf) {
	    rs = SR_INVALID ;
	    DPRINTF("non-empty inname=%s\n",inname) ;
	    if (inname[0] && (of >= 0) && (opt >= 0)) {
	        DPRINTF("-> mall\n") ;
	        if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) {
	            if ((rs = mkpathexp(pbuf,inname,-1)) > 0) {
	                DPRINTF("gt rs=%d\n",rs) ;
		        rs = opentmpxer(pbuf,of,om,opt,obuf) ;
		        fd = rs ;
	            } else if (rs == 0) {
	                DPRINTF("eq rs=%d\n",rs) ;
		        rs = opentmpxer(inname,of,om,opt,obuf) ;
		        fd = rs ;
	            } /* end if */
	            DPRINTF("out rs=%d\n",rs) ;
	            rs1 = lm_free(pbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	        DPRINTF("mall-out ts=%d\n",rs) ;
		if ((rs < 0) && (fd >= 0)) {
		    uc_close(fd) ;
		} /* end if (error) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (opentmpx) */

local int opentmpxer(cchar *inname,int of,mode_t om,int opt,char *obuf) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	DPRINTF("ent inname=%s\n",inname) ;
	if (inname && obuf) {
	    rs = SR_INVALID ;
	    obuf[0] = '\0' ;
	    DPRINTF("valid\n") ;
	    if (inname[0] && (of >= 0) && (opt >= 0)) {
		openmgr oo(of,om,obuf) ;
		DPRINTF("-> oo\n") ;
		rs = oo(inname,opt) ;
		fd = rs ;
		DPRINTF("oo() rs=%d fd=%d\n",rs,fd) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (opentmpxer) */

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
	} /* end if (is-sock) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (openmgr::typeinit) */

int openmgr::setft() noex {
	int		rs = SR_OK ;
	con filetypes	ft = filetype(om) ;
	DPRINTF("ent om=%08x ft=%d\n",om,ft) ;
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
	DPRINTF("ret rs=%d ft=%d\n",rs,ft) ;
	return rs ;
} /* end method (openmgr::setft) */

int openmgr::split(cchar *inname) noex {
	int		rs = SR_OK ;
	DPRINTF("ent inname=%s\n",inname) ;
	if_constexpr (f_splitfname) {
	    DPRINTF("splitfname obj\n") ;
	    if (splitfname so ; (rs = so(inname)) >= 0) {
		dirp = so.dp ;
		dirl = so.dl ;
		basep = so.bp ;
		basel = so.bl ;
		{
		    strnul bs(basep,basel) ;
	            DPRINTF("splitfname bn=%s\n",ccp(bs)) ;
		}
		{
		    strnul ds(dirp,dirl) ;
	            DPRINTF("splitfname dn=%s\n",ccp(ds)) ;
		}
	    } /* end if (splitfname) */
	    DPRINTF("splitfname rs=%d\n",rs) ;
	} else {
	    if ((dirl = sfdirname(inname,-1,&dirp)) >= 0) {
	        rs = SR_ISDIR ;
	        if ((basel = sfbasename(inname,-1,&basep)) > 0) {
		    rs = SR_OK ;
	        } /* end if (sfbasename) */
	    } /* end if (sfdirname) */
	} /* end if_constexpr (f_splitfname) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (openmgr::split) */

int openmgr::dirload() noex {
	int		rs = SR_OK ;
	if (dirl > 0) {
	    if ((rs = mkpathw(obuf,dirp,dirl)) >= 0) {
	        pl = rs ;
	    }
	} /* end if */
	return rs ;
} /* end method (openmgr::dirload) */

int openmgr::obufbegin() noex {
	int		rs = SR_OK ;
	if (obuf == nullptr) {
	    if ((rs = lm_mp(&obuf)) >= 0) {
		obuf[0] = '\0' ;
		falloc = true ;
	    } /* end if (memory-acquire) */
	} /* end if */
	return rs ;
} /* end method (openmgr::obufbegin) */

int openmgr::obufend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (falloc && obuf) {
	    obuf[0] = '\0' ;
	    rs1 = lm_free(obuf) ;
	    if (rs >= 0) rs = rs1 ;
	    obuf = nullptr ;
	    falloc = false ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (openmgr::obufend) */

int openmgr::operator () (cchar *inname,int opt) noex {
	int		rs ;
	int		rs1 ;
	DPRINTF("ent\n") ;
	if ((rs = typeinit(opt)) >= 0) {
	    if ((rs = setft()) >= 0) {
		DPRINTF("1\n") ;
	        if ((rs = split(inname)) >= 0) {
		DPRINTF("2\n") ;
		    if ((rs = dirload()) >= 0) {
		DPRINTF("3\n") ;
	                if ((rs = randload(&rv)) >= 0) {
		DPRINTF("4\n") ;
		            if ((rs = obufbegin()) >= 0) {
		DPRINTF("5\n") ;
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
	    } /* end if (error) */
	} /* end if (typeinit) */
	DPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end method (openmgr::operator) */

int openmgr::mkofname() noex {
	int		rs ;
	if ((rs = pathadd(obuf,pl,basep,basel)) >= 0) {
	    cint	bl = (rs - pl) ;
	    char	*bp = (obuf + pl) ;
	    rs = substr(bp,bl,rv) ;
	} /* end if (pathadd) */
	return rs ;
} /* end method (openmgr::mkofname) */

int openmgr::loop() noex {
	int		rs = SR_OK ;
	DPRINTF("ent\n") ;
	for (int c = 0 ; (rs >= 0) && (c < MAXLOOP) ; c += 1) {
	    if ((rs = mkofname()) >= 0) {
		rv += 1 ;
		rs = (this->*m)() ;
	    } /* end if (mkofname) */
	    if (fd >= 0) break ;
	} /* end for */
	DPRINTF("for-out rs=%d fd=%d\n",rs,fd) ;
	if ((rs >= 0) && (fd < 0)) {
	    rs = SR_ADDRINUSE ;
	} /* end if (error) */
	DPRINTF("for-out rs=%d fd=%d\n",rs,fd) ;
	return rs ;
} /* end method (openmgr::loop) */

int openmgr::ofifo() noex {
	int		rs ;
	if ((rs = uc_mkfifo(obuf,am)) >= 0) {
	    rs = uc_open(obuf,of,am) ;
	    fd = rs ;
	} else if (rs == SR_EXIST) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end method (openmgr::ofifo) */

int openmgr::odir() noex {
	int		rs ;
	if ((rs = uc_mkdir(obuf,am)) >= 0) {
	    int	nof = of ;
	    nof &= (compl O_ACCMODE) ;
	    nof |= O_RDONLY ;
	    if ((rs = uc_open(obuf,nof,am)) >= 0) {
	        fd = rs ;
	    }
	} else if (rs == SR_EXIST) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end method (openmgr::odir) */

int openmgr::oreg() noex {
	int		rs ;
	int		nof = (of | O_CREAT | O_EXCL) ;
	DPRINTF("ent\n") ;
	if_constexpr (f_debug) {
	    debflags(nof) ;
	} /* end if_constexpr (f_debug) */
	if (!  isaccmode_writable(of)) {
	    nof &= (compl O_ACCMODE) ;
	    if (isaccmode_readable(of)) {
	        nof |= O_RDWR ;
	    } else {
	        nof |= O_WRONLY ;
	    }
	} /* end if (access-mode adjustment) */
	if_constexpr (f_debug) {
	    debflags(nof) ;
	} /* end if_constexpr (f_debug) */
	if ((rs = uc_open(obuf,nof,am)) >= 0) {
	    DPRINTF("uc_open() rs=%d\n",rs) ;
	    if (rs >= 0) debdesc(rs) ;
	    fd = rs ;
	} else if (rs == SR_EXIST) {
	    rs = SR_OK ;
	}
	DPRINTF("ret rs=%d fd=%d\n",rs,fd) ;
	return rs ;
} /* end method (openmgr::oreg) */

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
		    cmode	nom = (om & (compl S_IFMT)) ;
                    rs = uc_chmod(obuf,nom) ;
                    if (rs < 0) {
                        uc_unlink(obuf) ;
                        obuf[0] = '\0' ;
                    } /* end if (error) */
                } /* end if (bind) */
                rs1 = sockaddress_finish(&sa) ;
		if (rs >= 0) rs = rs1 ;
            } /* end if (sockaddress) */
            if (rs < 0) {
                uc_close(fd) ;
                fd = -1 ;
            } /* end if (error) */
        } /* end if */
	return rs ;
} /* end method (openmgr::osock) */

local int randload(ulong *rvp) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (rvp) {
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
} /* end subroutine (randload) */

/* load buffer w/ random HEX digits (16 bytes) from random variable (8 bytes) */
local int substr(char *dp,int dl,ulong rv) noex {
	cint		randlen = RANDBUFLEN ;
	int		rs ;
	char		randbuf[RANDBUFLEN+1] ;
	if ((rs = cthex(randbuf,randlen,rv)) >= 0) {
	    for (int j = rs, i = 0 ; i < dl ; i += 1) {
		if (dp[i] == 'X') {
		    if (j > 0) {
		        dp[i] = randbuf[--j] ;
		    }
	        } /* end if */
	    } /* end for */
	} /* end if (cthex) */
	return rs ;
} /* end subroutine (substr) */

vars::operator int () noex {
	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	}
	return rs ;
} /* end method (vars::operator) */

local int debflags(int of) noex {
    	int		rs ;
	int		rs1 ;
	if (char *lbuf ; (rs = libmem.ml(&lbuf)) >= 0) {
	    if ((rs = snflagsopen(lbuf,rs,of)) >= 0) {
		DPRINTF("opentmp oflags %s\n",lbuf) ;
	    } /* end if (snflagsopen) */
	    rs1 = libmem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-d) */
	return rs ;
} /* end subroutine (debflags) */

local int debdesc(int fd) noex {
    	int		rs ;
	if ((rs = u_fgetfl(fd)) >= 0) {
	    debflags(rs) ;
	} /* end if (u_fgetfl) */
	return rs ;
} /* end subroutine (debdesc) */


