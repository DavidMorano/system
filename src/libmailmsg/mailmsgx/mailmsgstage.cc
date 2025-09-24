/* mailmsgstage SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process the input messages and spool them up */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mailmsgstage

	Description:
	This object module processes one or more mail messages (in
	appropriate mailbox format if more than one) from a file
	descriptor passed in the 'init' call-method.  The input
	file descriptor can be a pipe as a standard mailbox file
	is not required (one of the whole reasons for this object
	as opposed to the MAILBOX object).  Any mail messages found
	are effectively partially parsed and the object is then
	ready for queries by additional method calls.  The parsed
	mail meta-data is stored in a manner such that it is optimized
	for repeated access.

	Use should proceed roughly as:

		if ((rs = mailmsgstage_start()) >= 0) {

		    mailmsgstage_xxxx() 

		    mailmegstage_finish()
		} end if (mailmsgstage)

	This object is pretty fast also!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<sys/time.h>		/* for |gethrtime(3c)| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memcpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<filer.h>
#include	<fdliner.h>
#include	<vechand.h>
#include	<comparse.h>
#include	<mhcom.h>
#include	<strn.h>
#include	<six.h>
#include	<mkpathx.h>
#include	<hdrextnum.h>
#include	<char.h>
#include	<hasx.h>
#include	<mailmsghdrs.h>
#include	<mailmsgmatenv.h>
#include	<mailmsgmathdr.h>
#include	<mailmsg.h>
#include	<msgentry.h>
#include	<opentmp.h>
#include	<localmisc.h>

#include	"mailmsgstage.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |getlenstr(3u) */
import uconstants ;

/* local defines */

#ifndef	HDRNAMELEN
#define	HDRNAMELEN	80
#endif

#define	FMAT(cp)	((cp)[0] == 'F')

#ifndef	HN_XMAILER
#define	HN_XMAILER	"x-mailer"
#endif

#ifndef	HN_RECEIVED
#define	HN_RECEIVED	"received"
#endif

#define	MMS		mailmsgstage


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef msgentry *	msgentryp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int mailmsgstage_magic(MMS *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MAILMSGSTAGE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailmsgstage_magic) */

static int	mailmsgstage_starts(MMS *,int,cc *) noex ;
static int	mailmsgstage_starter(MMS *,int) noex ;
static int	mailmsgstage_g(MMS *,int) noex ;
static int	mailmsgstage_gmsg(MMS *,filer *,fdliner *,int) noex ;
static int	mailmsgstage_gmsgbody(MMS *,filer *,fdliner *,
			msgentry *) noex ;
static int	mailmsgstage_gmsgent(MMS *,filer *,fdliner *,
			cchar *,int,int) noex ;
static int	mailmsgstage_gmsgenter(MMS *,filer *,fdliner *,
			msgentry *) noex ;
