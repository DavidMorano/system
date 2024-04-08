/* outema SUPPORT */
/* lang=C++20 */
 
/* manage printing lines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

	= 1999-02-01, David A­D­ Morano
	I added a little code to "post" articles that do not have
	a valid newsgroup to a special "dead article" directory in
	the BB spool area.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object deals with printing lines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<netdb.h>
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<ascii.h>
#include	<estrings.h>
#include	<buffer.h>
#include	<strn.h>
#include	<sfx.h>
#include	<localmisc.h>		/* |NTABCOLS| */

#include	"outema.h"


/* local defines */

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
#endif

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(2048,LINE_MAX)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#undef	BUFLEN
#define	BUFLEN		(2 * 1024)

#ifndef	MAXMSGLINELEN
#define	MAXMSGLINELEN	76
#endif

#define	BASE64LINELEN	72
#define	BASE64BUFLEN	((BASE64LINELEN / 4) * 3)

/* types of "content encodings" */
#define	CE_7BIT		0
#define	CE_8BIT		1
#define	CE_BINARY	2
#define	CE_BASE64	3

#ifndef	FROM_ESCAPE
#define	FROM_ESCAPE	'\b'
#endif

#undef	CHAR_TOKSEP
#define	CHAR_TOKSEP(c)	(CHAR_ISWHITE(c) || (! isprintlatin(c)))


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int outema_magic(outema *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == OUTEMA_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (outema_magic) */

static int	filer_outpart(filer *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int outema_start(outema *op,filer *ofp,int maxlen) noex {
	int		rs = SR_FAULT ;
	if (op && ofp) {
	    rs = memclear(op) ;
	    op->maxlen = maxlen ;
	    op->rlen = maxlen ;
	    op->ofp = ofp ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (outema_start) */

int outema_finish(outema *ldp) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = outema_magic(ldp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (ldp->ofp) {
	        if (ldp->llen > 0) {
	            rs = filer_println(ldp->ofp,nullptr,0) ;
	            ldp->wlen += rs ;
	            ldp->rlen = ldp->maxlen ;
	            ldp->llen = 0 ;
	        }
	        wlen = ldp->wlen ;
	        ldp->ofp = nullptr ;
	    } /* end if (bugcheck) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_finish) */

int outema_ent(outema *ldp,EMA_ENT *ep) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = outema_magic(ldp)) >= 0) {
	    buffer	b, *bufp = &b ;
	    if ((rs = buffer_start(bufp,80)) >= 0) {
	        cchar	*bp ;
	        int	bl ;
	        int	c = 0 ;
	        if ((rs >= 0) && (ep->ap != nullptr) && (ep->al > 0)) {
	            if (c++ > 0) rs = buffer_char(bufp,CH_SP) ;
	            if (rs >= 0)
	                rs = buffer_strquote(bufp,ep->ap,ep->al) ;
	        }
	        if ((rs >= 0) && (ep->rp != nullptr) && (ep->rl > 0)) {
	            if (c++ > 0) rs = buffer_char(bufp,CH_SP) ;
	            if (rs >= 0) rs = buffer_char(bufp,CH_LANGLE) ;
	            if (rs >= 0) rs = buffer_stropaque(bufp,ep->rp,ep->rl) ;
	            if (rs >= 0) rs = buffer_char(bufp,CH_RANGLE) ;
	        }
	        if ((rs >= 0) && (ep->cp != nullptr) && (ep->cl > 0)) {
	            cchar	*cp ;
	            int		cl ;
	            if ((cl = sfshrink(ep->cp,ep->cl,&cp)) > 0) {
	                if (c++ > 0) rs = buffer_char(bufp,CH_SP) ;
	                if (rs >= 0) {
	                    cint	sz = (cl+2+1) ;
	                    char	*ap ;
	                    if ((rs = uc_malloc(sz,&ap)) >= 0) {
	                        char	*bp = ap ;
	                        *bp++ = CH_LPAREN ;
	                        if ((rs = snwcpycompact(bp,cl,cp,cl)) >= 0) {
	                            bp += rs ;
	                            *bp++ = CH_RPAREN ;
	                            rs = buffer_strw(bufp,ap,(bp-ap)) ;
	                        } /* end if (snwcpycompact) */
	                        rs1 = uc_free(ap) ;
				if (rs >= 0) rs = rs1 ;
	                    } /* end if (m-a-f) */
	                } /* end if (ok) */
	            } /* end if (shrink) */
	        } /* end if (comment) */
	        if (rs >= 0) {
	            if ((rs = buffer_get(bufp,&bp)) > 0) {
	                bl = rs ;
	                rs = outema_item(ldp,bp,bl) ;
	                wlen += rs ;
	            }
	        }
	        rs1 = buffer_finish(bufp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (buffer-ret) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_ent) */

int outema_item(outema *ldp,cchar *vp,int vl) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = outema_magic(ldp)) >= 0) {
	    if (vl < 0) vl = strlen(vp) ;
	    if (vl > 0) {
	        bool	f_prevcomma = ldp->f.comma ;
	        ldp->f.comma = true ;
	        rs = outema_value(ldp,vp,vl) ;
	        wlen += rs ;
	        ldp->c_items += 1 ;
	        ldp->f.comma = f_prevcomma ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_item) */

int outema_value(outema *ldp,cchar *vp,int vl) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = outema_magic(ldp,vp)) >= 0) {
	    if (vp && vp[0]) {		/* <- not an error to be empty */
	        int		nlen ;
	        int		cl, cl2 ;
	        bool		f_comma = false ;
	        cchar	*fmt ;
	        cchar	*tp, *cp ;
	        if (vl < 0) vl = strlen(vp) ;
	        ldp->c_values = 0 ;
	        while ((rs >= 0) && (vl > 0)) {
	            if ((cl = sfnextqtok(vp,vl,&cp)) > 0) {
	                f_comma = (ldp->f.comma && (ldp->c_items > 0)) ;
	                nlen = outema_needlength(ldp,cl) ;
	                if (nlen > ldp->rlen) {
	                    if (ldp->llen > 0) {
	                        fmt = "\n" ;
	                        if (f_comma) {
	                            f_comma = false ;
	                            ldp->f.comma = false ;
	                            fmt = ",\n" ;
	                        }
	                        rs = filer_write(ldp->ofp,fmt,-1) ;
	                        wlen += rs ;
	                    }
	                    ldp->rlen = ldp->maxlen ;
	                    ldp->llen = 0 ;
	                    ldp->c_values = 0 ;
	                } /* end if (overflow) */
	                if (rs >= 0) {
	                    if (f_comma) {
	                        ldp->f.comma = false ;
	                    }
	                    rs = filer_outpart(ldp->ofp,f_comma,cp,cl) ;
	                    wlen += rs ;
	                    ldp->llen += rs ;
	                    ldp->rlen -= rs ;
	                    f_comma = false ;
	                }
	                ldp->c_values += 1 ;
	                cl2 = (cp + cl - vp) ;
	                vp += cl2 ;
	                vl -= cl2 ;
	            } else if ((tp = strnchr(vp,vl,'\n')) != nullptr) {
	                vl -= ((tp + 1) - vp) ;
	                vp = (tp + 1) ;
	            } else {
	                vl = 0 ;
	            }
	        } /* end while */
	        ldp->wlen += wlen ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_value) */

int outema_write(outema *ldp,cchar *v,int vlen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = outema_magic(ldp)) >= 0) {
	    if (vlen < 0) vlen = strlen(v) ;
	    if (vlen > 0) {
	        rs = filer_write(ldp->ofp,v,vlen) ;
	        wlen += rs ;
	        ldp->llen += rs ;
	        ldp->rlen -= rs ;
	    }
	    ldp->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_write) */

#ifdef	COMMENT

int outema_printf(outema *ldp,cchar *fmt,...) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = outema_magic(ldp,fmt)) >= 0) {
	    va_list	ap ;
	    cint	flen = BUFLEN ;
	    char	fbuf[BUFLEN + 1] ;
	    va_begin(ap,fmt) ;
	    if ((rs = vbufprintf(buf,BUFLEN,fmt,ap)) >= 0) {
	    	cint	len = rs ;
	        if ((rs = filer_write(ldp->ofp,buf,len)) >= 0) {
	            wlen += rs ;
	            ldp->wlen += rs ;
	            ldp->llen += rs ;
	            ldp->rlen -= rs ;
		} /* end if (filer_write) */
	    } /* end if (vbufprintf) */
	    va_end(ap) ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_printf) */

