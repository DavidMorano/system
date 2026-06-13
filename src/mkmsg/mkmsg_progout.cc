/* mkmsg_progout SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support building a message without output related subroutines */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time debug print-outs */
#define	CF_OUTVALUE	1		/* use 'outvalue()' */

/* revision history:

	= 1998-03-01, David A­D­ Morano
        The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mksmg_progout

	Description:
	Output a header.

	Synopsis:
	int progouthead(PI *pip,bfile *ofp,cchar *name,cchar *vp,int vl) noex

	Arguments:
	pip
	ofp
	name
	vp
	vl

	Description:
	The 'progoutheadema' subroutine is used to print out (write
	to the mail message file being built) an E-Mail Address
	(EMA).

	Synopsis:
	int progoutheadema(PI *pip,bfile *ofp,cchar *name,ema *ap) noex

	Arguments:
	pip
	ofp
	name
	ap

	Description:
	The 'progoutpart' subroutine is used to print out a email
	body part.

	Synopsis:
	int progoutpart(PI *pip,bfile *ofp,int f_multi,msgboundary,ep)
	struct proginf	*pip ;
	bfile		*ofp ;
	int		f_multi ;
	char		msgboundary[] ;
	MAILMSGATTENT	*ep ;

	Note: 
	Just a little diatribe rant on how stupid the MIME standard
	is for hanlding plain text.  In short, it was the most
	stupid and idiotic idea ever to make all mailers make plain
	text to end in <cr><nl> character pairs!  Mail was essentially
	invented on UNIX® systems and everybody knows (right well)
	that lines are ended with a single <nl> character!  The
	MIME standard really made a big mistake with that <cr><nl>
	crap!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */
#include	<ema.h>			/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<base64.h>		/* LIBUC */
#include	<linefold.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DPRINTF(3debug)| */

#include	"mkmsg_config.h"
#include	"defs.h"
#include	"mailmsgatt.h"
#include	"contentencodings.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	MAILMSGLINELEN
#define	MAILMSGLINELEN	76
#endif

#ifndef	MAILTEXTCOLS
#define	MAILTEXTCOLS	998
#endif

#define	BASE64LINELEN	72

#define	BASE64BUFLEN	((BASE64LINELEN / 4) * 3)

#ifndef	FROM_ESCAPE
#define	FROM_ESCAPE	'\b'
#endif

#ifndef	BUFLEN
#define	BUFLEN		MAX(BASE64LINELEN,LINEBUFLEN)
#endif

#define	BIGLINEBUFLEN	4096

#ifndef	PI
#define	PI		proginfo
#endif

#define	BF		bfile
#define	MMAE		mailmsgattent
#define	OL		outline


/* external subroutines */

extern int	buffer_stropaque(buffer *,cchar *,int) noex ;


/* external variables */


/* external variables */


/* local structures */

struct outline {
	int		maxlen ;
	int		rlen ;
} ; /* end struct (outline) */


/* forward references */

local int	outct(PI *,BF *,MMAE *) noex ;
local int	outentry(PI *,BF *,OL *,buffer *,ema_ent *) noex ;
local int	outvalue(PI *,BF *,OL *,cchar *,int) noex ;
local int	outpartbody(PI *,BF *,bfile *,MMAE *) noex ;
local int	outpartbodybits(PI *,BF *,bfile *,MMAE *) noex ;
local int	outbase64(PI *,BF *,cchar *,int) noex ;

local int	outline_start(OL *,int,int) noex ;
local int	outline_finish(OL *) noex ;

