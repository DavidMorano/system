/* sesnotes SUPPORT */
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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<ucgetpid.h>
#include	<mktmp.h>
#include	<sockaddress.h>
#include	<fsdir.h>
#include	<nulstr.h>
#include	<localmisc.h>

#include	"sesnotes.h"
#include	"sesmsg.h"


/* local defines */

#define	SESNOTES_PROGDNAME	"sesnotes"

#define	NDF			"sesnotes.deb"

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif


/* external subroutines */


/* local structures */


/* forward references */

template<typename ... Args>
static int sesnotes_ctor(sesnotes *op,Args ... args) noex {
	SESNOTES	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    if (char *bp{} ; (rs = malloc_un(&bp)) >= 0) {
		op->unbuf = bp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sesnotes_ctor) */

static int sesnotes_dtor(sesnotes *op) noex {
	int		rs = SR_FAULT ;
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
static inline int sesnotes_magic(sesnotes *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SESNOTES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sesnotes_magic) */

static int sesnotes_ready(sesnotes *) noex ;
static int sesnotes_sends(sesnotes *,char *,int,int,int,cchar *,int) noex ;
static int sesnotes_sender(sesnotes *,cchar *,int,int,time_t,cchar *,int) noex ;

static int haveproc(cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sesnotes_open(sesnotes *op,cchar *un) noex {
	int		rs ;
	if ((rs = sesnotes_ctor(op,un)) >= 0) {
	    rs = SR_INVALID ;
	    if (un[0]) {
		if ((rs = ucgetpid()) >= 0) {
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

int sesnotes_close(sesnotes *op) noex {
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
	        rs1 = uc_free(op->sfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->sfname = nullptr ;
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

int sesnotes_sendbiff(sesnotes *op,cchar *sp,int sl,pid_t sid) noex {
	cint		mt = sesmsgtype_biff ;
	return sesnotes_send(op,mt,sp,sl,sid) ;
}
/* end subroutine (sesnotes_sendbiff) */

int sesnotes_sendgen(sesnotes *op,cchar *sp,int sl,pid_t sid) noex {
	cint		mt = sesmsgtype_gen ;
	return sesnotes_send(op,mt,sp,sl,sid) ;
}
/* end subroutine (sesnotes_sendgen) */

int sesnotes_send(sesnotes *op,int mt,cchar *mp,int ml,pid_t sid) noex {
	const time_t	st = time(nullptr) ;
	uint		uv = sid ;
	cint	clen = MAXNAMELEN ;
	int		rs ;
	int		c = 0 ;
	cchar	*sesdname = SESNOTES_SESDNAME ;
	char		cbuf[MAXNAMELEN+1] ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != SESNOTES_MAGIC) return SR_NOTOPEN ;
	if ((rs = snsd(cbuf,clen,"s",uv)) >= 0) {
	    char	dbuf[MAXPATHLEN+1] ;
	    if ((rs = mkpath2(dbuf,sesdname,cbuf)) >= 0) {
	        cint	dl = rs ;
	        rs = sesnotes_sends(op,dbuf,dl,mt,st,mp,ml) ;
	        c += rs ;
	    } /* end if (mkpath) */
	} /* end if (snsd) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sesnotes_send) */


/* private subroutines */

static int sesnotes_ready(sesnotes *op) noex {
	int		rs = SR_OK ;
	if (op->sfname == nullptr) {
	    const mode_t	dm = 0775 ;
	    cchar		*dname = SESNOTES_PROGDNAME ;
	    char		dbuf[MAXPATHLEN+1] ;
	    if ((rs = mktmpuserdir(dbuf,op->unbuf,dname,dm)) >= 0) {
	        cchar	*template = "sesnotesXXXXXX" ;
	        char		rbuf[MAXNAMELEN+1] ;
	        if ((rs = mkpath2(rbuf,dbuf,template)) >= 0) {
	            const mode_t	om = 0666 ;
	            cint		of = (O_CREAT|O_RDWR) ;
	            char		sbuf[MAXPATHLEN+1] ;
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
	} /* end if (initializaion needed) */
	return rs ;
}
/* end subroutine (sesnotes_ready) */

static int sesnotes_sends(sesnotes *op,char *dbuf,int dlen,
		int mt,int st,cchar *mp,int ml) noex {
	fsdir		d ;
	fsdir_ent	de ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = fsdir_open(&d,dbuf)) >= 0) {
	    while ((rs = fsdir_read(&d,&de)) > 0) {
	        if (de.name[0] == 'p') {
		    if ((rs = haveproc((de.name+1),(rs-1))) > 0) {
	                if ((rs = pathadd(dbuf,dlen,de.name)) >= 0) {
	                    USTAT	sb ;
			    cint	dl = rs ;
			    if ((rs = u_lstat(dbuf,&sb)) >= 0) {
	                        if (S_ISSOCK(sb.st_mode)) {
				    cchar	*dp = dbuf ;
	                            rs = sesnotes_sender(op,dp,dl,mt,st,mp,ml) ;
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
	    dbuf[dlen] = '\0' ;
	    rs1 = fsdir_close(&d) ;
	    if (rs >= 0) rs = rs1 ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sesnotes_sends) */

static int sesnotes_sender(sesnotes *op,cc *ap,int al,int mt,int st,
		cc *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = sesnotes_ready(op)) >= 0) {
	    sockaddress	sa ;
	    uint	tag = op->pid ;
	    cint	af = AF_UNIX ;
	    if ((rs = sockaddress_startaddr(&sa,af,ap,al,0,0)) >= 0) {
	        cint	sal = rs ;
	        cint	mlen = MSGBUFLEN ;
	        int	ml = -1 ;
	        char	mbuf[MSGBUFLEN+1] ;
	        switch (mt) {
	        case sesmsgtype_gen:
	            {
	                SESMSG_GEN	m2 ;
	                cint	nlen = SESMSG_NBUFLEN ;
	                memset(&m2,0,sizeof(SESMSG_GEN)) ;
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
	                SESMSG_BIFF	m3 ;
	                cint	nlen = SESMSG_NBUFLEN ;
	                memset(&m3,0,sizeof(SESMSG_BIFF)) ;
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
			    if ((rs = nulstr_start(&n,ap,al,&name)) >= 0) {
				{
			            u_unlink(name) ;
				}
				rs1 = nulstr_finish(&n) ;
				if (rs >= 0) rs = rs1 ;
			    } /* end if (nulstr) */
			} else {
	                    rs = SR_OK ;
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
		rs = isproc(pid) ;
		f = (rs > 0) ;
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (haveproc) */


