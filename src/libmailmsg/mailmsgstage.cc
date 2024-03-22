/* mailmsgstage SUPPORT */
/* lang=C++20 */

/* process the input messages and spool them up */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module processes one or more mail messages (in
	appropriate mailbox format if more than one) from a file
	descriptor passed in the 'init' call-method.  The input
	file descriptor can be a pipe as a standard mailbox file
	is not required (one of the whole reasons for this object
	as opposed to the MAILBOX object).

	Any mail messages found are effectively partially parsed
	and the object is then ready for queries by additional
	method calls.

	The parsed mail meta-data is stored in a manner such that
	it is optimized for repeated access.

	Use should proceed roughly as:

		if ((rs = mailmsgstage_start()) >= 0) {

		    mailmsgstage_xxxx() 

		    mailmegstage_finish()
		} end if (mailmsgstage)

	This object is pretty fast also!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<sys/time.h>		/* for |gethrtime(3c)| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>		/* |memcpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<filebuf.h>
#include	<fdliner.h>
#include	<vechand.h>
#include	<mailmsghdrs.h>
#include	<mailmsgmatenv.h>
#include	<mailmsgmathdr.h>
#include	<mailmsg.h>
#include	<comparse.h>
#include	<mhcom.h>
#include	<strn.h>
#include	<six.h>
#include	<mkpathx.h>
#include	<hdrextnum.h>
#include	<char.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"mailmsgstage.h"


/* local defines */

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	HDRNAMELEN
#define	HDRNAMELEN	80
#endif

#ifndef	MSGLINELEN
#define	MSGLINELEN	(2 * 1024)
#endif

#ifndef	MAXMSGLINELEN
#define	MAXMSGLINELEN	76
#endif

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
#endif

#ifndef	TABLEN
#define	TABLEN		8
#endif

#define	DATEBUFLEN	80
#define	STACKADDRBUFLEN	(2 * 1024)

#undef	BUFLEN
#define	BUFLEN		(2 * 1024)

#define	FMAT(cp)	((cp)[0] == 'F')

#ifndef	HN_XMAILER
#define	HN_XMAILER	"x-mailer"
#endif

#ifndef	HN_RECEIVED
#define	HN_RECEIVED	"received"
#endif

#undef	NBLANKS
#define	NBLANKS		20

#define	MMS		mailmsgstage

#define	MSGENTRY	struct msgentry
#define	MSGENTRY_FL	struct msgentry_flags


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	opentmpfile(cchar *,int,mode_t,char *) noex ;
}


/* external variables */


/* local structures */

struct msgentry_flags {
	uint		ctype:1 ;
	uint		cencoding:1 ;
	uint		ctplain:1 ;
	uint		ceplain:1 ;
	uint		plaintext:1 ;
	uint		clen:1 ;
	uint		clines:1 ;
	uint		eoh:1 ;
} ;

struct msgentry {
	MSGENTRY_FL	f, hdr ;
	MAILMSG		m ;
	off_t		boff ;		/* w/ tmpfile */
	int		blen ;		/* w/ tmpfile */
	int		clen ;		/* supplied or calculated */
	int		clines ;	/* supplied or calculated */
} ;


/* forward references */

static int	mailmsgstage_starts(MMS *,int,cc *) noex ;
static int	mailmsgstage_starter(MMS *,int) noex ;
static int	mailmsgstage_g(MMS *,int) noex ;
static int	mailmsgstage_gmsg(MMS *,filebuf *,fdliner *,int) noex ;
static int	mailmsgstage_gmsgbody(MMS *,filebuf *,fdliner *,
			MSGENTRY *) noex ;
static int	mailmsgstage_gmsgent(MMS *,filebuf *,fdliner *,
			cchar *,int,int) noex ;
static int	mailmsgstage_gmsgenter(MMS *,filebuf *,fdliner *,
			MSGENTRY *) noex ;
static int	mailmsgstage_msgfins(MMS *) noex ;
static int	mailmsgstage_gmsgentnew(MMS *,MSGENTRY **) noex ;
static int	mailmsgstage_gmsgentdel(MMS *,MSGENTRY *) noex ;