local int	strestlen(cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

/* output a general header */
int progouthead(PI *pip,BF *ofp,cchar *name,cchar *vp,int vl) noex {
	OL		ld ;
	cint		llen = MAILMSGLINELEN ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if (ofp == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;
	if (vp == nullptr) return SR_FAULT ;

	if (name[0] == '\0') return SR_INVALID ;

	if (vl < 0)
	    vl = lenstr(vp) ;

	if ((rs = outline_start(&ld,llen,llen)) >= 0) {

	    if (rs >= 0) {
	        rs = bprintf(ofp,"%s: ",name) ;
	        wlen += rs ;
	        ld.rlen -= rs ;
	    }

	    if (rs >= 0) {
	        rs = outvalue(pip,ofp,&ld,vp,vl) ;
	        wlen += rs ;
	    }

	    if (rs >= 0) {
	        rs = bputc(ofp,'\n') ;
	        wlen += rs ;
	    }

	    rs1 = outline_finish(&ld) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (outline) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (progouthead) */

/* output a header that comtains one or more EMAs */
int progoutheadema(PI *pip,BF *ofp,cchar *name,EMA *ap) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progout/_headema: ent name=%s\n",name) ;
#endif

	if (ofp == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;

	if (name[0] == '\0') return SR_INVALID ;

	if (ap != nullptr) {
	    buffer	b ;
	    if ((rs = buffer_start(&b,80)) >= 0) {
	        if ((rs = ema_count(ap)) > 0) {
	            OL	ld ;
	            cint	llen = MAILMSGLINELEN ;
	            cint	n = rs ;
	            if ((rs = outline_start(&ld,llen,llen)) >= 0) {
	                int	c = 0 ;

	                if (rs >= 0) {
	                    rs = bprintf(ofp,"%s: ",name) ;
	                    wlen += rs ;
	                    ld.rlen -= rs ;
	                }

	                if (rs >= 0) {
	                    ema_ent	*ep ;
	                    cchar	*fmt ;
	                    int		nlen ;
	                    int		elen ;
	                    int		i ;
	                    int		f_linestart = false ;
	                    for (i = 0 ; ema_get(ap,i,&ep) >= 0 ; i += 1) {
	                        if (ep != nullptr) {

	                        elen = strestlen(ep->op,ep->ol) ;

/* calculate how much space (columns) need for this EMA */

	                        nlen = (f_linestart) ? (elen+2) : elen ;
	                        if ((c + 1) < n) nlen += 1 ;

/* see if it will fit in remaining available space */

	                        if (nlen > ld.rlen) {
	                            fmt = (f_linestart) ? ",\n " : "\n " ;
	                            rs = bwrite(ofp,fmt,lenstr(fmt)) ;
	                            wlen += rs ;
	                            f_linestart = false ;
	                            ld.rlen = (ld.maxlen - 1) ;
	                        }

	                        if ((rs >= 0) && f_linestart) {
	                            rs = bwrite(ofp,", ",2) ;
	                            wlen += rs ;
	                            ld.rlen -= rs ;
	                        } /* end if */

	                        if (rs >= 0) {

#if	CF_OUTVALUE
	                            rs = outentry(pip,ofp,&ld,&b,ep) ;
	                            wlen += rs ;
#else
	                            rs = bwrite(ofp,ep->op,ep->ol) ;
	                            wlen += rs ;
	                            ld.rlen -= rs ;
#endif /* CF_OUTVALUE */

	                        } /* end if */

	                        f_linestart = true ;
	                        c += 1 ;

				}
	                        if (rs < 0) break ;
	                    } /* end for */
	                } /* end if (ok) */

	                if (rs >= 0) {
	                    rs = bprintf(ofp,"\n") ;
	                    wlen += rs ;
	                }

	                rs1 = outline_finish(&ld) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (outline) */
	        } /* end if (ema-count) */
	        rs1 = buffer_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (buffer) */
	} /* end if (non-null) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (progoutheadema) */

/* put out a mail body part */
int progoutpart(PI *pip,BF *ofp,int f_multi,cc *msgboundary,MMAE *ep) noex {
	BF		infile, *ifp = &infile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar	*fn ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5)) {
	    debugprintf("progoutpart: ent\n") ;
	    debugprintf("progoutpart: f_multi=%u\n",f_multi) ;
	    debugprintf("progoutpart: type=%s\n",ep->type) ;
	    debugprintf("progoutpart: subtype=%s\n",ep->subtype) ;
	}
#endif

	fn = (ep->auxfname != nullptr) ? ep->auxfname : ep->attfname ;
	if ((fn == nullptr) || (fn[0] == '\0') || (fn[0] == '-'))
	    fn = BFILE_STDIN ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progoutpart: fn=%s\n",fn) ;
#endif

	if ((rs = bopen(ifp,fn,"r",0666)) >= 0) {
	    cint	cte = ep->cte ;
	    cint	f_pt = ep->f_plaintext ;
	    int		f_fname ;
	    int		f_enc = false ;
	    cchar	*dn = "/dev/fd/" ;
	    cchar	*enc = ep->encoding ;
	    cchar	*kn ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("progoutpart: f_pt=%u\n",f_pt) ;
#endif

/* start printing */

	    if (f_multi) {
	        rs = bprintf(ofp,"--%s\n",msgboundary) ;
	        wlen += rs ;
	    }

/* content description */

	    if ((rs >= 0) && (ep->description != nullptr)) {
	        cchar	*desc = ep->description ;
	        kn = "content-description" ;
	        rs = bprintf(ofp,"%s: %s\n",kn,desc) ;
	        wlen += rs ;
	    }

/* content type */

	    f_fname = true ;
	    f_fname = f_fname && (ep->attfname != nullptr) ;
	    f_fname = f_fname && (ep->attfname[0] != '\0') ;
	    f_fname = f_fname && (ep->attfname[0] != '-') ;
	    f_fname = f_fname && (strncmp(ep->attfname,dn,8) != 0) ;

	    if (rs >= 0) {
	        cint	f0 = (ep->cte >= CE_7BIT) ;
	        cint	f1 = (ep->subtype != nullptr) ;
	        if (f_multi || f_fname || (! f_pt) || f0 || f1) {
	            f_enc = true ;
	            rs = outct(pip,ofp,ep) ;
	            wlen += rs ;
	        }
	    } /* end if (content type) */

/* content disposition */

	    if ((rs >= 0) && f_multi && pip->fl.dis_inline) {
	        cchar	*val = "inline" ;
	        kn = "content-disposition" ;
	        rs = bprintf(ofp, "%s: %s\n",kn,val) ;
	        wlen += rs ;
	    } /* end if (content-disposition) */

/* content transfer encoding */

	    if ((rs >= 0) && ((enc && f_enc) || (cte >= CE_7BIT))) {

	        if (enc != nullptr) {
	            kn = "content-transfer-encoding" ;
	            rs = bprintf(ofp,"%s: %s\n",kn,enc) ;
	            wlen += rs ;
	        }

	        if ((rs >= 0) && (ep->cte == CE_BINARY)) {

	            if (ep->clen < 0) {
	                ustat	sb ;
	                if ((rs1 = bcontrol(ifp,BC_STAT,&sb)) >= 0) {
	                    if (S_ISREG(sb.st_mode)) {
	                        ep->clen = (int) sb.st_size ;
	                    }
	                }
	            } /* end if (try to get CLEN) */

	            if ((rs >= 0) && (ep->clen >= 0)) {
	                kn = "content-length" ;
	                rs = bprintf(ofp, "%s: %u\n",kn,ep->clen) ;
	                wlen += rs ;
	            }

	        } /* end if (binary content) */

	    } /* end if (not '7bit') */

/* content lines */

	    if ((rs >= 0) && f_pt) {
	        if (ep->clines >= 0) {
	            kn = "content-lines" ;
	            rs = bprintf(ofp,"%s: %d\n",kn,ep->clines) ;
	            wlen += rs ;
	        }
	    } /* end if (content lines) */

/* end-of-headers */

	    if (rs >= 0) {
	        rs = bputc(ofp,'\n') ;
	        wlen += rs ;
	    }

/* message part body */

	    if (rs >= 0) {
	        rs = outpartbody(pip,ofp,ifp,ep) ;
	        wlen += rs ;
	    }

	    rs1 = bclose(ifp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progoutpart: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (progoutpart) */


/* local subroutines */

local int outentry(PI *pip,BF *ofp,OL *ldp,buffer *bufp,ema_ent *ep) noex {
	int		rs ;
	int		wlen = 0 ;

	if ((rs = buffer_reset(bufp)) >= 0) {
	    cchar	*bp ;
	    int		bl ;
	    int		c = 0 ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("outentry: a=>%r<\n",
	            ep->ap,strlinelen(ep->ap,ep->al,40)) ;
#endif

	    if ((rs >= 0) && (ep->ap != nullptr) && (ep->al > 0)) {
	        if (c++ > 0) rs = buffer_chr(bufp,CH_SP) ;
	        if (rs >= 0)
	            rs = buffer_stropaque(bufp,ep->ap,ep->al) ;
	    }

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("outentry: r=>%r<\n",
	            ep->rp,strlinelen(ep->rp,ep->rl,40)) ;
#endif

	    if ((rs >= 0) && (ep->rp != nullptr) && (ep->rl > 0)) {
	        if (c++ > 0) rs = buffer_chr(bufp,CH_SP) ;
	        if (rs >= 0)
	            rs = buffer_chr(bufp,CH_LANGLE) ;
	        if (rs >= 0)
	            rs = buffer_stropaque(bufp,ep->rp,ep->rl) ;
	        if (rs >= 0)
	            rs = buffer_chr(bufp,CH_RANGLE) ;
	    }
	    DEBUGPRINTF("c=>%r<\n",ep->cp,strlinelen(ep->cp,ep->cl,40)) ;
	    if ((rs >= 0) && (ep->cp != nullptr) && (ep->cl > 0)) {
	        if (c++ > 0) rs = buffer_chr(bufp,CH_SP) ;
	        if (rs >= 0) {
	            rs = buffer_chr(bufp,CH_LPAREN) ;
		}
	        if (rs >= 0) {
	            rs = buffer_strw(bufp,ep->cp,ep->cl) ;
		}
	        if (rs >= 0) {
	            rs = buffer_chr(bufp,CH_RPAREN) ;
		}
	    }
	    if ((rs = buffer_get(bufp,&bp)) > 0) {
	        bl = rs ;
	        rs = outvalue(pip,ofp,ldp,bp,bl) ;
	        wlen += rs ;
	    }
	} /* end if (buffer-reset) */
	DEBUGPRINTF("ret rs=%d wlen=%u\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (outentry) */

/* output a single value for a header (folding lines as needed) */
local int outvalue(PI *pip,BF *ofp,OL *ldp,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (pip == nullptr) return SR_FAULT ;

	if (ldp == nullptr) return SR_INVALID ;

	if ((vp != nullptr) && (vp[0] != '\0')) {
	    int		nlen ;
	    int		cl, cl2 ;
	    int		f_linestart = false ;
	    cchar	*fmt ;
	    cchar	*tp, *cp ;

	    if (vl < 0) vl = lenstr(vp) ;

	    while ((rs >= 0) && (vl > 0)) {

	        if ((cl = nextfield(vp,vl,&cp)) > 0) {

	            nlen = (f_linestart) ? (cl + 1) : cl ;
	            if (nlen > ldp->rlen) {

	                rs = bprintf(ofp,"\n ") ;
	                wlen += rs ;

	                ldp->rlen = ldp->maxlen - 1 ;
	                f_linestart = false ;
	            }

	            fmt = (f_linestart) ? " %r" : "%r" ;
	            if (rs >= 0) {
	                rs = bprintf(ofp,fmt,cp,cl) ;
	                wlen += rs ;
	                ldp->rlen -= rs ;
	            }

	            cl2 = (cp + cl - vp) ;
	            vp += cl2 ;
	            vl -= cl2 ;
	            f_linestart = true ;

	        } else if ((tp = strnchr(vp,vl,'\n')) != nullptr) {
	            vl -= ((tp + 1) - vp) ;
	            vp = (tp + 1) ;
	        } else {
	            vl = 0 ;
		}

	    } /* end while */

	} /* end if (non-empty) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (outvalue) */

/* output the content type */
local int outct(PI *pip,BF *ofp,MMAE *ep) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		cte = ep->cte ;
	cint		f_pt = ep->f_plaintext ;
	int		f_mime = pip->fl.mime ;
	DEBUGPRINTF("ent f_mime=%u\n",f_mime) ;
	if (pip == nullptr) return SR_FAULT ;
	if ((ep->type != nullptr) && (f_mime || (! f_pt) || (cte >= CE_7BIT))) {
	    cchar	*kn ;
	    kn = "content-type" ;
	    if ((rs = bprintf(ofp,"%s:\n",kn)) >= 0) {
	        wlen += rs ;
	        rs = bprintf(ofp," %s",ep->type) ;
	        wlen += rs ;
	    }

/* do we need to put out "/plain" for stupid mailers (like Netscape)? */

	    if (rs >= 0) {
	        if ((ep->subtype != nullptr) && (ep->subtype[0] != '\0')) {
	            rs = bprintf(ofp,"/%s",ep->subtype) ;
	            wlen += rs ;
	        } else if (ep->f_plaintext) {
	            rs = bprintf(ofp,"/plain") ;
	            wlen += rs ;
	        } /* end if */
	    } /* end if (ok) */

	    if (rs >= 0) {
	        if (ep->f_plaintext && (ep->cte >= CE_7BIT)) {
	            cchar *cs = "ISO-8859-1" ;
	            if (ep->cte == CE_7BIT) cs = "US-ASCII" ;
	            rs = bprintf(ofp," ; charset=%s",cs) ;
	            wlen += rs ;
	        } /* end if */
	        if (rs >= 0) {
	            cchar	*dn = "/dev/fd/" ;
	            cchar	*fn = ep->attfname ;
	            if ((fn != nullptr) && (fn[0] != '-') && (fn[0] != '\0')) {
	                if (strncmp(fn,dn,8) != 0) {
	                    if ((rs = bprintf(ofp,";\n")) >= 0) {
	                        wlen += rs ;
	                        rs = bprintf(ofp," name=\"%s\"",fn) ;
	                        wlen += rs ;
	                    }
	                }
	            }
	        } /* end if (ok) */
	    } /* end if (ok) */
	    if (rs >= 0) {
	        rs = bputc(ofp,'\n') ;
	        wlen += rs ;
	    } /* end if (ok) */
	} /* end if (non-null) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progout/outct: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (outct) */

local int outpartbody(PI *pip,BF *ofp,bfile *ifp,MMAE *ep) noex {
	cint	cte = ep->cte ;
	cint	rlen = BUFLEN ;
	int		rs = SR_OK ;
	int		len ;
	int		wlen = 0 ;
	int		f_textcrnl = (ep->f_plaintext && pip->fl.crnl) ;
	char		rbuf[BUFLEN + 2] ; /* added 2 rather than 1 for later */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progoutbody: ent cte=%u f_textcrnl=%u\n",
	        cte,f_textcrnl) ;
#endif

	if ((cte == CE_BASE64) && f_textcrnl) {
	    int	f_eol ;

	    while ((rs = breadln(ifp,rbuf,rlen)) > 0) {
	        len = rs ;

	        f_eol = (rbuf[len - 1] == '\n') ;
	        if (f_eol && (len > 1) && (rbuf[len-2] != '\r')) {
	            rbuf[len-1] = '\r' ;
	            rbuf[len++] = '\n' ;
	        }

	        rs = outbase64(pip,ofp,rbuf,len) ;
	        wlen += rs ;

	        if (rs < 0) break ;
	    } /* end while */

	} else if (cte == CE_BASE64) {
	    cint	ml = MIN(BASE64BUFLEN,BUFLEN) ;

	    while ((rs = bread(ifp,rbuf,ml)) > 0) {
	        len = rs ;

	        rs = outbase64(pip,ofp,rbuf,len) ;
	        wlen += rs ;

	        if (rs < 0) break ;
	    } /* end while */

	} else if ((cte >= CE_7BIT) && (cte < CE_BINARY)) {

	    rs = outpartbodybits(pip,ofp,ifp,ep) ;
	    wlen += rs ;

	} else {

	    rs = bcopyblock(ifp,ofp,-1) ;
	    wlen += rs ;

	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progoutbody: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (outpartbody) */

local int outpartbodybits(PI *pip,BF *ofp,bfile *ifp,MMAE *ep) noex {
	cint	ind = 2 ;
	cint	bllen = BIGLINEBUFLEN ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cint	f_pt = ep->f_plaintext ;
	char		*blbuf = nullptr ;
	char		*p ;

	if ((rs = uc_malloc((bllen+1),&p)) >= 0) {
	    linefold	lf ;
	    cint	cols = MAILTEXTCOLS ;
	    cint	f_textcrnl = (pip->fl.crnl && f_pt) ;
	    int		len ;
	    int		ll ;
	    int		f_bol = true ;
	    int		f_eol ;
	    cchar	*lp ;
	    blbuf = p ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(5)) {
	        debugprintf("out/outpartbodybits: f_pt=%u\n",f_pt) ;
	        debugprintf("out/outpartbodybits: f_textcrnl=%u\n",f_textcrnl) ;
	    }
#endif

	    while ((rs = breadln(ifp,blbuf,bllen)) > 0) {
	        len = rs ;

	        f_eol = (blbuf[len - 1] == '\n') ;
	        if (f_bol && f_pt && (strncmp(blbuf,"From ",5) == 0)) {
	            rs = bputc(ofp,FROM_ESCAPE) ;
	            wlen += rs ;
	        } /* end if ("From" escape for dumb mailers!) */

	        if (rs >= 0) {
	            if (f_textcrnl) {

	                if (blbuf[len-1] == '\n') {
	                    len -= 1 ;
	                    if (blbuf[len-1] == '\r') len -= 1 ;
	                }

	                if (len > 0) {
	                    cint	c = cols ;
	                    cint	l = len ;
	                    char	*blb = blbuf ;
	                    if ((rs = linefold_start(&lf,c,ind,blb,l)) >= 0) {
	                        int	i = 0 ;

	                        while ((ll = linefold_get(&lf,i,&lp)) >= 0) {

#if	CF_DEBUG
	                            if (DEBUGLEVEL(5))
	                                debugprintf("out/outpartbodybits: "
	                                    "i=%u l=>%r<\n",
	                                    i,lp,strlinelen(lp,ll,40)) ;
#endif

	                            if (ll > 0) {
	                                rs = bwrite(ofp,lp,ll) ;
	                                wlen += rs ;
	                            }

	                            if (rs >= 0) {
	                                lp = "\r\n" ;
	                                ll = 2 ;
	                                rs = bwrite(ofp,lp,ll) ;
	                                wlen += rs ;
	                            }

	                            i += 1 ;
	                        } /* end while */

	                        rs1 = linefold_finish(&lf) ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if (linefold) */
	                } else {
	                    lp = "\r\n" ;
	                    ll = 2 ;
	                    rs = bwrite(ofp,lp,ll) ;
	                    wlen += rs ;
	                }

	            } else {
	                rs = bwrite(ofp,blbuf,len) ;
	                wlen += rs ;
	            }
	        } /* end if (ok) */

	        f_bol = f_eol ;
	        if (rs < 0) break ;
	    } /* end while */

	    rs1 = uc_free(blbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */
	DEBUGPRINTF("ret rs=%d wlen=%u\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (outpartbodybits) */

/* write out in BASE64! */
local int outbase64(PI *pip,BF *ofp,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	DEBUGPRINTF("ent slen=%u\n",slen) ;
	if (pip) {
	    rs = SR_OK ;
	    int		rlen = slen ;
	    char	outbuf[BASE64LINELEN + 4] ;
	    for (int mlen, len, i = 0 ; (rs >= 0) && (rlen > 0) ; i += mlen) {
	        mlen = MIN(BASE64BUFLEN,rlen) ;
	        len = base64_e((sbuf + i),mlen,outbuf) ;
	        rs = bprintln(ofp,outbuf,len) ;
	        wlen += rs ;
	        rlen -= mlen ;
	        i += mlen ;
	    } /* end while */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d wlen=%u\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (outbase64) */

local int outline_start(OL *op,int maxlen,int rlen) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    memclear(op) ;
	    op->maxlen = maxlen ;
	    op->rlen = rlen ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (outline_start) */

local int outline_finish(OL *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (outline_finish) */

/* calculate an estimated length */
local int strestlen(cchar *sp,int sl) noex {
	int		len = 0 ;
	cchar		*cp ;
	for (int cl ; (cl = sfnext(sp,sl,&cp)) > 0 ; ) {
	    len += (cl + 1) ;
	    sl -= ((cp + cl) - sp) ;
	    sp = (cp + cl) ;
	} /* end for */
	return len ;
} /* end subroutine (strestlen) */