static int	mailmsgstage_msgfins(MMS *) noex ;
static int	mailmsgstage_gmsgentnew(MMS *,msgentry **) noex ;
static int	mailmsgstage_gmsgentdel(MMS *,msgentry *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsgstage_start(MMS *op,int ifd,int to,int mmo) noex {
    	MAILMSGSTAGE	*hop = op ;
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (op) {
	    cnullptr	np{} ;
	    rs = SR_BADF ;
	    memclear(hop) ;
	    if (ifd >= 0) {
		cint	osz = szof(vechand) ;
	        cchar	*tmpdn = nullptr ;
		void	*vp{} ;
	        op->tfd = -1 ;
		op->to = to ;
	        op->fl.useclen = bool(mmo & MAILMSGSTAGE_OUSECLEN) ;
	        op->fl.useclines = bool(mmo & MAILMSGSTAGE_OUSECLINES) ;
	        if (tmpdn == np) {
		    static cchar *vap = getenv(varname.tmpdir) ;
		    tmpdn = vap ;
		}
	        if (tmpdn == np) tmpdn = MAILMSGSTAGE_TMPDNAME ;
		if ((rs = libmem.mall(osz,&vp)) >= 0) {
		    op->mlp = (vechand *) vp ;
	            if ((rs = mailmsgstage_starts(op,ifd,tmpdn)) >= 0) {
			n = rs ;
		    } /* end if (mailmsgstage_starts) */
		    if (rs < 0) {
			libmem.free(op->mlp) ;
			op->mlp = nullptr ;
		    } /* end if (error) */
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
	if (char *tpat ; (rs = malloc_mp(&tpat)) >= 0) {
	    cchar	*xpat = "msXXXXXXXXXXXX" ;
	    if ((rs = mkpath2(tpat,tmpdn,xpat)) >= 0) {
	        cint	of = O_RDWR ;
	        cmode	om = 0660 ;
	        if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	            if ((rs = opentmpfile(tpat,of,om,tbuf)) >= 0) {
	                op->tfd = rs ;
	                if ((rs = uc_closeonexec(op->tfd,true)) >= 0) {
	                    cchar	*cp ;
	                    if ((rs = libmem.strw(tbuf,-1,&cp)) >= 0) {
	                        op->tmpfname = cp ;
			        {
	                            rs = mailmsgstage_starter(op,ifd) ;
	                            nmsgs = rs ;
			        }
	                        if (rs < 0) {
				    void *vp = voidp(op->tmpfname) ;
	                            libmem.free(vp) ;
	                            op->tmpfname = nullptr ;
	                        }
	                    } /* end if (memory-allocation) */
	                } /* end if (uc_closeonexec) */
	                if (rs < 0) {
	                    u_close(op->tfd) ;
	                    op->tfd = -1 ;
	                    if (tbuf[0] != '\0') {
			        u_unlink(tbuf) ;
			        tbuf[0] = '\0' ;
		            }
	                } /* end if (error) */
	            } /* end if (opentmpfile) */
		    rs1 = malloc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (mkpath2) */
	    rs1 = malloc_free(tpat) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? nmsgs : rs ;
}
/* end subroutine (mailmsgstage_starts) */

static int mailmsgstage_starter(MMS *op,int ifd) noex {
	cint		vn = 4 ;
	int		rs ;
	int 		vo = 0 ;
	int		n = 0 ;
	vo |= VECHAND_OCOMPACT ;
	vo |= VECHAND_OSTATIONARY ;
	if ((rs = vechand_start(op->mlp,vn,vo)) >= 0) {
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
	        if (rs >= 0) {
		    op->magic = MAILMSGSTAGE_MAGIC ;
		}
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
	int		rs ;
	int		rs1 ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (op->mapdata) {
		void	*md = op->mapdata ;
		csize	ms = op->mapsize ;
	        rs1 = u_mmapend(md,ms) ;
	        if (rs >= 0) rs = rs1 ;
	        op->mapdata = nullptr ;
	        op->mapsize = 0 ;
	    }
	    {
	        rs1 = mailmsgstage_msgfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vechand_finish(op->mlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->tfd >= 0) {
	        rs1 = u_close(op->tfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->tfd = -1 ;
	    }
	    if (op->tmpfname) {
	        if (op->tmpfname[0] != '\0') {
	            u_unlink(op->tmpfname) ;
	        }
		void *vp = voidp(op->tmpfname) ;
	        rs1 = libmem.free(vp) ;
	        op->tmpfname = nullptr ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mlp) {
	        rs1 = libmem.free(op->mlp) ;
	        if (rs >= 0) rs = rs1 ;
		op->mlp = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_finish) */

int mailmsgstage_count(MMS *op) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    rs = vechand_count(op->mlp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_count) */

int mailmsgstage_clen(MMS *op,int mi) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = (mep->clen >= 0) ? mep->clen : SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_clen) */

int mailmsgstage_clines(MMS *op,int mi) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = (mep->clines >= 0) ? mep->clines : SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_clines) */

int mailmsgstage_envcount(MMS *op,int mi) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = mailmsg_envcount(&mep->m) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_envcount) */

int mailmsgstage_envaddress(MMS *op,int mi,int n,cchar **rpp) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = mailmsg_envaddress(&mep->m,n,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_envaddress) */

int mailmsgstage_envdate(MMS *op,int mi,int n,cchar **rpp) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = mailmsg_envdate(&mep->m,n,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_envdate) */

int mailmsgstage_envremote(MMS *op,int mi,int n,cchar **rpp) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = mailmsg_envremote(&mep->m,n,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_envremote) */

int mailmsgstage_hdrikey(MMS *op,int mi,int hi,cchar **rpp) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = mailmsg_hdrikey(&mep->m,hi,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_hdrikey) */

int mailmsgstage_hdrcount(MMS *op,int mi,cchar *name) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = mailmsg_hdrcount(&mep->m,name) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_hdrcount) */

int mailmsgstage_hdriline(MMS *op,int mi,cc *name,int hi,int li,cc **rpp) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = mailmsg_hdriline(&mep->m,name,hi,li,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_hdriline) */

int mailmsgstage_hdrival(MMS *op,int mi,cchar *name,
		int hi,cchar **rpp) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    void	*vp{} ;
	    if ((rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = mailmsg_hdrival(&mep->m,name,hi,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_hdrival) */

int mailmsgstage_hdrval(MMS *op,int mi,cchar *name,cchar **rpp) noex {
	int		rs ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        rs = mailmsg_hdrval(&mep->m,name,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgstage_hdrval) */

int mailmsgstage_getfl(MMS *op,int mi) noex {
	int		rs ;
	int		flags = 0 ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    if (void *vp{} ; (rs = vechand_get(op->mlp,mi,&vp)) >= 0) {
	        msgentry	*mep = msgentryp(vp) ;
	        flags |= ((mep->hdr.clen) ? MAILMSGSTAGE_MCLEN : 0) ;
	        flags |= ((mep->hdr.clines) ? MAILMSGSTAGE_MCLINES : 0) ;
	        flags |= ((mep->hdr.ctype) ? MAILMSGSTAGE_MCTYPE : 0) ;
	        flags |= ((mep->hdr.cencoding) ? MAILMSGSTAGE_MCENCODING : 0) ;
	        flags |= ((mep->fl.ctplain) ? MAILMSGSTAGE_MCTPLAIN : 0) ;
	        flags |= ((mep->fl.ceplain) ? MAILMSGSTAGE_MCEPLAIN : 0) ;
	        flags |= ((mep->fl.plaintext) ? MAILMSGSTAGE_MCPLAIN : 0) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? flags : rs ;
}
/* end subroutine (mailmsgstage_getfl) */

int mailmsgstage_bodyget(MMS *op,int mi,off_t boff,cchar **bpp) noex {
	int		rs = SR_OK ;
	int		ml = 0 ;
	if ((rs = mailmsgstage_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (mi >= 0) {
		rs = SR_DOM ;
	        if (boff >= 0) {
		    rs = SR_OK ;
	            if (bpp) *bpp = nullptr ;
	            if (op->mapsize > 0) {
			vechand	*mlp = op->mlp ;
		        if (void *vp{} ; (rs = mlp->get(mi,&vp)) >= 0) {
	                    msgentry	*mep = msgentryp(vp) ;
	                    if (boff < mep->blen) {
	                        off_t	moff = (mep->boff + boff) ;
	                        ml = intconv(mep->blen - boff) ;
	                        if (bpp) {
	                            *bpp = (op->mapdata + moff) ;
				}
	                    }
	    		}
		    } /* end if */
	        } /* end if (domain) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (mailmsgstage_bodyget) */

int mailmsgstage_bodyread(MMS *op,int mi,off_t boff,
		char *bbuf,int blen) noex {
	int		rs ;
	int		ml = 0 ;
	if ((rs = mailmsgstage_magic(op,bbuf)) >= 0) {
	    rs = SR_DOM ;
	    if (boff >= 0) {
		rs = SR_INVALID ;
	        if (blen >= 0) {
	            cchar	*bp ;
	            if ((rs = mailmsgstage_bodyget(op,mi,boff,&bp)) > 0) {
	                ml = min(rs,blen) ;
	                memcpy(bbuf,bp,ml) ;
	            }
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (mailmsgstage_bodyread) */


/* private subroutines */

static int mailmsgstage_g(MMS *op,int ifd) noex {
	coff		ostart = 0z ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (filer tfb ; (rs = tfb.start(op->tfd,ostart,0,0)) >= 0) {
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
	    rs1 = tfb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailmsgstage_g) */

/* parse out the headers of this message */
static int mailmsgstage_gmsg(MMS *op,filer *tfp,
		fdliner *lsp,int mi) noex {
	mmenvdat	me ;
	int		rs = SR_OK ;
	int		vi = -1 ;
	int		ll = 0 ;
	bool		f_env = false ;
	bool		f_hdr = false ;
	bool		f_eoh = false ;
	cchar		*lp ;
	/* find message start */
	while ((rs = fdliner_getln(lsp,&lp)) > 0) {
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

static int mailmsgstage_gmsgent(MMS *op,filer *tfp,fdliner *lsp,
		cchar *lp,int ll,int f_eoh) noex {
	msgentry	*mep ;
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

static int mailmsgstage_gmsgenter(MMS *op,filer *tfp,fdliner *lsp,
		msgentry *mep) noex {
	int		rs ;
	int		ll = 0 ;
	if ((rs = msgentry_setoff(mep,op->tflen)) >= 0) {
	    if ((rs = msgentry_setflags(mep)) >= 0) {
	        if ((rs = mailmsgstage_gmsgbody(op,tfp,lsp,mep)) >= 0) {
		    ll = rs ;
	        } /* end if (mailmsgstage_gmsgbody) */
	    }
	} /* end if (insertion) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsgstage_gmsgenter) */

static int mailmsgstage_gmsgbody(MMS *op,filer *tfp,fdliner *lsp,
		msgentry *mep) noex {
	int		rs = SR_OK ;
	int		nmax = INT_MAX ;
	int		blen = 0 ;
	int		blines = 0 ;
	int		clen = -1 ;
	int		clines = -1 ;
	int		ll = 0 ;
	bool		f_env = false ;
	bool		f_bol = true ;
	bool		f_eol = false ;
	cchar		*lp ;
	if (rs >= 0) clen = msgentry_getclen(mep) ;
	if (rs >= 0) clines = msgentry_getclines(mep) ;
	if (op->fl.useclines && mep->fl.plaintext && (clines >= 0)) {
	    nmax = clines ;
	}
	while ((rs >= 0) && (blines < nmax) && 
	    ((rs = fdliner_getln(lsp,&lp)) > 0)) {
	    ll = rs ;
	    f_eol = (lp[ll - 1] == '\n') ;
	    if (f_bol && FMAT(lp) && (ll > 5)) {
	            mmenvdat	me ;
	            if ((rs = mailmsgmatenv(&me,lp,ll)) > 0) {
			f_env = true ;
		    }
	        if (f_env) break ;
	    }
	    if (rs >= 0) {
	        rs = filer_write(tfp,lp,ll) ;
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

static int mailmsgstage_gmsgentnew(MMS *op,msgentry **mpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	esz = szof(msgentry) ;
	    rs = libmem.mall(esz,mpp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgstage_gmsgentnew) */

static int mailmsgstage_gmsgentdel(MMS *op,msgentry *mep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && mep) {
	    rs = SR_OK ;
	    {
	        rs1 = libmem.free(mep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgstage_gmsgentdel) */

static int mailmsgstage_msgfins(MMS *op) noex {
	vechand		*mlp = op->mlp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(mlp,i,&vp) >= 0 ; i += 1) {
	    msgentry	*mep = msgentryp(vp) ;
	    if (vp) {
		{
	            rs1 = msgentry_finish(mep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = libmem.free(mep) ;
	            if (rs >= 0) rs = rs1 ;
		    vechand_del(mlp,i--) ;
	 	}
	    } /* end if (non-null) */
	} /* end for */
	return rs ;
}
/* end subroutine (mailmsgstage_msgfins) */