static int	msgentry_start(MSGENTRY *) noex ;
static int	msgentry_finish(MSGENTRY *) noex ;
static int	msgentry_loadline(MSGENTRY *,cchar *,int) noex ;
static int	msgentry_loadhdrs(MSGENTRY *,fdliner *) noex ;
static int	msgentry_setflags(MSGENTRY *) noex ;
static int	msgentry_setct(MSGENTRY *) noex ;
static int	msgentry_setce(MSGENTRY *) noex ;
static int	msgentry_getclines(MSGENTRY *) noex ;
static int	msgentry_setclines(MSGENTRY *,int) noex ;
static int	msgentry_getclen(MSGENTRY *) noex ;
static int	msgentry_setclen(MSGENTRY *,int) noex ;
static int	msgentry_setoff(MSGENTRY *,off_t) noex ;
static int	msgentry_setlen(MSGENTRY *,int) noex ;
static int	mailentry_gethdrnum(MSGENTRY *,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsgstage_start(MMS *op,int ifd,int to,int mmo) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (op) {
	    cnullptr	np{} ;
	    rs = SR_BADF ;
	    memclear(op) ; /* dangerous */
	    if (ifd >= 0) {
		cint	osz = sizeof(vechand) ;
	        cchar	*tmpdn = nullptr ;
		void	*vp{} ;
	        op->tfd = -1 ;
		op->to = to ;
	        op->f.useclen = bool(mmo & MAILMSGSTAGE_OUSECLEN) ;
	        op->f.useclines = bool(mmo & MAILMSGSTAGE_OUSECLINES) ;
	        if (tmpdn == np) tmpdn = getenv(VARTMPDNAME) ;
	        if (tmpdn == np) tmpdn = MAILMSGSTAGE_TMPDNAME ;
		if ((rs = uc_malloc(osz,&vp)) >= 0) {
		    op->mlp = (vechand *) vp ;
	            if ((rs = mailmsgstage_starts(op,ifd,tmpdn)) >= 0) {
			n = rs ;
		    } /* end if (mailmsgstage_starts) */
		    if (rs < 0) {
			uc_free(op->mlp) ;
			op->mlp = nullptr ;
		    }
		} /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailmsgstage_start) */

static int mailmsgstage_starts(MMS *op,int ifd,cc *tmpdn) noex {
	int		rs ;
	int		rs1 ;
	int		nmsgs = 0 ;
	char		*tpat{} ;
	if ((rs = malloc_mp(&tpat)) >= 0) {
	    cchar	*xpat = "msXXXXXXXXXXXX" ;
	    if ((rs = mkpath2(tpat,tmpdn,xpat)) >= 0) {
	        cint	of = O_RDWR ;
	        cmode	om = 0660 ;
	        char	*tbuf{} ;
		if ((rs = malloc_mp(&tbuf)) >= 0) {
	            if ((rs = opentmpfile(tpat,of,om,tbuf)) >= 0) {
	                op->tfd = rs ;
	                if ((rs = uc_closeonexec(op->tfd,true)) >= 0) {
	                    cchar	*cp ;
	                    if ((rs = uc_mallocstrw(tbuf,-1,&cp)) >= 0) {
	                        op->tmpfname = cp ;
			        {
	                            rs = mailmsgstage_starter(op,ifd) ;
	                            nmsgs = rs ;
			        }
	                        if (rs < 0) {
	                            uc_free(op->tmpfname) ;
	                            op->tmpfname = nullptr ;
	                        }
	                    } /* end if (m-a) */
	                } /* end if (uc_closeonexec) */
	                if (rs < 0) {
	                    u_close(op->tfd) ;
	                    op->tfd = -1 ;
	                    if (tbuf[0] != '\0') {
			        u_unlink(tbuf) ;
			        tbuf[0] = '\0' ;
		            }
	                }
	            } /* end if (opentmpfile) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (mkpath2) */
	    rs1 = uc_free(tpat) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mailmsgstage_starts) */

static int mailmsgstage_starter(MMS *op,int ifd) noex {
	int		rs ;
	int 		vo = 0 ;
	int		n = 0 ;
	vo |= VECHAND_OCOMPACT ;
	vo |= VECHAND_OSTATIONARY ;
	if ((rs = vechand_start(op->mlp,4,vo)) >= 0) {
	    if ((rs = mailmsgstage_g(op,ifd)) >= 0) {
	        n = rs ;
	        if (op->tflen > 0) {
		    cnullptr	np{} ;
	            csize	ms = size_t(op->tflen) ;
	            cint	mp = PROT_READ ;
	            cint	mf = MAP_SHARED ;
	            cint	fd = op->tfd ;
	            void	*md{} ;
	            if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0L,&md)) >= 0) {
	                op->mapdata = charp(md) ;
	                op->mapsize = ms ;
	            }
	        } /* end if */
	        if (rs >= 0) op->magic = MAILMSGSTAGE_MAGIC ;
	        if (rs < 0) {
	            mailmsgstage_msgfins(op) ;
		}
	    } /* end if */
	    if (rs < 0) {
	        vechand_finish(op->mlp) ;
	    }
	} /* end if (vechand) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailmsgstage_starter) */

int mailmsgstage_finish(MMS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if (op->mapdata != nullptr) {
	    rs1 = u_munmap(op->mapdata,op->mapsize) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	}

	rs1 = mailmsgstage_msgfins(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vechand_finish(op->mlp) ;
	if (rs >= 0) rs = rs1 ;

	if (op->tfd >= 0) {
	    rs1 = u_close(op->tfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->tfd = -1 ;
	}

	if (op->tmpfname != nullptr) {
	    if (op->tmpfname[0] != '\0') {
	        u_unlink(op->tmpfname) ;
	    }
	    rs1 = uc_free(op->tmpfname) ;
	    op->tmpfname = nullptr ;
	    if (rs >= 0) rs = rs1 ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (mailmsgstage_finish) */

int mailmsgstage_count(MMS *op) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	rs = vechand_count(op->mlp) ;

	return rs ;
}
/* end subroutine (mailmsgstage_count) */

int mailmsgstage_clen(MMS *op,int mi) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = (mep->clen >= 0) ? mep->clen : SR_NOTFOUND ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_clen) */

int mailmsgstage_clines(MMS *op,int mi) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = (mep->clines >= 0) ? mep->clines : SR_NOTFOUND ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_clines) */

int mailmsgstage_envcount(MMS *op,int mi) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = mailmsg_envcount(&mep->m) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_envcount) */

int mailmsgstage_envaddress(MMS *op,int mi,int n,cchar **rpp) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = mailmsg_envaddress(&mep->m,n,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_envaddress) */

int mailmsgstage_envdate(MMS *op,int mi,int n,cchar **rpp) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = mailmsg_envdate(&mep->m,n,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_envdate) */

int mailmsgstage_envremote(MMS *op,int mi,int n,cchar **rpp) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = mailmsg_envremote(&mep->m,n,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_envremote) */

int mailmsgstage_hdrikey(MMS *op,int mi,int hi,cchar **rpp) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = mailmsg_hdrikey(&mep->m,hi,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdrikey) */

int mailmsgstage_hdrcount(MMS *op,int mi,cchar *name) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = mailmsg_hdrcount(&mep->m,name) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdrcount) */

int mailmsgstage_hdriline(MMS *op,int mi,cchar *name,int hi,int li,
		cchar **rpp) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = mailmsg_hdriline(&mep->m,name,hi,li,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdriline) */

int mailmsgstage_hdrival(MMS *op,int mi,cchar *name,
		int hi,cchar **rpp) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = mailmsg_hdrival(&mep->m,name,hi,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdrival) */

int mailmsgstage_hdrval(MMS *op,int mi,cchar *name,
		cchar **rpp) noex {
	MSGENTRY	*mep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    rs = mailmsg_hdrval(&mep->m,name,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdrval) */

int mailmsgstage_flags(MMS *op,int mi) noex {
	MSGENTRY	*mep ;
	int		rs ;
	int		flags = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	    flags |= ((mep->hdr.clen) ? MAILMSGSTAGE_MCLEN : 0) ;
	    flags |= ((mep->hdr.clines) ? MAILMSGSTAGE_MCLINES : 0) ;
	    flags |= ((mep->hdr.ctype) ? MAILMSGSTAGE_MCTYPE : 0) ;
	    flags |= ((mep->hdr.cencoding) ? MAILMSGSTAGE_MCENCODING : 0) ;
	    flags |= ((mep->f.ctplain) ? MAILMSGSTAGE_MCTPLAIN : 0) ;
	    flags |= ((mep->f.ceplain) ? MAILMSGSTAGE_MCEPLAIN : 0) ;
	    flags |= ((mep->f.plaintext) ? MAILMSGSTAGE_MCPLAIN : 0) ;
	}

	return (rs >= 0) ? flags : rs ;
}
/* end subroutine (mailmsgstage_flags) */

int mailmsgstage_bodyget(MMS *op,int mi,off_t boff,cchar **bpp) noex {
	int		rs = SR_OK ;
	int		ml = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if (mi < 0) return SR_INVALID ;

	if (boff < 0) return SR_DOM ;

	if (bpp != nullptr)
	    *bpp = nullptr ;

	if (op->mapsize > 0) {
	    MSGENTRY	*mep ;
	    if ((rs = vechand_get(op->mlp,mi,&mep)) >= 0) {
	        if (boff < mep->blen) {
	            off_t	moff = (mep->boff + boff) ;
	            ml = (mep->blen - boff) ;
	            if (bpp != nullptr)
	                *bpp = (op->mapdata + moff) ;
	        }
	    }
	}

	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (mailmsgstage_bodyget) */

int mailmsgstage_bodyread(MMS *op,int mi,off_t boff,
		char *bbuf,int blen) noex {
	int		rs ;
	int		ml = 0 ;
	cchar		*bp ;

	if (boff < 0) return SR_DOM ;

	if (bbuf == nullptr) return SR_FAULT ;

	if (blen < 0) return SR_INVALID ;

	if ((rs = mailmsgstage_bodyget(op,mi,boff,&bp)) > 0) {
	    ml = MIN(rs,blen) ;
	    memcpy(bbuf,bp,ml) ;
	}

	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (mailmsgstage_bodyread) */


/* private subroutines */

static int mailmsgstage_g(MMS *op,int ifd) noex {
	filebuf		tfb ;
	const off_t	ostart = 0L ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = filebuf_start(&tfb,op->tfd,ostart,0,0)) >= 0) {
	    fdliner	ls, *lsp = &ls ;
	    cint	to = op->to ;
	    if ((rs = fdliner_start(lsp,ifd,ostart,to)) >= 0) {
		int	mi = 0 ;
	        while ((rs = mailmsgstage_gmsg(op,&tfb,lsp,mi++)) > 0) {
		    /* nothing to do here */
	        } /* end while */
	        n = op->nmsgs ;
	        rs1 = fdliner_finish(lsp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (liner) */
	    rs1 = filebuf_finish(&tfb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filebuf) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailmsgstage_g) */

/* parse out the headers of this message */
static int mailmsgstage_gmsg(MMS *op,filebuf *tfp,
		fdliner *lsp,int mi) noex {
	MAILMSGMATENV	me ;
	int		rs = SR_OK ;
	int		vi = -1 ;
	int		ll = 0 ;
	int		f_env = false ;
	int		f_hdr = false ;
	int		f_eoh = false ;
	cchar	*lp ;

/* find message start */

	while ((rs = fdliner_read(lsp,&lp)) > 0) {
	    ll = rs ;
	    if (ll == 0) break ;

	    if ((ll > 5) && FMAT(lp) &&
		((rs = mailmsgmatenv(&me,lp,ll)) > 0)) {
	        f_env = true ;
	    } else if ((ll > 2) &&
	       ((rs = mailmsgmathdr(lp,ll,&vi)) > 0)) {
	        f_hdr = true ;
	    } else if ((ll <= 2) && (mi == 0)) {
	        if ((lp[0] == '\n') || hasEOH(lp,ll)) {
	            f_eoh = true ;
		}
	    }

	    if (rs < 0) break ;
	    if (f_env || f_hdr) break ;

	    ll = 0 ;
	    fdliner_done(lsp) ;

	    if (f_eoh) break ;
	} /* end while */

	if ((rs >= 0) && (f_eoh || f_env || f_hdr)) {
	    rs = mailmsgstage_gmsgent(op,tfp,lsp,lp,ll,f_eoh) ;
	    ll = rs ;
	}

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsgstage_gmsg) */

static int mailmsgstage_gmsgent(MMS *op,filebuf *tfp,fdliner *lsp,
		cchar *lp,int ll,int f_eoh) noex {
	MSGENTRY	*mep ;
	int		rs ;
	if ((rs = mailmsgstage_gmsgentnew(op,&mep)) >= 0) {
	    if ((rs = msgentry_start(mep)) >= 0) {
	        if ((! f_eoh) && (ll > 0)) {
	            rs = msgentry_loadline(mep,lp,ll) ;
		    ll = rs ;
	            f_eoh = (rs == 1) ;
	            fdliner_done(lsp) ;
	        }
	        if ((rs >= 0) && (! f_eoh)) {
	            rs = msgentry_loadhdrs(mep,lsp) ;
		    ll = rs ;
	        }
	        if (rs >= 0) {
	            if ((rs = mailmsgstage_gmsgenter(op,tfp,lsp,mep)) >= 0) {
		        ll = rs ;
	                if ((rs = vechand_add(op->mlp,mep)) >= 0) {
			    op->nmsgs += 1 ;
	                    mep = nullptr ;
			}
	            }
		}
		if (rs < 0) {
		    msgentry_finish(mep) ;
		}
	    } /* end if (msgentry_start) */
	    if ((rs < 0) && (mep != nullptr)) {
	        mailmsgstage_gmsgentdel(op,mep) ;
	        mep = nullptr ;
	    }
	} /* end if (mailmsgstage_gmsgentnew) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsgstage_gmsgent) */

static int mailmsgstage_gmsgenter(MMS *op,filebuf *tfp,fdliner *lsp,
		MSGENTRY *mep) noex {
	int		rs ;
	int		ll = 0 ;
	rs = msgentry_setoff(mep,op->tflen) ;
	if (rs >= 0) rs = msgentry_setflags(mep) ;
	if (rs >= 0) {
	    if ((rs = mailmsgstage_gmsgbody(op,tfp,lsp,mep)) >= 0) {
		ll = rs ;
	    } /* end if (mailmsgstage_gmsgbody) */
	} /* end if (insertion) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsgstage_gmsgenter) */

static int mailmsgstage_gmsgbody(MMS *op,filebuf *tfp,fdliner *lsp,
		MSGENTRY *mep) noex {
	int		rs = SR_OK ;
	int		max = INT_MAX ;
	int		blen = 0 ;
	int		blines = 0 ;
	int		clen = -1 ;
	int		clines = -1 ;
	int		ll = 0 ;
	int		f_env = false ;
	int		f_bol = true ;
	int		f_eol = false ;
	cchar		*lp ;

	if (rs >= 0) clen = msgentry_getclen(mep) ;
	if (rs >= 0) clines = msgentry_getclines(mep) ;

	if (op->f.useclines && mep->f.plaintext && (clines >= 0))
	    max = clines ;

	while ((rs >= 0) && (blines < max) && 
	    ((rs = fdliner_read(lsp,&lp)) > 0)) {

	    ll = rs ;
	    f_eol = (lp[ll - 1] == '\n') ;
	    if (f_bol && FMAT(lp) && (ll > 5)) {
	            MAILMSGMATENV	me ;
	            if ((rs = mailmsgmatenv(&me,lp,ll)) > 0) {
			f_env = true ;
		    }
	        if (f_env) break ;
	    }

	    if (rs >= 0) {
	        rs = filebuf_write(tfp,lp,ll) ;
	    }

	    if (rs < 0) break ;
	    blen += ll ;
	    if (f_eol) blines += 1 ;
	    f_bol = f_eol ;
	    ll = 0 ; /* signal possible EOF */
	    fdliner_done(lsp) ;
	} /* end while (searching for new start-msg) */

	if (rs >= 0) {
	    op->tflen += blen ;
	    msgentry_setlen(mep,blen) ;
	    if (clen < 0) msgentry_setclen(mep,blen) ;
	    if (clines < 0) msgentry_setclines(mep,blines) ;
	}

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsgstage_gmsgbody) */

static int mailmsgstage_gmsgentnew(MMS *op,MSGENTRY **mpp) noex {
	cint		esize = sizeof(MSGENTRY) ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	rs = uc_malloc(esize,mpp) ;

	return rs ;
}
/* end subroutine (mailmsgstage_gmsgentnew) */

static int mailmsgstage_gmsgentdel(MMS *op,MSGENTRY *mep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (mep == nullptr) return SR_FAULT ;

	rs1 = uc_free(mep) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (mailmsgstage_gmsgentdel) */

static int mailmsgstage_msgfins(MMS *op) noex {
	VECHAND		*mlp = op->mlp ;
	MSGENTRY	*mep ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; vechand_get(mlp,i,&mep) >= 0 ; i += 1) {
	    if (mep) {
		{
	            rs1 = msgentry_finish(mep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(mep) ;
	            if (rs >= 0) rs = rs1 ;
		    vechand_del(mlp,i--) ;
	 	}
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (mailmsgstage_msgfins) */

static int msgentry_start(MSGENTRY *mep) noex {
	int		rs = SR_FAULT ;
	if (mep) {
	    memclear(mep) ;
	    mep->clines = -1 ;
	    mep->clen = -1 ;
	    mep->blen = -1 ;
	    mep->boff = -1 ;
	    rs = mailmsg_start(&mep->m) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgentry_start) */

static int msgentry_finish(MSGENTRY *mep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mep) {
	    rs = SR_OK ;
	    mep->boff = -1 ;
	    mep->blen = -1 ;
	    {
	        rs1 = mailmsg_finish(&mep->m) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgentry_finish) */

static int msgentry_loadline(MSGENTRY *mep,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	if (ll > 0) {
	    rs = mailmsg_loadline(&mep->m,lp,ll) ;
	}
	return rs ;
}
/* end subroutine (msgentry_loadline) */

static int msgentry_loadhdrs(MSGENTRY *mep,fdliner *lsp) noex {
	int		rs ;
	int		ll ;
	int		tlen = 0 ;
	int		f_eoh = false ;
	cchar		*lp ;
	while ((rs = fdliner_read(lsp,&lp)) > 0) {
	    ll = rs ;
	    tlen += ll ;
	    f_eoh = (lp[0] == '\n') ;
	    if (! f_eoh) {
	        rs = mailmsg_loadline(&mep->m,lp,ll) ;
	    }
	    if (rs < 0) break ;
	    fdliner_done(lsp) ;
	    if (f_eoh) break ;
	} /* end while */
	mep->f.eoh = f_eoh ;
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (msgentry_loadhdrs) */

static int msgentry_getclen(MSGENTRY *mep) noex {
	int		rs = SR_OK ;
	int		clen ;
	cchar		*kn = HN_CLEN ;
	clen = mep->clen ;
	if (! mep->f.clen) {
	    mep->f.clen = true ; /* once-flag */
	    mep->clen = -1 ;
	    clen = mailentry_gethdrnum(mep,kn) ;
	    if (clen >= 0) {
	        mep->hdr.clen = true ;
	        mep->clen = clen ;
	    }
	} /* end if (only once) */

	return (rs >= 0) ? clen : rs ;
}
/* end subroutine (msgentry_getclen) */

static int msgentry_setclen(MSGENTRY *mep,int clen) noex {
	int		rs = SR_FAULT ;
	if (mep) {
	    rs = SR_OK ;
	    mep->f.clen = true ;
	    mep->clen = clen ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgentry_setclen) */

static int msgentry_setflags(MSGENTRY *mep) noex {
	int		rs ;
	if ((rs = msgentry_setct(mep)) >= 0) {
	    if ((rs = msgentry_setce(mep)) >= 0) {
	        mep->f.plaintext = mep->f.ctplain && mep->f.ceplain ;
	    }
	}
	return rs ;
}
/* end subroutine (msgentry_setflags) */

static int msgentry_setct(MSGENTRY *mep) noex {
	MHCOM		hc ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rs1 ;
	int		hl ;
	cchar		*hp ;
	mep->f.ctplain = true ;
	if ((hl = mailmsg_hdrval(&mep->m,HN_CTYPE,&hp)) > 0) {
	    mep->hdr.ctype = true ;
	    if ((rs = mhcom_start(&hc,hp,hl)) >= 0) {
		int	vl ;
		cchar	*vp ;
	        if ((vl = mhcom_getval(&hc,&vp)) > 0) {
	            if (cchar *tp ; (tp = strnchr(vp,vl,';')) != nullptr) {
	                vl = (tp - vp) ;
		    }
	            rs1 = sisub(vp,vl,"text") ;
	            if ((rs1 >= 0) && (strnchr(vp,vl,'/') != nullptr)) {
	                rs1 = sisub(vp,vl,"plain") ;
		    }
	            mep->f.ctplain = (rs1 >= 0) ;
	        } /* end if */
	        rs1 = mhcom_finish(&hc) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mhcom) */
	} /* end if (positive) */
	return rs ;
}
/* end subroutine (msgentry_setct) */

static int msgentry_setce(MSGENTRY *mep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		hl ;
	cchar		*hp ;
	mep->f.ceplain = true ;
	if ((hl = mailmsg_hdrval(&mep->m,HN_CENCODING,&hp)) > 0) {
	    COMPARSE	com ;
	    int		vl ;
	    cchar	*tp ;
	    cchar	*vp ;
	    mep->hdr.cencoding = true ;
	    if ((rs = comparse_start(&com,hp,hl)) >= 0) {
	        if ((vl = comparse_getval(&com,&vp)) > 0) {
	            if ((tp = strnchr(vp,vl,';')) != nullptr) {
	                vl = (tp - vp) ;
	            }
	            rs1 = sisub(vp,vl,"7bit") ;
	            if (rs1 < 0) {
	                rs1 = sisub(vp,vl,"8bit") ;
		    }
	            mep->f.ceplain = (rs1 >= 0) ;
	        } /* end if */
	        rs1 = comparse_finish(&com) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (comparse) */
	} /* end if (mailmsg_hdrval) */
	return rs ;
}
/* end subroutine (msgentry_setce) */

static int msgentry_getclines(MSGENTRY *mep) noex {
	int		rs = SR_OK ;
	int		clines = 0 ;
	if (! mep->f.clines) {
	    cchar		*kn = HN_CLINES ;
	    mep->f.clines = true ; /* once-flag */
	    mep->clines = -1 ;
	    if ((clines = mailentry_gethdrnum(mep,kn)) >= 0) {
	        mep->hdr.clines = true ;
	        mep->clines = clines ;
	    }
	} else {
	    clines = mep->clines ;
	}
	return (rs >= 0) ? clines : rs ;
}
/* end subroutine (msgentry_getclines) */

static int msgentry_setclines(MSGENTRY *mep,int clines) noex {
	mep->f.clines = true ;
	mep->clines = clines ;
	return SR_OK ;
}
/* end subroutine (msgentry_setclines) */

static int msgentry_setoff(MSGENTRY *mep,off_t boff) noex {
	mep->boff = boff ;
	return SR_OK ;
}
/* end subroutine (msgentry_setoff) */

static int msgentry_setlen(MSGENTRY *mep,int blen) noex {
	mep->blen = blen ;
	return SR_OK ;
}
/* end subroutine (msgentry_setoff) */

static int mailentry_gethdrnum(MSGENTRY *mep,cchar *kn) noex {
	mailmsg		*mmp = &mep->m ;
	int		rs = SR_NOTFOUND ;
	int		hl ;
	int		v = -1 ;
	cchar		*hp ;
	for (int i = 0 ; (hl = mailmsg_hdrival(mmp,kn,i,&hp)) >= 0 ; i += 1) {
	    if (hl > 0) {
	        rs = hdrextnum(hp,hl) ;
	        v = rs ;
	    }
	    if (rs >= 0) break ;
	} /* end for */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (msgentry_gethdrnum) */


