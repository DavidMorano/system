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
#include	<xstring>
#include	<usystem.h>
#include	<filebuf.h>
#include	<vechand.h>
#include	<mailmsghdrs.h>
#include	<mailmsgmatenv.h>
#include	<mailmsg.h>
#include	<char.h>
#include	<mhcom.h>
#include	<comparse.h>
#include	<localmisc.h>

#include	"mailmsgstage.h"


/* local defines */

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(2048,LINE_MAX)
#else
#define	LINEBUFLEN	2048
#endif
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

#define	MSGENTRY	struct msgentry
#define	MSGENTRY_FL	struct msgentry_flags

#define	LINER		struct liner


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	sisub(cchar *,int,cchar *) ;
extern int	mailmsgmathdr(cchar *,int,int *) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	hdrextnum(cchar *,int) ;
extern int	opentmpfile(cchar *,int,mode_t,char *) ;
extern int	hasuc(cchar *,int) ;
extern int	isprintlatin(int) ;
extern int	iceil(int,int) ;
extern int	hasEOH(cchar *,int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;
extern char	*timestr_edate(time_t,char *) ;
extern char	*timestr_hdate(time_t,char *) ;
extern char	*timestr_logz(time_t,char *) ;


/* external variables */


/* local structures */

struct liner {
	filebuf		mfb ;
	off_t	poff ;		/* file-offset previous */
	off_t	foff ;		/* file-offset current */
	int		to ;		/* read time-out */
	int		f_net ;		/* regular file */
	int		llen ;
	char		lbuf[LINEBUFLEN + 1] ;
} ;

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
	off_t	boff ;		/* w/ tmpfile */
	int		blen ;		/* w/ tmpfile */
	int		clen ;		/* supplied or calculated */
	int		clines ;	/* supplied or calculated */
} ;


/* forward references */

static int	mailmsgstage_starter(MAILMSGSTAGE *,int,int) ;
static int	mailmsgstage_g(MAILMSGSTAGE *,int,int) ;
static int	mailmsgstage_gmsg(MAILMSGSTAGE *,filebuf *,LINER *,int) ;
static int	mailmsgstage_gmsgbody(MAILMSGSTAGE *,filebuf *,LINER *,
			MSGENTRY *) ;
static int	mailmsgstage_gmsgent(MAILMSGSTAGE *,filebuf *,LINER *,
			cchar *,int,int) ;
static int	mailmsgstage_gmsgenter(MAILMSGSTAGE *,filebuf *,LINER *,
			MSGENTRY *) ;
static int	mailmsgstage_msgfins(MAILMSGSTAGE *) ;
static int	mailmsgstage_gmsgentnew(MAILMSGSTAGE *,MSGENTRY **) ;
static int	mailmsgstage_gmsgentdel(MAILMSGSTAGE *,MSGENTRY *) ;

static int	msgentry_start(MSGENTRY *) ;
static int	msgentry_finish(MSGENTRY *) ;
static int	msgentry_loadline(MSGENTRY *,cchar *,int) ;
static int	msgentry_loadhdrs(MSGENTRY *,LINER *) ;
static int	msgentry_setflags(MSGENTRY *) ;
static int	msgentry_setct(MSGENTRY *) ;
static int	msgentry_setce(MSGENTRY *) ;
static int	msgentry_getclines(MSGENTRY *) ;
static int	msgentry_setclines(MSGENTRY *,int) ;
static int	msgentry_getclen(MSGENTRY *) ;
static int	msgentry_setclen(MSGENTRY *,int) ;
static int	msgentry_setoff(MSGENTRY *,off_t) ;
static int	msgentry_setlen(MSGENTRY *,int) ;
static int	mailentry_gethdrnum(MSGENTRY *,cchar *) ;

static int liner_start(LINER *,int,off_t,int) ;
static int liner_finish(LINER *) ;
static int liner_readline(LINER *,cchar **) ;
static int liner_done(LINER *) ;


/* local variables */


