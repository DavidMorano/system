/* prognotify SUPPORT */
/* lang=C++20 */

/* fairly generic (PCS) subroutine */
/* version %I% last-modified %G% */

#define	CF_ISSAMEHOST	1		/* use 'issamehostname(3dam)' */

/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine sends a message to hosts indicating that
	some mail has come in for a user.

	Implementation notes:
	Note that we use the same socket for all messages. This
	avoids creating new sockets for each one. But it also means
	that we only bother with one protocol family, and that
	family is PF_INET. This has not proved to be a big problem!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<unistd.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getportnum.h>
#include	<bfile.h>
#include	<vecobj.h>
#include	<paramfile.h>
#include	<sockaddress.h>
#include	<hash.h>
#include	<snx.h>
#include	<strwcpy.h>
#include	<mcmsg.h>
#include	<recip.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"proginfo.hh"


/* local defines */

#define	REPORTINFO	struct reportinfo

#define	BUFLEN		MAX((2 * MAXHOSTNAMELEN),MSGBUFLEN)
#define	HOSTBUFLEN	(10 * MAXHOSTNAMELEN)
#define	NSBUFLEN	(MAXHOSTNAMELEN + 32)

#define	PROTONAME	"udp"

#ifndef	LOCALHOST
#define	LOCALHOST	"localhost"
#endif

#ifndef	IPPORT_BIFFUDP
#define	IPPORT_BIFFUDP	512
#endif

#ifndef	IPPORT_MAILPOLL
#define	IPPORT_MAILPOLL	5110
#endif

#define	MSGSENDTRIES	5


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	getheour(cchar *,cchar *,HOSTENT *,char *,int) noex ;
    extern int	issamehostname(cchar *,cchar *,cchar *) noex ;
    extern int	parsenodespec(proginfo *,char *,cchar *,int) noex ;
}


/* external variables */


/* local structures */

struct reportinfo {
	mcmsg_rep	m1 ;
	sockaddress	sa ;
	uint		tag ;
	int		defport ;
	int		salen ;
	int		fd ;
	int		sendflags ;
	char		buf[MSGBUFLEN + 1] ;
} ;


/* forward references */

static int	prognotifyrecip(proginfo *,vecobj *,
			REPORTINFO *,paramfile *,RECIP *) noex ;
static int	prognotifyrecipnode(proginfo *,vecobj *,
			REPORTINFO *,RECIP *,cchar *,int) noex ;
static int	report(proginfo *,REPORTINFO *) noex ;

static int	searchfunc() noex ;


/* local variables */

static cint	frd = true ;
static cint	fwr = false ;


/* exported variables */


/* exported subroutines */

int prognotify(proginfo *pip,vecobj *mip,vecobj *rsp) noex {
	REPORTINFO	ri{} ;
	paramfile	mbtab ;
	int		rs ;
	int		rs1 ;
	int		defport ;
	int		cl ;
	int		c = 0 ;
	cchar		*proto = PROTONAME ;
	cchar		*svc = PORTSPEC_MAILPOLL ;
	cchar		*cluster = pip->cluster ;

/* do we have a MBTAB database? */

	if ((pip->mbfname == nullptr) || (! pip->fl.mbtab))
	    return SR_OK ;

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: mbtab=%s\n",
	        pip->progname,pip->mbfname) ;
	}

	if (pip->open.logmsg)
	    logfile_printf(&pip->lh,"mbtab=%s\n",pip->mbfname) ;

/* get some information about MAILPOLL */

	if ((rs = getportnum(proto,svc)) >= 0) {
	    defport = rs ;
	} else if (isNotPresent(rs)) {
	    defport = IPPORT_MAILPOLL ;
	    rs = SR_OK ;
	}

	ri.defport = defport ;

