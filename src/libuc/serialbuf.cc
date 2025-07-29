/* serialbuf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* serializing buffer object handling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-04, David A­D­ Morano
	This object was first written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	serialbug

	Description:
	This object provides a means by which data units can be
	serialized into a sbuffer and unserialized back again.  The
	serialization into the sbuffer is done in a portable way
	using Network Byte Order.  This whole function is similar
	to XDR in general but this object allows the details of the
	serial sbuffer to be known rather than opaque as in XDR!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |strlen(3c)| + |memcpy(3c)| */
#include	<usystem.h>
#include	<stdintx.h>		/* |longlong| */
#include	<stdorder.h>
#include	<strwcpy.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"serialbuf.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

#ifndef	TYPEDEF_LONGLONGP
#define	TYPEDEF_LONGLONGP
typedef longlong *	longlongp ;
#endif


/* extern subroutines */


/* extern variables */


/* local structures */


/* forward references */

static int serialbuf_wok(serialbuf *op,cvoid *wp,int wl) noex {
	int		rs = SR_FAULT ;
	if (op && wp) ylikely {
	    if (op->sbuf) ylikely {
		if ((rs = op->i) >= 0) ylikely {
		    cint	al = (op->slen - op->i) ;
		    rs = SR_TOOBIG ;
	            if (al >= wl) {
		        rs = al ;
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (serialbuf_wok) */

static int serialbuf_rok(serialbuf *op,void *rp,int rl) noex {
	return serialbuf_wok(op,rp,rl) ;
}
/* end subroutine (serialbuf_rok) */

static int serialbuf_szok(serialbuf *op,int sz) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (op->sbuf) ylikely {
		if ((rs = op->i) >= 0) {
		    cint	al = (op->slen - op->i) ;
		    rs = SR_TOOBIG ;
	            if (al >= sz) {
		        rs = al ;
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (serialbuf_szok) */

template<typename T>
static int serialbuf_rx(serialbuf *op,T *rp) noex {
	cint		sz = szof(T) ;
	int		rs ;
	if ((rs = serialbuf_rok(op,rp,sz)) >= 0) ylikely {
	    cchar	*sp = (op->sbuf + op->i) ;
	    stdorder_r(sp,rp) ;
	    op->i += sz ;
	}
	return rs ;
}
/* end subroutine-template (serialbuf_rx) */

template<typename T>
static int serialbuf_rxa(serialbuf *op,T *rp,int n) noex {
	cint		sz = szof(T) ;
	for (int i = 0 ; (op->i >= 0) && (i < n) ; i += 1) {
	    if ((op->slen - op->i) >= sz) {
		cchar	*sp = (op->sbuf + op->i) ;
	        stdorder_r(sp,(rp + i)) ;
	        op->i += sz ;
	    } else {
	        op->i = SR_TOOBIG ;
	    }
	} /* end for */
	return op->i ;
}
/* end subroutine-template (serialbuf_rxa) */

template<typename T>
static int serialbuf_wx(serialbuf *op,T v) noex {
	cint		sz = szof(T) ;
	int		rs ;
	if ((rs = serialbuf_szok(op,sz)) >= 0) ylikely {
	    char	*bp = (op->sbuf + op->i) ;
	    stdorder_w(bp,v) ;
	    op->i += sz ;
	}
	return rs ;
}
/* end subroutine-template (serialbuf_wx) */

template<typename T>
static int serialbuf_wxa(serialbuf *op,T *wa,int n) noex {
	cint		sz = szof(T) ;
	for (int i = 0 ; (op->i >= 0) && (i < n) ; i += 1) {
	    if ((op->slen - op->i) >= sz) {
		char	*bp = (op->sbuf + op->slen) ;
	        stdorder_w(bp,wa[i]) ;
	        op->i += sz ;
	    } else {
	        op->i = SR_TOOBIG ;
	    }
	} /* end for */
	return op->i ;
}
/* end subroutine-template (serialbuf_wxa) */


/* local variables */


/* exported variables */


/* exported subroutines */

int serialbuf_start(serialbuf *op,char *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (sbuf) ylikely {
		rs = SR_INVALID ;
		if (slen > 0) {
	            rs = SR_OK ;
	            op->sbuf = sbuf ;
	            op->slen = slen ;
	            op->i = 0 ;
		}
	    } /* end if (non-null supplied buffer) */
	    if (rs < 0) {
	        op->sbuf = nullptr ;
	        op->slen = 0 ;
	        op->i = -1 ;
	    } /* end if (error) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (serialbuf_start) */

int serialbuf_finish(serialbuf *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->sbuf && op->slen) {
	        op->sbuf = nullptr ;
	        op->slen = 0 ;
	        rs = op->i ;
	        op->i = 0 ;
	    }
	}
	return rs ;
}
/* end subroutine (serialbuf_finish) */

/* "advance" the buffer-index as if we wrote something in there */
int serialbuf_adv(serialbuf *op,int sz) noex {
	int		rs ;
	if ((rs = serialbuf_szok(op,sz)) >= 0) ylikely {
	    op->i += sz ;
	}
	return rs ;
}
/* end subroutine (serialbuf_adv) */

int serialbuf_getlen(serialbuf *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->sbuf && op->slen) ylikely {
	        rs = op->i ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (serialbuf_getlen) */

int serialbuf_reset(serialbuf *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->sbuf && op->slen) ylikely {
	        op->i = 0 ;
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (serialbuf_reset) */

int serialbuf_robj(serialbuf *op,void *rp,int rl) noex {
	char		*rb = charp(rp) ;
	int		rs ;
	if ((rs = serialbuf_rok(op,rb,rl)) >= 0) ylikely {
	    cchar	*sp = (op->sbuf + op->i) ;
	    memcpy(rp,sp,rl) ;
	    op->i += rl ;
	}
	return rs ;
}
/* end subroutine (serialbuf_robj) */

int serialbuf_rc(serialbuf *op,char *rp) noex {
	cint		sz = szof(char) ;
	int		rs ;
	if ((rs = serialbuf_rok(op,rp,sz)) >= 0) ylikely {
	    cchar	*sp = (op->sbuf + op->i) ;
	    *rp = *sp ;
	    op->i += sz ;
	}
	return rs ;
}
/* end subroutine (serialbuf_rc) */

int serialbuf_rs(serialbuf *op,short *rp) noex {
	return serialbuf_rx(op,rp) ;
}
/* end subroutine (serialbuf_rshort) */

int serialbuf_ri(serialbuf *op,int *rp) noex {
	return serialbuf_rx(op,rp) ;
}
/* end subroutine (serialbuf_ri) */

int serialbuf_rl(serialbuf *op,long *rp) noex {
	return serialbuf_rx(op,rp) ;
}
/* end subroutine (serialbuf_rl) */

int serialbuf_rll(serialbuf *op,longlong *rp) noex {
	return serialbuf_rx(op,rp) ;
}
/* end subroutine (serialbuf_rll) */

int serialbuf_ria(serialbuf *op,int *rp,int n) noex {
	return serialbuf_rxa(op,rp,n) ;
}
/* end subroutine (serialbuf_ria) */

int serialbuf_rla(serialbuf *op,long *rp,int n) noex {
	return serialbuf_rxa(op,rp,n) ;
}
/* end subroutine (serialbuf_rla) */

int serialbuf_rlla(serialbuf *op,longlong *rp,int n) noex {
	return serialbuf_rxa(op,rp,n) ;
}
/* end subroutine (serialbuf_rlla) */

/* read a fixed length string (possibly not NUL-terminated) */
int serialbuf_rstrn(serialbuf *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = serialbuf_rok(op,rbuf,rlen)) >= 0) ylikely {
	    cchar	*sp = (op->sbuf + op->i) ;
	    rl = intconv(strwcpy(rbuf,sp,rlen) - rbuf) ;
	    op->i += rl ;
	} /* end if (valid) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (serialbuf_rstrn) */

/* read a NUL-terminated variable length string */
int serialbuf_rstrw(serialbuf *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = serialbuf_rok(op,rbuf,rlen)) >= 0) ylikely {
	    cchar	*sp = (op->sbuf + op->i) ;
	    if ((rs = sncpy(rbuf,rlen,sp)) >= 0) ylikely {
		rl = rs ;
	        op->i += rl ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (serialbuf_rstrw) */

int serialbuf_rbuf(serialbuf *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = serialbuf_rok(op,rbuf,rlen)) >= 0) ylikely {
	    cchar	*sp = (op->sbuf + op->i) ;
	    memcpy(rbuf,sp,rlen) ;
	    rl = rlen ;
	    op->i += rl ;
	} /* end if (valid) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (serialbuf_rbuf) */

int serialbuf_ruc(serialbuf *op,uchar *rp) noex {
	char		*p = charp(rp) ;
	return serialbuf_rx(op,p) ;
}
/* end subroutine (serialbuf_ruc) */

int serialbuf_rus(serialbuf *op,ushort *rp) noex {
	short		*p = shortp(rp) ;
	return serialbuf_rx(op,p) ;
}
/* end subroutine (serialbuf_rus) */

int serialbuf_rui(serialbuf *op,uint *rp) noex {
	int		*p = intp(rp) ;
	return serialbuf_rx(op,p) ;
}
/* end subroutine (serialbuf_rui) */

int serialbuf_rul(serialbuf *op,ulong *rp) noex {
	long		*p = longp(rp) ;
	return serialbuf_rx(op,p) ;
}
/* end subroutine (serialbuf_rul) */

int serialbuf_rull(serialbuf *op,ulonglong *rp) noex {
	longlong	*p = longlongp(rp) ;
	return serialbuf_rx(op,p) ;
}
/* end subroutine (serialbuf_rull) */

int serialbuf_ruia(serialbuf *op,uint *rp,int n) noex {
	int		*p = intp(rp) ;
	return serialbuf_rxa(op,p,n) ;
}
/* end subroutine (serialbuf_ruia) */

int serialbuf_rula(serialbuf *op,ulong *rp,int n) noex {
	long		*p = longp(rp) ;
	return serialbuf_rxa(op,p,n) ;
}
/* end subroutine (serialbuf_rula) */

int serialbuf_rulla(serialbuf *op,ulonglong *rp,int n) noex {
	longlong	*p = longlongp(rp) ;
	return serialbuf_rxa(op,p,n) ;
}
/* end subroutine (serialbuf_rulla) */

int serialbuf_rustrn(serialbuf *op,uchar *rbuf,int rlen) noex {
	char		*rb = charp(rbuf) ;
	return serialbuf_rstrn(op,rb,rlen) ;
}
/* end subroutine (serialbuf_rustrn) */

int serialbuf_rustrw(serialbuf *op,uchar *rbuf,int rlen) noex {
	char		*rb = charp(rbuf) ;
	return serialbuf_rstrw(op,rb,rlen) ;
}
/* end subroutine (serialbuf_rustrw) */

int serialbuf_rubuf(serialbuf *op,uchar *rbuf,int rlen) noex {
	char		*rb = charp(rbuf) ;
	return serialbuf_rbuf(op,rb,rlen) ;
}
/* end subroutine (serialbuf_rubuf) */

int serialbuf_wobj(serialbuf *op,cvoid *wp,int wl) noex {
	char		*wb = charp(wp) ;
	int		rs ;
	if ((rs = serialbuf_wok(op,wb,wl)) >= 0) ylikely {
	    char	*bp = (op->sbuf + op->i) ;
	    memcpy(bp,wp,wl) ;
	    op->i += wl ;
	}
	return rs ;
}
/* end subroutine (serialbuf_wobj) */

int serialbuf_wc(serialbuf *op,char ch) noex {
	cint		sz = szof(char) ;
	int		rs ;
	if ((rs = serialbuf_szok(op,sz)) >= 0) ylikely {
	    char	*bp = (op->sbuf + op->slen) ;
	    *bp = char(ch) ;
	    op->i += sz ;
	}
	return rs ;
}
/* end subroutine (serialbuf_wc) */

int serialbuf_ws(serialbuf *op,short sw) noex {
	return serialbuf_wx(op,sw) ;
}
/* end subroutine (serialbuf_ws) */

int serialbuf_wi(serialbuf *op,int iw) noex {
	return serialbuf_wx(op,iw) ;
}
/* end subroutine (serialbuf_wi) */

int serialbuf_wl(serialbuf *op,long lw) noex {
	return serialbuf_wx(op,lw) ;
}
/* end subroutine (serialbuf_wl) */

int serialbuf_wll(serialbuf *op,longlong llw) noex {
	return serialbuf_wx(op,llw) ;
}
/* end subroutine (serialbuf_wll) */

int serialbuf_wia(serialbuf *op,cint *iwa,int n) noex {
	return serialbuf_wxa(op,iwa,n) ;
}
/* end subroutine (serialbuf_wia) */

int serialbuf_wla(serialbuf *op,clong *lwa,int n) noex {
	return serialbuf_wxa(op,lwa,n) ;
}
/* end subroutine (serialbuf_wla) */

int serialbuf_wlla(serialbuf *op,const longlong *llwa,int n) noex {
	return serialbuf_wxa(op,llwa,n) ;
}
/* end subroutine (serialbuf_wlla) */

/* write a fixed length string (possibly not NUL-terminated) */
int serialbuf_wstrn(serialbuf *op,cchar *wbuf,int wlen) noex {
	int		rs ;
	int		wl = 0 ;
	if ((rs = serialbuf_wok(op,wbuf,wlen)) >= 0) ylikely {
	    char	*bp = (op->sbuf + op->i) ;
	    strncpy(bp,wbuf,wlen) ;
	    wl = wlen ;
	    op->i += wl ;
	}
	return (rs >= 0) ? wl : rs ;
}
/* end subroutine (serialbuf_wstrn) */

/* write a NUL-terminated variable length string */
int serialbuf_wstrw(serialbuf *op,cchar *wbuf,int wlen) noex {
	int		rs ;
	int		wl = 0 ;
	if ((rs = serialbuf_wok(op,wbuf,(wlen+1))) >= 0) ylikely {
	    char	*bp = (op->sbuf + op->i) ;
	    wl = intconv(strwcpy(bp,wbuf,wlen) - bp) ;
	    op->i += wl ;
	}
	return (rs >= 0) ? wl : rs ;
}
/* end subroutine (serialbuf_wstrw) */

int serialbuf_wbuf(serialbuf *op,cchar *wbuf,int wlen) noex {
	int		rs ;
	int		wl = 0 ;
	if ((rs = serialbuf_wok(op,wbuf,(wlen+1))) >= 0) ylikely {
	    char	*bp = (op->sbuf + op->i) ;
	    memcpy(bp,wbuf,wlen) ;
	    wl = wlen ;
	    op->i += wl ;
	}
	return (rs >= 0) ? wl : rs ;
}
/* end subroutine (serialbuf_wbuf) */

int serialbuf_wuc(serialbuf *op,uchar uch) noex {
	char		ch = char(uch) ;
	return serialbuf_wx(op,ch) ;
}
/* end subroutine (serialbuf_wuchar) */

int serialbuf_wus(serialbuf *op,ushort usw) noex {
	short		sw = short(usw) ;
	return serialbuf_wx(op,sw) ;
}
/* end subroutine (serialbuf_wus) */

int serialbuf_wui(serialbuf *op,uint uiw) noex {
	int		iw = int(uiw) ;
	return serialbuf_wx(op,iw) ;
}
/* end subroutine (serialbuf_wui) */

int serialbuf_wul(serialbuf *op,ulong ulw) noex {
	long		lw = long(ulw) ;
	return serialbuf_wx(op,lw) ;
}
/* end subroutine (serialbuf_wul) */

int serialbuf_wull(serialbuf *op,ulonglong ullw) noex {
	longlong	llw = longlong(ullw) ;
	return serialbuf_wx(op,llw) ;
}
/* end subroutine (serialbuf_wull) */

int serialbuf_wuia(serialbuf *op,cuint *uiwa,int n) noex {
	int		*iwa = intp(uiwa) ;
	return serialbuf_wxa(op,iwa,n) ;
}
/* end subroutine (serialbuf_wuia) */

int serialbuf_wula(serialbuf *op,culong *ulwa,int n) noex {
	long		*lwa = longp(ulwa) ;
	return serialbuf_wxa(op,lwa,n) ;
}
/* end subroutine (serialbuf_wula) */

int serialbuf_wulla(serialbuf *op,const ulonglong *ullwa,int n) noex {
	longlong	*llwa = longlongp(ullwa) ;
	return serialbuf_wxa(op,llwa,n) ;
}
/* end subroutine (serialbuf_wulla) */

int serialbuf_wustrn(serialbuf *op,const uchar *wbuf,int wlen) noex {
	char		*wb = charp(wbuf) ;
	return serialbuf_wstrn(op,wb,wlen) ;
}
/* end subroutine (serialbuf_wustrn) */

int serialbuf_wustrw(serialbuf *op,const uchar *wbuf,int wlen) noex {
	char		*wb = charp(wbuf) ;
	return serialbuf_wstrw(op,wb,wlen) ;
}
/* end subroutine (serialbuf_wustrw) */

int serialbuf_wubuf(serialbuf *op,const uchar *wbuf,int wlen) noex {
	char		*wb = charp(wbuf) ;
	return serialbuf_wbuf(op,wb,wlen) ;
}
/* end subroutine (serialbuf_wubuf) */


/* local subroutines */

int serialbuf::start(char *bp,int bl) noex {
	return serialbuf_start(this,bp,bl) ;
}

int serialbuf::robj(void *rp,int rl) noex {
	return serialbuf_robj(this,rp,rl) ;
}
int serialbuf::rc(char *rp) noex {
	return serialbuf_rc(this,rp) ;
}
int serialbuf::rs(short *rp) noex {
	return serialbuf_rs(this,rp) ;
}
int serialbuf::ri(int *rp) noex {
	return serialbuf_ri(this,rp) ;
}
int serialbuf::rl(long *rp) noex {
	return serialbuf_rl(this,rp) ;
}
int serialbuf::rll(longlong *rp) noex {
	return serialbuf_rll(this,rp) ;
}
int serialbuf::ria(int *rp,int n) noex {
	return serialbuf_ria(this,rp,n) ;
}
int serialbuf::rla(long *rp,int n) noex {
	return serialbuf_rla(this,rp,n) ;
}
int serialbuf::rlla(longlong *rp,int n) noex {
	return serialbuf_rlla(this,rp,n) ;
}
int serialbuf::rstrn(char *rbuf,int rlen) noex {
	return serialbuf_rstrn(this,rbuf,rlen) ;
}
int serialbuf::rstrw(char *rbuf,int rlen) noex {
	return serialbuf_rstrw(this,rbuf,rlen) ;
}
int serialbuf::rbuf(char *rbuf,int rlen) noex {
	return serialbuf_rbuf(this,rbuf,rlen) ;
}
int serialbuf::ruc(uchar *rp) noex {
	return serialbuf_ruc(this,rp) ;
}
int serialbuf::rus(ushort *rp) noex {
	return serialbuf_rus(this,rp) ;
}
int serialbuf::rui(uint *rp) noex {
	return serialbuf_rui(this,rp) ;
}
int serialbuf::rul(ulong *rp) noex {
	return serialbuf_rul(this,rp) ;
}
int serialbuf::rull(ulonglong *rp) noex {
	return serialbuf_rull(this,rp) ;
}
int serialbuf::ruia(uint *rp,int n) noex {
	return serialbuf_ruia(this,rp,n) ;
}
int serialbuf::rula(ulong *rp,int n) noex {
	return serialbuf_rula(this,rp,n) ;
}
int serialbuf::rulla(ulonglong *rp,int n) noex {
	return serialbuf_rulla(this,rp,n) ;
}
int serialbuf::rustrn(uchar *rbuf,int rlen) noex {
	return serialbuf_rustrn(this,rbuf,rlen) ;
}
int serialbuf::rustrw(uchar *rbuf,int rlen) noex {
	return serialbuf_rustrw(this,rbuf,rlen) ;
}
int serialbuf::rubuf(uchar *rbuf,int rlen) noex {
	return serialbuf_rubuf(this,rbuf,rlen) ;
}

int serialbuf::wobj(cvoid *wp,int wl) noex {
	return serialbuf_wobj(this,wp,wl) ;
}
int serialbuf::wc(char ch) noex {
	return serialbuf_wc(this,ch) ;
}
int serialbuf::ws(short sw) noex {
	return serialbuf_ws(this,sw) ;
}
int serialbuf::wi(int iw) noex {
	return serialbuf_wi(this,iw) ;
}
int serialbuf::wl(long lw) noex {
	return serialbuf_wl(this,lw) ;
}
int serialbuf::wll(longlong llw) noex {
	return serialbuf_wll(this,llw) ;
}
int serialbuf::wia(cint *iwa,int n) noex {
	return serialbuf_wia(this,iwa,n) ;
}
int serialbuf::wla(clong *lwa,int n) noex {
	return serialbuf_wla(this,lwa,n) ;
}
int serialbuf::wlla(const longlong *llwa,int n) noex {
	return serialbuf_wlla(this,llwa,n) ;
}
int serialbuf::wstrn(cchar *wbuf,int wlen) noex {
	return serialbuf_wstrn(this,wbuf,wlen) ;
}
int serialbuf::wstrw(cchar *wbuf,int wlen) noex {
	return serialbuf_wstrw(this,wbuf,wlen) ;
}
int serialbuf::wbuf(cchar *wbuf,int wlen) noex {
	return serialbuf_wbuf(this,wbuf,wlen) ;
}
int serialbuf::wuc(uchar uch) noex {
	return serialbuf_wuc(this,uch) ;
}
int serialbuf::wus(ushort usw) noex {
	return serialbuf_wus(this,usw) ;
}
int serialbuf::wui(uint uiw) noex {
	return serialbuf_wui(this,uiw) ;
}
int serialbuf::wul(ulong ulw) noex {
	return serialbuf_wul(this,ulw) ;
}
int serialbuf::wull(ulonglong ullw) noex {
	return serialbuf_wull(this,ullw) ;
}
int serialbuf::wuia(const uint *uiwa,int n) noex {
	return serialbuf_wuia(this,uiwa,n) ;
}
int serialbuf::wula(const ulong *ulwa,int n) noex {
	return serialbuf_wula(this,ulwa,n) ;
}
int serialbuf::wulla(const ulonglong *ullwa,int n) noex {
	return serialbuf_wulla(this,ullwa,n) ;
}
int serialbuf::wustrn(const uchar *wbuf,int wlen) noex {
	return serialbuf_wustrn(this,wbuf,wlen) ;
}
int serialbuf::wustrw(const uchar *wbuf,int wlen) noex {
	return serialbuf_wustrw(this,wbuf,wlen) ;
}
int serialbuf::wubuf(const uchar *wbuf,int wlen) noex {
	return serialbuf_wubuf(this,wbuf,wlen) ;
}

int serialbuf::adv(int sz) noex {
	return serialbuf_adv(this,sz) ;
}

void serialbuf::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("serialbuf",rs,"fini-finish") ;
	}
} /* end method (serialbuf::dtor) */

serialbuf_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case serialbufmem_getlen:
	        rs = serialbuf_getlen(op) ;
	        break ;
	    case serialbufmem_reset:
	        rs = serialbuf_reset(op) ;
	        break ;
	    case serialbufmem_finish:
	        rs = serialbuf_finish(op) ;
	        break ;
	    } /* end switch */
	}
	return rs ;
}
/* end method (serialbuf_co::operator) */