/* exported subroutines */

int mailmsgstage_start(MAILMSGSTAGE *op,int ifd,int to,int opts)
{
	int		rs ;
	int		n = 0 ;
	cchar	*tmpdname = NULL ;
	char		template[MAXPATHLEN + 1] ;

	if (op == NULL) return SR_FAULT ;

	if (ifd < 0) return SR_BADF ;

	memset(op,0,sizeof(MAILMSGSTAGE)) ;

	op->tfd = -1 ;
	op->f.useclen = (opts & MAILMSGSTAGE_OUSECLEN) ? 1 : 0 ;
	op->f.useclines = (opts & MAILMSGSTAGE_OUSECLINES) ? 1 : 0 ;

	if (tmpdname == NULL) tmpdname = getenv(VARTMPDNAME) ;
	if (tmpdname == NULL) tmpdname = MAILMSGSTAGE_TMPDNAME ;

	if ((rs = mkpath2(template,tmpdname,"msXXXXXXXXXXXX")) >= 0) {
	    const mode_t	om = 0660 ;
	    cint		of = O_RDWR ;
	    char		tmpfname[MAXPATHLEN + 1] ;
	    if ((rs = opentmpfile(template,of,om,tmpfname)) >= 0) {
	        op->tfd = rs ;
	        if ((rs = uc_closeonexec(op->tfd,TRUE)) >= 0) {
	            cchar	*cp ;
	            if ((rs = uc_mallocstrw(tmpfname,-1,&cp)) >= 0) {
	                op->tmpfname = cp ;
	                rs = mailmsgstage_starter(op,ifd,to) ;
	                n = rs ;
	                if (rs < 0) {
	                    uc_free(op->tmpfname) ;
	                    op->tmpfname = NULL ;
	                }
	            } /* end if (m-a) */
	        } /* end if (uc_closeonexec) */
	        if (rs < 0) {
	            u_close(op->tfd) ;
	            op->tfd = -1 ;
	            if (tmpfname[0] != '\0') u_unlink(tmpfname) ;
	        }
	    } /* end if (opentmpfile) */
	} /* end if (mkpath2) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailmsgstage_start) */


static int mailmsgstage_starter(MAILMSGSTAGE *op,int ifd,int to)
{
	int		rs ;
	int 		vo = 0 ;
	int		n = 0 ;
	vo |= VECHAND_OCOMPACT ;
	vo |= VECHAND_OSTATIONARY ;
	if ((rs = vechand_start(&op->msgs,4,vo)) >= 0) {
	    if ((rs = mailmsgstage_g(op,ifd,to)) >= 0) {
	        n = rs ;
	        if (op->tflen > 0) {
	            size_t	ms = (size_t) op->tflen ;
	            int		mp = PROT_READ ;
	            int		mf = MAP_SHARED ;
	            int		fd = op->tfd ;
	            void	*md ;
	            if ((rs = u_mmap(NULL,ms,mp,mf,fd,0L,&md)) >= 0) {
	                op->mapdata = md ;
	                op->mapsize = ms ;
	            }
	        } /* end if */
	        if (rs >= 0) op->magic = MAILMSGSTAGE_MAGIC ;
	        if (rs < 0)
	            mailmsgstage_msgfins(op) ;
	    } /* end if */
	    if (rs < 0)
	        vechand_finish(&op->msgs) ;
	} /* end if (vechand) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailmsgstage_starter) */


int mailmsgstage_finish(MAILMSGSTAGE *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if (op->mapdata != NULL) {
	    rs1 = u_munmap(op->mapdata,op->mapsize) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = NULL ;
	    op->mapsize = 0 ;
	}

	rs1 = mailmsgstage_msgfins(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vechand_finish(&op->msgs) ;
	if (rs >= 0) rs = rs1 ;

	if (op->tfd >= 0) {
	    rs1 = u_close(op->tfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->tfd = -1 ;
	}

	if (op->tmpfname != NULL) {
	    if (op->tmpfname[0] != '\0') {
	        u_unlink(op->tmpfname) ;
	    }
	    rs1 = uc_free(op->tmpfname) ;
	    op->tmpfname = NULL ;
	    if (rs >= 0) rs = rs1 ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (mailmsgstage_finish) */


int mailmsgstage_count(MAILMSGSTAGE *op)
{
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	rs = vechand_count(&op->msgs) ;

	return rs ;
}
/* end subroutine (mailmsgstage_count) */


int mailmsgstage_clen(MAILMSGSTAGE *op,int mi)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = (mep->clen >= 0) ? mep->clen : SR_NOTFOUND ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_clen) */


int mailmsgstage_clines(MAILMSGSTAGE *op,int mi)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = (mep->clines >= 0) ? mep->clines : SR_NOTFOUND ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_clines) */


int mailmsgstage_envcount(MAILMSGSTAGE *op,int mi)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = mailmsg_envcount(&mep->m) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_envcount) */


int mailmsgstage_envaddress(MAILMSGSTAGE *op,int mi,int n,cchar **rpp)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = mailmsg_envaddress(&mep->m,n,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_envaddress) */


int mailmsgstage_envdate(MAILMSGSTAGE *op,int mi,int n,cchar **rpp)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = mailmsg_envdate(&mep->m,n,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_envdate) */


int mailmsgstage_envremote(MAILMSGSTAGE *op,int mi,int n,cchar **rpp)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = mailmsg_envremote(&mep->m,n,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_envremote) */


int mailmsgstage_hdrikey(MAILMSGSTAGE *op,int mi,int hi,cchar **rpp)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = mailmsg_hdrikey(&mep->m,hi,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdrikey) */


int mailmsgstage_hdrcount(MAILMSGSTAGE *op,int mi,cchar *name)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = mailmsg_hdrcount(&mep->m,name) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdrcount) */


int mailmsgstage_hdriline(MAILMSGSTAGE *op,int mi,cchar *name,int hi,int li,
		cchar **rpp)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = mailmsg_hdriline(&mep->m,name,hi,li,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdriline) */


int mailmsgstage_hdrival(MAILMSGSTAGE *op,int mi,cchar *name,int hi,cchar **rpp)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = mailmsg_hdrival(&mep->m,name,hi,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdrival) */


int mailmsgstage_hdrval(MAILMSGSTAGE *op,int mi,cchar *name,cchar **rpp)
{
	MSGENTRY	*mep ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	    rs = mailmsg_hdrval(&mep->m,name,rpp) ;
	}

	return rs ;
}
/* end subroutine (mailmsgstage_hdrval) */


int mailmsgstage_flags(MAILMSGSTAGE *op,int mi)
{
	MSGENTRY	*mep ;
	int		rs ;
	int		flags = 0 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
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


int mailmsgstage_bodyget(MAILMSGSTAGE *op,int mi,off_t boff,cchar **bpp)
{
	int		rs = SR_OK ;
	int		ml = 0 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MAILMSGSTAGE_MAGIC) return SR_NOTOPEN ;

	if (mi < 0) return SR_INVALID ;

	if (boff < 0) return SR_DOM ;

	if (bpp != NULL)
	    *bpp = NULL ;

	if (op->mapsize > 0) {
	    MSGENTRY	*mep ;
	    if ((rs = vechand_get(&op->msgs,mi,&mep)) >= 0) {
	        if (boff < mep->blen) {
	            off_t	moff = (mep->boff + boff) ;
	            ml = (mep->blen - boff) ;
	            if (bpp != NULL)
	                *bpp = (op->mapdata + moff) ;
	        }
	    }
	}

	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (mailmsgstage_bodyget) */


int mailmsgstage_bodyread(MAILMSGSTAGE *op,int mi,off_t boff,
		char *bbuf,int blen)
{
	int		rs ;
	int		ml = 0 ;
	cchar	*bp ;

	if (boff < 0) return SR_DOM ;

	if (bbuf == NULL) return SR_FAULT ;

	if (blen < 0) return SR_INVALID ;

	if ((rs = mailmsgstage_bodyget(op,mi,boff,&bp)) > 0) {
	    ml = MIN(rs,blen) ;
	    memcpy(bbuf,bp,ml) ;
	}

	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (mailmsgstage_bodyread) */


/* private subroutines */


/* gather the messages */
static int mailmsgstage_g(MAILMSGSTAGE *op,int ifd,int to)
{
	filebuf		tfb ;
	const off_t	ostart = 0L ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

	if ((rs = filebuf_start(&tfb,op->tfd,ostart,0,0)) >= 0) {
	    LINER	ls, *lsp = &ls ;

	    if ((rs = liner_start(lsp,ifd,ostart,to)) >= 0) {
		int	mi = 0 ;

	        while ((rs = mailmsgstage_gmsg(op,&tfb,lsp,mi++)) > 0) {

	        } /* end while */
	        n = op->nmsgs ;

	        rs1 = liner_finish(lsp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (liner) */

	    rs1 = filebuf_finish(&tfb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filebuf) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailmsgstage_g) */


/* parse out the headers of this message */
static int mailmsgstage_gmsg(MAILMSGSTAGE *op,filebuf *tfp,LINER *lsp,int mi)
{
	MAILMSGMATENV	me ;
	int		rs = SR_OK ;
	int		vi = -1 ;
	int		ll = 0 ;
	int		f_env = FALSE ;
	int		f_hdr = FALSE ;
	int		f_eoh = FALSE ;
	cchar	*lp ;

/* find message start */

	while ((rs = liner_readline(lsp,&lp)) > 0) {
	    ll = rs ;
	    if (ll == 0) break ;

	    if ((ll > 5) && FMAT(lp) &&
		((rs = mailmsgmatenv(&me,lp,ll)) > 0)) {
	        f_env = TRUE ;
	    } else if ((ll > 2) &&
	       ((rs = mailmsgmathdr(lp,ll,&vi)) > 0)) {
	        f_hdr = TRUE ;
	    } else if ((ll <= 2) && (mi == 0)) {
	        if ((lp[0] == '\n') || hasEOH(lp,ll)) {
	            f_eoh = TRUE ;
		}
	    }

	    if (rs < 0) break ;
	    if (f_env || f_hdr) break ;

	    ll = 0 ;
	    liner_done(lsp) ;

	    if (f_eoh) break ;
	} /* end while */

	if ((rs >= 0) && (f_eoh || f_env || f_hdr)) {
	    rs = mailmsgstage_gmsgent(op,tfp,lsp,lp,ll,f_eoh) ;
	    ll = rs ;
	}

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsgstage_gmsg) */


static int mailmsgstage_gmsgent(MAILMSGSTAGE *op,filebuf *tfp,LINER *lsp,
		cchar *lp,int ll,int f_eoh)
{
	MSGENTRY	*mep ;
	int		rs ;
	if ((rs = mailmsgstage_gmsgentnew(op,&mep)) >= 0) {
	    if ((rs = msgentry_start(mep)) >= 0) {
	        if ((! f_eoh) && (ll > 0)) {
	            rs = msgentry_loadline(mep,lp,ll) ;
		    ll = rs ;
	            f_eoh = (rs == 1) ;
	            liner_done(lsp) ;
	        }
	        if ((rs >= 0) && (! f_eoh)) {
	            rs = msgentry_loadhdrs(mep,lsp) ;
		    ll = rs ;
	        }
	        if (rs >= 0) {
	            if ((rs = mailmsgstage_gmsgenter(op,tfp,lsp,mep)) >= 0) {
		        ll = rs ;
	                if ((rs = vechand_add(&op->msgs,mep)) >= 0) {
			    op->nmsgs += 1 ;
	                    mep = NULL ;
			}
	            }
		}
		if (rs < 0)
		    msgentry_finish(mep) ;
	    } /* end if (msgentry_start) */
	    if ((rs < 0) && (mep != NULL)) {
	        mailmsgstage_gmsgentdel(op,mep) ;
	        mep = NULL ;
	    }
	} /* end if (mailmsgstage_gmsgentnew) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsgstage_gmsgent) */


static int mailmsgstage_gmsgenter(MAILMSGSTAGE *op,filebuf *tfp,LINER *lsp,
	MSGENTRY *mep)
{
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


static int mailmsgstage_gmsgbody(MAILMSGSTAGE *op,filebuf *tfp,LINER *lsp,
	MSGENTRY *mep)
{
	int		rs = SR_OK ;
	int		max = INT_MAX ;
	int		blen = 0 ;
	int		blines = 0 ;
	int		clen = -1 ;
	int		clines = -1 ;
	int		ll = 0 ;
	int		f_env = FALSE ;
	int		f_bol = TRUE ;
	int		f_eol = FALSE ;
	cchar		*lp ;

	if (rs >= 0) clen = msgentry_getclen(mep) ;
	if (rs >= 0) clines = msgentry_getclines(mep) ;

	if (op->f.useclines && mep->f.plaintext && (clines >= 0))
	    max = clines ;

	while ((rs >= 0) && (blines < max) && 
	    ((rs = liner_readline(lsp,&lp)) > 0)) {

	    ll = rs ;
	    f_eol = (lp[ll - 1] == '\n') ;
	    if (f_bol && FMAT(lp) && (ll > 5)) {
	            MAILMSGMATENV	me ;
	            if ((rs = mailmsgmatenv(&me,lp,ll)) > 0) {
			f_env = TRUE ;
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
	    liner_done(lsp) ;
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

static int mailmsgstage_gmsgentnew(MAILMSGSTAGE *op,MSGENTRY **mpp) noex {
	cint		esize = sizeof(MSGENTRY) ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	rs = uc_malloc(esize,mpp) ;

	return rs ;
}
/* end subroutine (mailmsgstage_gmsgentnew) */


static int mailmsgstage_gmsgentdel(MAILMSGSTAGE *op,MSGENTRY *mep)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;
	if (mep == NULL) return SR_FAULT ;

	rs1 = uc_free(mep) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (mailmsgstage_gmsgentdel) */


static int mailmsgstage_msgfins(MAILMSGSTAGE *op)
{
	VECHAND		*mlp = &op->msgs ;
	MSGENTRY	*mep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	for (i = 0 ; vechand_get(mlp,i,&mep) >= 0 ; i += 1) {
	    if (mep != NULL) {
	        rs1 = msgentry_finish(mep) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = uc_free(mep) ;
	        if (rs >= 0) rs = rs1 ;
		vechand_del(mlp,i--) ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (mailmsgstage_msgfins) */


static int msgentry_start(MSGENTRY *mep)
{
	int		rs ;

	memset(mep,0,sizeof(MSGENTRY)) ;
	mep->clines = -1 ;
	mep->clen = -1 ;
	mep->blen = -1 ;
	mep->boff = -1 ;
	rs = mailmsg_start(&mep->m) ;

	return rs ;
}
/* end subroutine (msgentry_start) */


static int msgentry_finish(MSGENTRY *mep)
{
	int		rs = SR_OK ;
	int		rs1 ;

	mep->boff = -1 ;
	mep->blen = -1 ;
	rs1 = mailmsg_finish(&mep->m) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (msgentry_finish) */


static int msgentry_loadline(MSGENTRY *mep,cchar *lp,int ll)
{
	int		rs = SR_OK ;

	if (ll > 0) {
	    rs = mailmsg_loadline(&mep->m,lp,ll) ;
	}

	return rs ;
}
/* end subroutine (msgentry_loadline) */


static int msgentry_loadhdrs(MSGENTRY *mep,LINER *lsp)
{
	int		rs ;
	int		ll ;
	int		tlen = 0 ;
	int		f_eoh = FALSE ;
	cchar	*lp ;

	while ((rs = liner_readline(lsp,&lp)) > 0) {
	    ll = rs ;

	    tlen += ll ;
	    f_eoh = (lp[0] == '\n') ;
	    if (! f_eoh) {
	        rs = mailmsg_loadline(&mep->m,lp,ll) ;
	    }

	    if (rs < 0) break ;

	    liner_done(lsp) ;
	    if (f_eoh) break ;
	} /* end while */

	mep->f.eoh = f_eoh ;

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (msgentry_loadhdrs) */


static int msgentry_getclen(MSGENTRY *mep)
{
	int		rs = SR_OK ;
	int		clen ;
	cchar	*kn = HN_CLEN ;

	clen = mep->clen ;
	if (! mep->f.clen) {
	    mep->f.clen = TRUE ; /* once-flag */
	    mep->clen = -1 ;
	    clen = mailentry_gethdrnum(mep,kn) ;
	    if (clen >= 0) {
	        mep->hdr.clen = TRUE ;
	        mep->clen = clen ;
	    }
	} /* end if (only once) */

	return (rs >= 0) ? clen : rs ;
}
/* end subroutine (msgentry_getclen) */


static int msgentry_setclen(MSGENTRY *mep,int clen)
{

	mep->f.clen = TRUE ;
	mep->clen = clen ;
	return SR_OK ;
}
/* end subroutine (msgentry_setclen) */


static int msgentry_setflags(MSGENTRY *mep)
{
	int		rs ;

	if ((rs = msgentry_setct(mep)) >= 0) {
	    if ((rs = msgentry_setce(mep)) >= 0) {
	        mep->f.plaintext = mep->f.ctplain && mep->f.ceplain ;
	    }
	}

	return rs ;
}
/* end subroutine (msgentry_setflags) */


static int msgentry_setct(MSGENTRY *mep)
{
	MHCOM		hc ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		hl, vl ;
	cchar	*tp ;
	cchar	*hp, *vp ;

	mep->f.ctplain = TRUE ;
	if ((hl = mailmsg_hdrval(&mep->m,HN_CTYPE,&hp)) > 0) {

	    mep->hdr.ctype = TRUE ;
	    if ((rs = mhcom_start(&hc,hp,hl)) >= 0) {

	        if ((vl = mhcom_getval(&hc,&vp)) > 0) {

	            if ((tp = strnchr(vp,vl,';')) != NULL)
	                vl = (tp - vp) ;

	            rs1 = sisub(vp,vl,"text") ;
	            if ((rs1 >= 0) && (strnchr(vp,vl,'/') != NULL))
	                rs1 = sisub(vp,vl,"plain") ;
	            mep->f.ctplain = (rs1 >= 0) ;

	        } /* end if */

	        mhcom_finish(&hc) ;
	    } /* end if (mhcom) */

	} /* end if (positive) */

	return rs ;
}
/* end subroutine (msgentry_setct) */


static int msgentry_setce(MSGENTRY *mep)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		hl ;
	cchar	*hp ;

	mep->f.ceplain = TRUE ;
	if ((hl = mailmsg_hdrval(&mep->m,HN_CENCODING,&hp)) > 0) {
	    COMPARSE	com ;
	    int		vl ;
	    cchar	*tp ;
	    cchar	*vp ;
	    mep->hdr.cencoding = TRUE ;
	    if ((rs = comparse_start(&com,hp,hl)) >= 0) {

	    if ((vl = comparse_getval(&com,&vp)) > 0) {

	        if ((tp = strnchr(vp,vl,';')) != NULL)
	            vl = (tp - vp) ;

	        rs1 = sisub(vp,vl,"7bit") ;
	        if (rs1 < 0)
	            rs1 = sisub(vp,vl,"8bit") ;
	        mep->f.ceplain = (rs1 >= 0) ;

	    } /* end if */

	        rs1 = comparse_finish(&com) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (comparse) */
	} /* end if (mailmsg_hdrval) */

	return rs ;
}
/* end subroutine (msgentry_setce) */


static int msgentry_getclines(MSGENTRY *mep)
{
	int		rs = SR_OK ;
	int		clines = 0 ;

	if (! mep->f.clines) {
	    cchar		*kn = HN_CLINES ;
	    mep->f.clines = TRUE ; /* once-flag */
	    mep->clines = -1 ;
	    if ((clines = mailentry_gethdrnum(mep,kn)) >= 0) {
	        mep->hdr.clines = TRUE ;
	        mep->clines = clines ;
	    }
	} else
	    clines = mep->clines ;

	return (rs >= 0) ? clines : rs ;
}
/* end subroutine (msgentry_getclines) */


static int msgentry_setclines(MSGENTRY *mep,int clines)
{

	mep->f.clines = TRUE ;
	mep->clines = clines ;
	return SR_OK ;
}
/* end subroutine (msgentry_setclines) */


static int msgentry_setoff(MSGENTRY *mep,off_t boff)
{

	mep->boff = boff ;
	return SR_OK ;
}
/* end subroutine (msgentry_setoff) */


static int msgentry_setlen(MSGENTRY *mep,int blen)
{

	mep->blen = blen ;
	return SR_OK ;
}
/* end subroutine (msgentry_setoff) */

static int mailentry_gethdrnum(MSGENTRY *mep,cchar *kn) noex {
	mailmsg		*mmp = &mep->m ;
{
	int		rs = SR_NOTFOUND ;
	int		hl ;
	int		v = -1 ;
	cchar	*hp ;

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

static int liner_start(LINER *lsp,int mfd,off_t foff,int to) noex {
	USTAT		sb ;
	int		rs ;

	if (mfd < 0)
	    return SR_BADF ;

	lsp->poff = 0 ;
	lsp->foff = foff ;
	lsp->to = to ;
	lsp->llen = -1 ;
	lsp->lbuf[0] = '\0' ;

	if ((rs = u_fstat(mfd,&sb)) >= 0) {
	    int		fbsize = 1024 ;
	    int		fbo = 0 ;
	    mode_t	m = sb.st_mode ;
	    lsp->f_net = S_ISCHR(m) || S_ISSOCK(m) ;
	    if (lsp->f_net) {
	        fbo |= filebuf_ONET ;
	    } else {
	        int	fs = ((sb.st_size == 0) ? 1 : (sb.st_size & INT_MAX)) ;
	        int	cs ;
	        cs = BCEIL(fs,512) ;
	        fbsize = MIN(cs,4096) ;
	    }
	    rs = filebuf_start(&lsp->mfb,mfd,foff,fbsize,fbo) ;
	} /* end if (stat) */

	return rs ;
}
/* end subroutine (liner_start) */


static int liner_finish(LINER *lsp)
{
	int		rs = SR_OK ;
	int		rs1 ;

	lsp->llen = 0 ;
	lsp->lbuf[0] = '\0' ;
	rs1 = filebuf_finish(&lsp->mfb) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (liner_finish) */


static int liner_readline(LINER *lsp,cchar **lpp)
{
	filebuf		*fbp = &lsp->mfb ;
	int		rs = SR_OK ;

	if (lsp->llen < 0) {

	    lsp->poff = lsp->foff ;
	    rs = filebuf_readln(fbp,lsp->lbuf,LINEBUFLEN,lsp->to) ;
	    lsp->llen = rs ;

	    if (rs > 0) {
	        lsp->foff += lsp->llen ;
	    }

	} /* end if (needed a new line) */

	if (lpp) {
	    *lpp = (rs >= 0) ? lsp->lbuf : NULL ;
	}

	return (rs >= 0) ? lsp->llen : rs ;
}
/* end subroutine (liner_readline) */

static int liner_done(LINER *lsp) noex {
	lsp->llen = -1 ;
	lsp->lbuf[0] = '\0' ;
	return SR_OK ;
}
/* end subroutine (liner_done) */