/* set some stuff that is relatively constant and used later */

	if (cluster == nullptr) cluster = pip->nodename ;
	{
	    char	hashbuf[MAXHOSTNAMELEN+ 1] ;
	    cl = snsds(hashbuf,MAXHOSTNAMELEN,cluster,pip->logid) ;
	    ri.tag = hash_elf(hashbuf,cl) ;
	}

	if (rs >= 0) {
	   cint	pf = PF_INET ;
	   cint	st = SOCK_DGRAM ;
	   cint	pnum = IPPROTO_UDP ;

/* initialze the message to send */

	ri.m1 = {} ;
	ri.m1.tag = ri.tag ;
	ri.m1.rc = mcmsgrc_ok ;
	strwcpy(ri.m1.cluster,cluster,MCMSG_LCLUSTER) ;

/* go through the recipients */

	if ((rs = u_socket(pf,st,pnum)) >= 0) {
	    cchar	**envv = pip->envv ;
	    cchar	*mbfname = pip->mbfname ;
	    ri.fd = rs ;

	    if ((rs = paramfile_open(&mbtab,envv,mbfname)) >= 0) {
	        RECIP	*rp ;
	        for (int i = 0 ; vecobj_get(rsp,i,&rp) >= 0 ; i += 1) {
	            if (rp != nullptr) {
	                if (rp->ds >= 0) {
	                    rs = prognotifyrecip(pip,mip,&ri,&mbtab,rp) ;
	                }
		    }
	            if (rs < 0) break ;
	        } /* end for (recipients) */
	        rs1 = paramfile_close(&mbtab) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (paramfile) */
	    rs1 = u_close(ri.fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (socket) */

	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (prognotify) */


/* local subroutines */

static int prognotifyrecip(proginfo *pip,vecobj *mip,REPORTINFO *rip,
		paramfile *mbp,RECIP *rp) noex {
	paramfile_cur	cur ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		ul ;
	int		c = 0 ;
	cchar	*up ;

	up = rp->recipient ;
	ul = strlen(up) ;

/* prepare some additional message information */

	strwcpy(rip->m1.mailuser,up,MIN(ul,MCMSG_LMAILUSER)) ;

/* loop through target machines for this mailuser (recipient) */

	if ((rs = paramfile_curbegin(mbp,&cur)) >= 0) {
	    cint	nslen = NSBUFLEN ;
	    cint	rsn = SR_NOTFOUND ;
	    int		port ;
	    int		nsl ;
	    char	nsbuf[NSBUFLEN + 1] ;
	    char	nn[NODENAMELEN + 1] ;

	    while (rs >= 0) {
	        nsl = paramfile_fetch(mbp,up,&cur,nsbuf,nslen) ;
	        if (nsl == rsn) break ;
	        rs = nsl ;
	        if (rs < 0) break ;
		/* separate the node-spec into node and port */
	        if ((rs1 = parsenodespec(pip,nn,nsbuf,nsl)) >= 0) {
	            port = rs1 ;
	        } else if (rs1 == rsn) {
	            port = rip->defport ;
	        } else {
	            rs = rs1 ;
		}
	        if ((rs >= 0) && (nn[0] != '\0')) {
	            rs = prognotifyrecipnode(pip,mip,rip,rp,nn,port) ;
	            if (rs > 0) c += 1 ;
	        }
	    } /* end while (target machines) */
	    rs1 = paramfile_curend(mbp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (prognotifyrecip) */

static int prognotifyrecipnode(proginfo *pip,vecobj *mip,REPORTINFO *rip,
		RECIP *rp,cc *nn,int port) noex {
	HOSTENT		he, *hep = &he ;
	MSGINFO		mi, *iep = &mi ;
	int		moff ;
	cint		helen = getbufsize(getbufsize_he) ;
	cint		af = AF_INET4 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		k ;
	int		ml ;
	int		c = 0 ;
	cchar		*np = nn ;
	char		*hebuf ;

#if	CF_ISSAMEHOST
	if (issamehostname(np,pip->nodename,pip->domainname))
	    np = LOCALHOST ;
#else
	if (strcmp(np,pip->nodename) == 0)
	    np = LOCALHOST ;
#endif

	if ((rs = uc_malloc((helen+1),&hebuf)) >= 0) {
	    if ((rs = getheour(np,nullptr,hep,hebuf,helen)) >= 0) {
		if (hep->h_addrtype == AF_INET) {
	    	    cchar	*a = (cchar *) hep->h_addr ;

	    if ((rs = sockaddress_start(&rip->sa,af,a,port,0)) >= 0) {
	        rip->salen = rs ;

/* send the messages */

	        for (k = 0 ; (ml = recip_getmo(rp,k,&moff)) >= 0 ; k += 1) {
		    /* find this message in the MSGINFO list */
	            mi.moff = moff ;
	            mi.mlen = ml ;
	            rs1 = vecobj_search(mip,&mi,searchfunc,&iep) ;
	            if (rs1 >= 0) {
	                rip->m1.mlen = iep->mlen ;
	                rip->m1.flags = 0 ;
	                rip->m1.flags |= (iep->fl.spam) ? 1 : 0 ;
	                rip->m1.flags |= (iep->fl.messageid) ? 2 : 0 ;
	                strwcpy(rip->m1.msgid,iep->h_messageid,MCMSG_LMSGID) ;
	                strwcpy(rip->m1.from,iep->h_from,MCMSG_LFROM) ;
			/* send the report */
	                rs = report(pip,rip) ;
	                if (rs > 0) c += 1 ;
	            } /* end if (found message) */

	        } /* end for (messages) */

	        rs1 = sockaddress_finish(&rip->sa) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if */

	    } /* end if (ver=inet4) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	    rs1 = uc_free(hebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (prognotifyrecipnode) */

static int report(proginfo *pip,REPORTINFO *rip) noex {
	SOCKADDR	*sap = (SOCKADDR *) &rip->sa ;
	cint		mlen = MSGBUFLEN ;
	cint		to = pip->to_msgread ;
	cint		sflags = rip->sendflags ;
	cint		sal = rip->salen ;
	cint		n = MSGSENDTRIES ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		blen ;
	int		f = false ;
	for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	    rip->m1.seq = (uchar) i ;
	    rip->m1.timestamp = (uint) time(nullptr) ;
	    rs = mcmsg_report(&rip->m1,frd,rip->buf,mlen) ;
	    blen = rs ;
	    if (rs < 0) break ;
	    if ((rs = u_sendto(rip->fd,rip->buf,blen,sflags,sap,sal)) >= 0) {
		mcmsg_ack	m2{} ;
		cint		fm = FM_TIMED ;
	        if ((rs1 = uc_recve(rip->fd,rip->buf,mlen,0,to,fm)) >= 0) {
	            blen = mcmsg_acknowledge(&m2,fwr,rip->buf,mlen) ;
	            if ((blen > 0) && (m2.tag == rip->m1.tag)) f = true ;
	        }
	    }
	    if (f) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (report) */

static int searchfunc(MSGINFO **e1pp,MSGINFO **e2pp) noex {
	int		rc = 0 ;
	if (*e1pp || *e2pp) {
	    if (*e1pp) {
	        if (*e2pp) {
	            rc = ((*e1pp)->moff - (*e2pp)->moff) ;
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = 1 ;
	    }
	} else {
	    rc = 0 ;
	}
	return rc ;
}
/* end subroutine (searchfunc) */


