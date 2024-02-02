/* qpdecoder SUPPORT */
/* lang=C++98 */

/* Quoted-Printable (QP) decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was really made from scratch.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We facilitate the decoding of Quoted-Printable (QP) encoded data.

		qpdecoder_start
		qpdecoder_load
		qpdecoder_read
		qpdecoder_finish

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |UCHAR_MAX| */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<new>
#include	<algorithm>		/* < for |min(3c++)| */
#include	<usystem.h>
#include	<usupport.h>		/* <- for |memclear(3u)| */
#include	<digval.h>		/* <- for |hexval(3uc)| */
#include	<six.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"qpdecoder.h"
#include	"obuf.hh"


/* local defines */


/* local namespaces */

using std::min ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int qpdecoder_magic(qpdecoder *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == QPDECODER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (qpdecoder_magic) */

static int	qpdecoder_loadspace(qpdecoder *,cchar *,int) noex ;
static int	qpdecoder_add(qpdecoder *,cchar *,int) noex ;
static int	qpdecoder_add(qpdecoder *,char) noex ;
static int	qpdecoder_cvt(qpdecoder *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int qpdecoder_start(qpdecoder *op,int f_space) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(op) ;		/* dangerous */
	    op->f.space = f_space ;
	    if (obuf *obp ; (obp = new(nothrow) obuf) != nullptr) {
	        op->outbuf = (void *) obp ;
	        op->magic = QPDECODER_MAGIC ;
	    } else {
	        rs = SR_NOMEM ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (qpdecoder_start) */

int qpdecoder_finish(qpdecoder *op) noex {
	int		rs ;
	if ((rs = qpdecoder_magic(op)) >= 0) {
	    if (op->outbuf) {
	        obuf	*obp = (obuf *) op->outbuf ;
	        delete obp ;
	        op->outbuf = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (qpdecoder_finish) */

int qpdecoder_load(qpdecoder *op,cchar *sp,int sl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = qpdecoder_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (obuf *obp ; (obp = ((obuf *) op->outbuf)) != nullptr) {
	        if (op->f.space) {
	            rs = qpdecoder_loadspace(op,sp,sl) ;
	            c += rs ;
	        } else {
	            cint	nl = 2 ;
	            while ((rs >= 0) && (sl > 0)) {
	                if (op->f.esc) {
	                    cint	rl = op->rl ;
	                    int		ml = min(sl,(nl-op->rl)) ;
	                    char	*rb = op->rb ;
	                    strwcpy((rb+rl),sp,ml) ;
	                    op->rl += ml ;
	                    sp += ml ;
	                    sl -= ml ;
	                    if (rl == nl) {
	                        rs = qpdecoder_cvt(op) ;
	                        c += rs ;
	                        op->rl = 0 ;
	                        op->f.esc = false ;
	                    }
	                } else {
	                    int		si ;
	                    while ((si = sichr(sp,sl,'=')) >= 0) {
	                        op->f.esc = true ;
	                        if (si > 0) {
	                            rs = qpdecoder_add(op,sp,si) ;
	                            c += rs ;
	                            sp += si ;
	                            sl -= si ;
	                        }
	                        sp += 1 ;
	                        sl -= 1 ;
	                        if (sl > 0) {
	                            cint	rl = op->rl ;
	                            int		ml = min(sl,(nl-op->rl)) ;
	                            char	*rb = op->rb ;
	                            strwcpy((rb+rl),sp,ml) ;
	                            op->rl += ml ;
	                            sp += ml ;
	                            sl -= ml ;
	                            if (op->rl == nl) {
	                                rs = qpdecoder_cvt(op) ;
	                                c += rs ;
	                                op->rl = 0 ;
	                                op->f.esc = false ;
	                            }
	                        } /* end if */
	                    } /* end while */
	                    if ((rs >= 0) && (sl > 0)) {
	                        rs = qpdecoder_add(op,sp,sl) ;
	                        c += rs ;
	                        sl = 0 ;
	                    } /* end if (remaining source) */
	                    if (rs < 0) break ;
	                } /* end if (escape or not) */
	            } /* end while */
	        } /* end if (space or not) */
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (qpdecoder_load) */

int qpdecoder_read(qpdecoder *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		i = 0 ;
	if ((rs = qpdecoder_magic(op,rbuf)) >= 0) {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (rlen > 0) {
	        int	ml ;
	        if (obuf *obp ; (obp = ((obuf *) op->outbuf)) != nullptr) {
	            cint	len = obp->len() ;
	            ml = min(len,rlen) ;
	            for (i = 0 ; i < ml ; i += 1) {
	                rbuf[i] = obp->at(i) ;
	            }
	            rbuf[i] = '\0' ;
	            rs = obp->adv(i) ;
	        } else {
	            rs = SR_BUGCHECK ;
	        }
	    } /* end if (non-zero positive) */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (qpdecoder_read) */


/* private subroutines */

static int qpdecoder_loadspace(qpdecoder *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	while ((rs >= 0) && (sl-- > 0)) {
	    cint	ch = mkchar(*sp) ;
	    if (! CHAR_ISWHITE(ch)) {
	        if (op->f.esc) {
	            op->rb[op->rl++] = ch ;
	            if (op->rl == 2) {
	                rs = qpdecoder_cvt(op) ;
	                c += rs ;
	                op->rl = 0 ;
	                op->f.esc = false ;
	            }
	        } else if (ch == '=') {
	            op->f.esc = true ;
	        } else {
	            rs = qpdecoder_add(op,ch) ;
	            c += rs ;
	        }
	    } /* end if (not white-space) */
	    sp += 1 ;
	} /* end while */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (qpdecoder_readspace) */

static int qpdecoder_add(qpdecoder *op,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	while ((rs >= 0) && vl-- && *vp) {
	    rs = qpdecoder_add(op,*vp++) ;
	    c += rs ;
	}
	return c ;
}
/* end subrolutine (qpdecoder_add) */

static int qpdecoder_add(qpdecoder *op,char v) noex {
	obuf 	*obp = (obuf *) op->outbuf ;
	if (op->f.space && (v == '_')) v = ' ' ;
	obp->add(v) ;
	return 1 ;
}
/* end subrolutine (qpdecoder_add) */

static int qpdecoder_cvt(qpdecoder *op) noex {
	int		rs = SR_OK ;
	char		*rb = op->rb ;
	if (op->outbuf != nullptr) {
	    obuf 	*obp = (obuf *) op->outbuf ;
	    int		v = 0 ;
	    int		ch0 = mkchar(rb[0]) ;
	    int		ch1 = mkchar(rb[1]) ;
	    if (ishexlatin(ch0) && ishexlatin(ch1)) {
		v |= (hexval(ch0)<<4) ;
		v |= (hexval(ch1)<<0) ;
	    } else {
	        v = '¿' ;
	    } 
	    obp->add(v) ;
	} else
	    rs = SR_BUGCHECK ;
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (qpdecoder_cvt) */