#endif /* COMMENT */

int outema_hdrkey(outema *ldp,cchar *kname) noex {
	int		rs ;
	int		wlen = 0 ;
	int		nlen = 0 ;
	if ((rs = outema_magic(ldp,kname)) >= 0) {
	    rs = SR_INVALID ;
	    if (kname[0]) {
		rs = SR_OK ;
	        if ((rs >= 0) && (ldp->llen > 0)) {
	            rs = filer_println(ldp->ofp,kname,0) ;
	            wlen += rs ;
	            ldp->llen = 0 ;
	            ldp->rlen = ldp->maxlen ;
	        }
	        if (rs >= 0) {
	            rs = filer_write(ldp->ofp,kname,-1) ;
	            wlen += rs ;
	            nlen += rs ;
	        }
	        if (rs >= 0) {
	            char	buf[2] ;
	            buf[0] = ':' ;
	            buf[1] = '\0' ;
	            rs = filer_write(ldp->ofp,buf,1) ;
	            wlen += rs ;
	            nlen += rs ;
	        }
	        if ((rs >= 0) && (nlen > 0)) {
	            ldp->llen += nlen ;
	            ldp->rlen -= nlen ;
	        }
	        ldp->wlen += wlen ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outema_hdrkey) */

int outema_needlength(outema *ldp,int cl) noex {
	int		rs ;
	int		nlen = (cl + 1) ;
	if ((rs = outema_magic(ldp)) >= 0) {
	    if (ldp->llen == 0) {
	        nlen += 1 ;
	    }
	    if (ldp->f.comma && (ldp->c_items > 0)) {
	        nlen += 1 ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (outema_needlength) */


/* private subroutines */

static int filer_outpart(filer *fbp,int f_comma,cchar *cp,int cl) noex {
	int		rs = SR_BUGCHECK ;
	int		wlen = 0 ;
	if (fbp) {
	    int		i = 0 ;
	    char	buf[3] ;
	    if (f_comma) {
		buf[i++] = CH_COMMA ;
	    }
	    buf[i++] = ' ' ;
	    buf[i] = '\0' ;
	    if ((rs = filer_write(fbp,buf,i)) >= 0) {
	        wlen += rs ;
	        rs = filer_write(fbp,cp,cl) ;
	        wlen += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_outpart) */


