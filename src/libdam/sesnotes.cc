/* sesnotes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* send notes to sessions */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sesnotes

	Description:
	This module manages the sending of messages (notes) to
	sessions.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ucgetpid.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<opentmp.h>
#include	<mktmp.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<snx.h>
#include	<sockaddress.h>
#include	<fsdir.h>
#include	<nulstr.h>
#include	<strdcpyx.h>
#include	<strwcpy.h>
#include	<cfdec.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"sesnotes.h"
#include	"sesmsg.h"


/* local defines */

#define	SN		sesnotes
#define	SN_PROGDNAME	"sesnotes"


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		usernamelen ;
	int		maxpathlen ;
	int operator () (int = 0) noex ;
    } ;
}


/* forward references */

static int	mkvars(int) noex ;

template<typename ... Args>
static int sesnotes_ctor(SN *op,Args ... args) noex {
	SESNOTES	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    if (char *bp ; (rs = malloc_un(&bp)) >= 0) {
		static cint	rsv = mkvars(rs) ;
		if ((rs = rsv) >= 0) {
		    op->unbuf = bp ;
		}
		if (rs < 0) {
		    uc_free(bp) ;
		    op->unbuf = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sesnotes_ctor) */

static int sesnotes_dtor(SN *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->unbuf) {
		rs1 = uc_free(op->unbuf) ;
		if (rs >= 0) rs = rs1 ;
		op->unbuf = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sesnotes_dtor) */

template<typename ... Args>
static inline int sesnotes_magic(SN *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SESNOTES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sesnotes_magic) */

static int sesnotes_ready(SN *) noex ;
static int sesnotes_sends(SN *,char *,int,int,int,cchar *,int) noex ;
static int sesnotes_sender(SN *,cchar *,int,time_t,cchar *,int) noex ;

static int haveproc(cchar *,int) noex ;


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int sesnotes_open(SN *op,cchar *un) noex {
	int		rs ;
	if ((rs = sesnotes_ctor(op,un)) >= 0) {
	    rs = SR_INVALID ;
	    if (un[0]) {
		if ((rs = ucpid) >= 0) {
		    cint	ulen = var.usernamelen ;
		    op->pid = rs ;
		    op->fd = -1 ;
		    strdcpy1(op->unbuf,ulen,un) ;
		    op->magic = SESNOTES_MAGIC ;
		} /* end if (ucgetpid) */
	    } /* end if (valid) */
	    if (rs < 0) {
		sesnotes_dtor(op) ;
	    }
	} /* end if (sesnotes_ctor) */
	return rs ;
}
/* end subroutine (sesnotes_open) */

int sesnotes_close(SN *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sesnotes_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        rs1 = u_close(op->fd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fd = -1 ;
	    }
	    if (op->sfname) {
	        if (op->sfname[0] != '\0') {
	            uc_unlink(op->sfname) ;
	            op->sfname[0] = '\0' ;
	        }
		{
	            rs1 = uc_free(op->sfname) ;
	            if (rs >= 0) rs = rs1 ;
	            op->sfname = nullptr ;
		}
	    }
	    {
		rs1 = sesnotes_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sesnotes_close) */

int sesnotes_sendbiff(SN *op,cchar *sp,int sl,pid_t sid) noex {
	cint		mt = sesmsgtype_biff ;
	return sesnotes_send(op,mt,sp,sl,sid) ;
}
/* end subroutine (sesnotes_sendbiff) */

int sesnotes_sendgen(SN *op,cchar *sp,int sl,pid_t sid) noex {
	cint		mt = sesmsgtype_gen ;
	return sesnotes_send(op,mt,sp,sl,sid) ;
}
/* end subroutine (sesnotes_sendgen) */

int sesnotes_send(SN *op,int mt,cchar *mp,int ml,pid_t sid) noex {
	constexpr char	sesdname[] = SESNOTES_SESDNAME ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = sesnotes_magic(op,mp)) >= 0) {
	    custime	dt = getustime ;
	    uint	uv = sid ;
	    int		sz = 0 ;
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
		cint	maxpath = rs ;
		int	ai = 0 ;
		sz += ((maxpath + 1) * 2) ;
		if (char *ap ; (rs = uc_malloc(sz,&ap)) >= 0) {
		     char	*cbuf = (ap + ((maxpath + 1) * ai++)) ;
		     if ((rs = snsd(cbuf,maxpath,"s",uv)) >= 0) {
	    	         char	*dbuf = (ap + ((maxpath + 1) * ai++)) ;
	    	         if ((rs = mkpath(dbuf,sesdname,cbuf)) >= 0) {
	        	     rs = sesnotes_sends(op,dbuf,rs,mt,dt,mp,ml) ;
	        	     c += rs ;
	    	        } /* end if (mkpath) */
		    } /* end if (snsd) */
		    rs1 = uc_free(ap) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (getbufsize) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sesnotes_send) */


/* private subroutines */

static int sesnotes_ready(SN *op) noex {
	constexpr char	dname[] = SN_PROGDNAME ;
	constexpr char	tpat[] = "sesnotesXXXXXX" ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->sfname == nullptr) {
	    cint	sz = ((var.maxpathlen + 1) * 3) ;
	    int		ai = 0 ;
	    cmode	dm = 0775 ;
	    if (char *ap ; (rs = uc_malloc(sz,&ap)) >= 0) {
	        char	*dbuf = ap + ((var.maxpathlen + 1) * ai++) ;
	        if ((rs = mktmpuserdir(dbuf,op->unbuf,dname,dm)) >= 0) {
	            char	*rbuf = ap + ((var.maxpathlen + 1) * ai++) ;
	            if ((rs = mkpath2(rbuf,dbuf,tpat)) >= 0) {
	                cint	of = (O_CREAT|O_RDWR) ;
	                cmode	om = 0666 ;
	                char	*sbuf = ap + ((var.maxpathlen + 1) * ai++) ;
	                if ((rs = opentmpusd(rbuf,of,om,sbuf)) >= 0) {
	                    cchar	*cp ;
	                    op->fd = rs ;
	                    if ((rs = uc_mallocstrw(sbuf,-1,&cp)) >= 0) {
	                        op->sfname = (char *) cp ;
	                        rs = 1 ;
	                    }
	                    if (rs < 0) {
	                        u_close(op->fd) ;
	                        op->fd = -1 ;
	                        uc_unlink(sbuf) ;
	                    }
	                } /* end if (opentmpusd) */
	            } /* end if (mkpath) */
	        } /* end if (mktmpuserdir) */
		rs1 = uc_free(ap) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (initializaion needed) */
	return rs ;
}
/* end subroutine (sesnotes_ready) */

static int sesnotes_sends(SN *op,char *dbuf,int dlen,
		int mt,int st,cchar *mp,int ml) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *nbuf ; (rs = malloc_mn(&nbuf)) >= 0) {
	    cint	nlen = rs ;
	    if (fsdir d ; (rs = fsdir_open(&d,dbuf)) >= 0) {
	        fsdir_ent	de ;
	        while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
		    cint	dnl = rs ;
		    cchar	*dnp = de.name ;
	            if (de.name[0] == 'p') {
		        if ((rs = haveproc((dnp+1),(dnl-1))) > 0) {
	                    if ((rs = pathadd(dbuf,dlen,dnp)) >= 0) {
	                        if (ustat sb ; (rs = u_lstat(dbuf,&sb)) >= 0) {
	                            if (S_ISSOCK(sb.st_mode)) {
				        cchar	*dp = dbuf ;
				        auto	ss = sesnotes_sender ;
	                                rs = ss(op,dp,mt,st,mp,ml) ;
	                                c += rs ;
	                            } /* end if (is-socket) */
	                        } /* end if (stat) */
	                    } /* end if (pathadd) */
		        } else if ((rs == 0) || isNotValid(rs)) {
			    u_unlink(de.name) ;
			    rs = SR_OK ;
		        } /* end if (haveproc) */
	            } /* end if (is-not-leading-dot) */
	            if (rs < 0) break ;
	        } /* end while (reading) */
		dbuf[dlen] = '\0' ;	/* <- reset buffer for repeat */
	        rs1 = fsdir_close(&d) ;
	        if (rs >= 0) rs = rs1 ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sesnotes_sends) */

static int sesnotes_sender(SN *op,cc *ap,int mt,time_t st,cc *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = sesnotes_ready(op)) >= 0) {
	    sockaddress	sa ;
	    uint	tag = op->pid ;
	    cint	af = AF_UNIX ;
	    if ((rs = sockaddress_start(&sa,af,ap,0,0)) >= 0) {
	        cint	sal = rs ;
	        cint	mlen = MSGBUFLEN ;
	        int	ml = -1 ;
	        char	mbuf[MSGBUFLEN+1] ;
	        switch (mt) {
	        case sesmsgtype_gen:
	            {
	                SESMSG_GEN	m2{} ;
	                cint		nlen = SESMSG_NBUFLEN ;
	                m2.tag = tag ;
	                m2.stime = st ;
	                strwcpy(m2.user,op->unbuf,SESMSG_USERLEN) ;
	                strdcpy1w(m2.nbuf,nlen,sp,sl) ;
	                rs = sesmsg_gen(&m2,0,mbuf,mlen) ;
	                ml = rs ;
	            }
	            break ;
	        case sesmsgtype_biff:
	            {
	                SESMSG_BIFF	m3{} ;
	                cint		nlen = SESMSG_NBUFLEN ;
	                m3.tag = tag ;
	                m3.stime = st ;
	                strwcpy(m3.user,op->unbuf,SESMSG_USERLEN) ;
	                strdcpy1w(m3.nbuf,nlen,sp,sl) ;
	                rs = sesmsg_biff(&m3,0,mbuf,mlen) ;
	                ml = rs ;
	            }
	            break ;
	        default:
	            rs = SR_PROTOTYPE ;
	            break ;
	        } /* end switch */
	        if (rs >= 0) {
	            SOCKADDR	*sap = (SOCKADDR *) &sa ;
	            cint	s = op->fd ;
	            if ((rs = u_sendto(s,mbuf,ml,0,sap,sal)) >= 0) {
	                f = true ;
	            } else {
			if (rs == SR_DESTADDRREQ) {
			    nulstr	n ;
			    cchar	*name ;
			    if ((rs = n.start(ap,al,&name)) >= 0) {
				{
			            u_unlink(name) ;
				}
				rs1 = n.finish ;
				if (rs >= 0) rs = rs1 ;
			    } /* end if (nulstr) */
			} else {
	                    rs = SR_OK ; /* just ignore any other failures */
			}
	            } /* end if */
	        } /* end if (ok) */
	        rs1 = sockaddress_finish(&sa) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sockaddress) */
	} /* end if (sesnotes_ready) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (sesnotes_sender) */

static int haveproc(cchar *pp,int pl) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (pl > 0) {
	    if (uint uv{} ; (rs = cfdecui(pp,pl,&uv)) >= 0) {
		const pid_t	pid = uv ;
		rs = uc_prochave(pid) ;
		f = (rs > 0) ;
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (haveproc) */

int vars::operator () (int unl) noex {
	int		rs = SR_BUGCHECK ;
	if (unl > 0) {
	    var.usernamelen = unl ;
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
		var.maxpathlen = rs ;
	    }
	}
	return rs ;
}
/* end method (vars::operator) */

static int mkvars(int unl) noex {
    	return var(unl) ;
}


