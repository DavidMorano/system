/* htm SUPPORT (HTML creation and output) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* hack to output HTML */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	htm

	Description:
	This is a hack to create and output some basic HTML.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<shio.h>
#include	<sbuf.h>
#include	<buffer.h>
#include	<ascii.h>
#include	<linefold.h>
#include	<bufprintf.h>
#include	<ctdec.h>
#include	<localmisc.h>		/* |COLUMNS| + |DIGBUFLEN| */

#include	"htm.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int htm_ctor(htm *op,Args ... args) noex {
    	HTM		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (htm_ctor) */

static int htm_dtor(htm *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (htm_dtor) */

template<typename ... Args>
static inline int htm_magic(htm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == HTM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (htm_magic) */

static int	htm_tagalone(htm *,cchar *,cchar *,cchar *) noex ;
static int	htm_printout(htm *,int,cchar *,int) noex ;
static int	htm_wrfile(htm *,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int htm_start(htm *op,shio *ofp,cchar *lang) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = htm_ctor(op,ofp)) >= 0) {
	    op->ofp = ofp ;
	    if ((rs = shio_print(op->ofp,"<!doctype html>",-1)) >= 0) {
	        wlen += rs ;
	        if (char *lp{} ; (rs = malloc_ml(&lp)) >= 0) {
		    cint	ll = rs ;
		    cchar	*fmt ;
		    op->lbuf = lp ;
		    op->llen = ll ;
	            if ((lang != nullptr) && (lang[0] != '\0')) {
			fmt = "<html lang=\"%s\">" ;
	                rs = bufprintf(op->lbuf,op->llen,fmt,lang) ;
	            } else {
			fmt = "<html>" ;
	                rs = bufprintf(op->lbuf,op->llen,fmt) ;
	            }
	            if (rs >= 0) {
	                if ((rs = shio_print(op->ofp,op->lbuf,rs)) >= 0) {
	                    wlen += rs ;
	    		    op->magic = HTM_MAGIC ;
		        }
	            }
		    if (rs < 0) {
		        rs1 = uc_free(op->lbuf) ;
		        if (rs >= 0) rs = rs1 ;
			op->lbuf = nullptr ;
			op->llen = 0 ;
		    } /* end if (error handling) */
		} /* end if (memory-allocation) */
	    } /* end if (doctype) */
	    op->wlen += wlen ;
	    if (rs < 0) {
		htm_dtor(op) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_start) */

int htm_finish(htm *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op)) >= 0) {
	    {
	        rs1 = htm_printline(op,"</html>",-1) ;
	        if (rs >= 0) rs = rs1 ;
		wlen += rs ;
		op->wlen += wlen ;
	    }
	    if (op->lbuf) {
		rs1 = uc_free(op->lbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = htm_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    wlen = op->wlen ;
	    op->ofp = nullptr ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_finish) */

int htm_headbegin(htm *op,cchar *cfname) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op)) >= 0) {
	    cchar	*sp = "<head>" ;
	    if ((rs = shio_print(op->ofp,sp,-1)) >= 0) {
	        wlen += rs ;
	        if ((cfname != nullptr) && (cfname[0] != '\0')) {
		    rs = htm_wrfile(op,cfname) ;
		    wlen += rs ;
	        } /* end if (have content-file) */
	    } /* end if (shio_print) */
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_headbegin) */

int htm_headend(htm *op) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op)) >= 0) {
	    cchar	*sp = "</head>" ;
	    rs = shio_print(op->ofp,sp,-1) ;
	    wlen += rs ;
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_headend) */

int htm_bodybegin(htm *op,cchar *cfname) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op)) >= 0) {
	    cchar	*sp = "<body>" ;
	    if ((rs = shio_print(op->ofp,sp,-1)) >= 0) {
	        wlen += rs ;
	        if ((cfname != nullptr) && (cfname[0] != '\0')) {
		    rs = htm_wrfile(op,cfname) ;
		    wlen += rs ;
	        } /* end if (have content-file) */
		op->wlen += wlen ;
	    } /* end if (shio_print) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_bodybegin) */

int htm_bodyend(htm *op) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op)) >= 0) {
	    cchar	*sp = "</body>" ;
	    rs = shio_print(op->ofp,sp,-1) ;
	    wlen += rs ;
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_bodyend) */

int htm_tagbegin(htm *op,cc *tag,cc *eclass,cc *id,cc *(*kv)[2]) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op,tag)) >= 0) {
	    rs = SR_INVALID ;
	    if (tag[0]) {
	        cint	c = COLUMNS ;
	        if (buffer b ; (rs = b.start(c)) >= 0) {
	            cchar	*k ;
	            cchar	*v ;
	            b.chr(CH_LANGLE) ;
	            b.strw(tag,-1) ;
	            for (int i = 0 ; i < 2 ; i += 1) {
	                v = nullptr ;
	                switch (i) {
	                case 0:
	                    k = "class" ;
	                    v = eclass ;
	                    break ;
	                case 1:
	                    k = "id" ;
	                    v = id ;
	                    break ;
	                } /* end switch */
	                if ((v != nullptr) && (v[0] != '\0')) {
	                    rs = b.printf(" %s=\"%s\"",k,v) ;
	                }
	                if (rs < 0) break ;
	            } /* end for */
	            if ((rs >= 0) && (kv != nullptr)) {
		        cchar	*fmt ;
	                for (int i = 0 ; kv[i][0] != nullptr ; i += 1) {
		            if (kv[i][1] != nullptr) {
			        if (kv[i][1][0] != '\0') {
	                            fmt = " %s=\"%s\"" ;
			        } else {
	                            fmt = " %s=" ;
			        }
		            } else {
	                        fmt = " %s" ;
		            }
	                    rs = b.printf(fmt,kv[i][0],kv[i][1]) ;
	                    if (rs < 0) break ;
	                } /* end for */
	            } /* end if (key-vals) */
	            b.chr(CH_RANGLE) ;
	            if (rs >= 0) {
		        if (cchar *bp ; (rs = b.get(&bp)) >= 0) {
		            rs = htm_printout(op,c,bp,rs) ;
		            wlen += rs ;
	                } /* enbd if (buffer_get) */
	            } /* end if (ok) */
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (buffer) */
	        op->wlen += wlen ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_tagbegin) */

int htm_tagend(htm *op,cchar *tag) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op,tag)) >= 0) {
	    rs = SR_INVALID ;
	    if (tag[0]) {
		cchar	*fmt = "</%s>" ;
	        if ((rs = bufprintf(op->lbuf,op->llen,fmt,tag)) >= 0) {
	            rs = shio_print(op->ofp,op->lbuf,rs) ;
	            wlen += rs ;
	            op->wlen += wlen ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_tagend) */

int htm_textbegin(htm *op,cc *eclass,cc *id,cc *title,
		int r,int c,cchar *(*tkv)[2]) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = htm_magic(op)) >= 0) {
	    cint	dlen = DIGBUFLEN ;
	    int		bsz = 0 ;
	    int		kvsize = 0 ;
	    cchar	*tag = "textarea" ;
	    if (tkv != nullptr) {
	        int	i ; /* used-afterwards */
	        for (i = 0 ; tkv[i][0] != nullptr ; i += 1) ;
	        kvsize = (i*(2*szof(cchar *))) ;
	    }
	    kvsize += (6*(2*szof(cchar *))) ;
	    bsz += kvsize ;
	    bsz += ((dlen+1)*2) ;
	    if (void *vp{} ; (rs = uc_malloc(bsz,&vp)) >= 0) {
	        cchar	*(*kv)[2] ;
	        char	*bp = charp(vp) ;
	        char	*d0, *d1 ;
	        kv = (cchar *(*)[2]) bp ;
	        bp += kvsize ;
	        d0 = bp ;
	        bp += (dlen+1) ;
	        d1 = bp ;
	        bp += (dlen+1) ;
	        if ((rs = ctdeci(d0,dlen,r)) >= 0) {
	            if ((rs = ctdeci(d1,dlen,c)) >= 0) {
		        int	i = 0 ;
	                kv[i][0] = "rows" ;
	                kv[i][1] = d0 ;
		        i += 1 ;
	                kv[i][0] = "cols" ;
	                kv[i][1] = d1 ;
		        i += 1 ;
		        if (title != nullptr) {
	                    kv[i][0] = "title" ;
	                    kv[i][1] = title ;
		            i += 1 ;
		        }
		        if (tkv != nullptr) {
			    for (int j = 0 ; tkv[j][0] != nullptr ; j += 1) {
			        kv[i][0] = tkv[j][0] ;
			        kv[i][1] = tkv[j][1] ;
			        i += 1 ;
			    } /* end for */
		        } /* end if (have extras) */
	                kv[i][0] = nullptr ;
	                kv[i][1] = nullptr ;
	                rs = htm_tagbegin(op,tag,eclass,id,kv) ;
	            } /* end if (ctdeci) */
	        } /* end if (ctdeci) */
	        rs1 = uc_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (htm_textbegin) */

int htm_textend(htm *op) noex {
	cchar	*tag = "textarea" ;
	return htm_tagend(op,tag) ;
}
/* end subroutine (htm_textend) */

int htm_abegin(htm *op,cc *eclass,cc *id,cc *href,cc *title) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op,href)) >= 0) {
	    rs = SR_INVALID ;
	    if (href[0]) {
	        if (sbuf b ; (rs = b.start(op->lbuf,op->llen)) >= 0) {
	            cchar	*tag = "a" ;
	            b.chr(CH_LANGLE) ;
	            b.strw(tag,-1) ;
	            for (int c = 0 ; c < 4 ; c += 1) {
	                cchar	*k = nullptr ;
	                cchar	*v = nullptr ;
	                switch (c) {
	                case 0:
	                    k = "class" ;
	                    v = eclass ;
	                    break ;
	                case 1:
	                    k = "id" ;
	                    v = id ;
	                    break ;
	                case 2:
	                    k = "href" ;
	                    v = href ;
	                    break ;
	                case 3:
	                    k = "title" ;
	                    v = title ;
	                    break ;
	                } /* end switch */
	                if ((v != nullptr) && (v[0] != '\0')) {
			    cchar	*fmt = "\n %s=\"%s\"" ;
	                    rs = b.printf(fmt,k,v) ;
	                }
	                if (rs < 0) break ;
	            } /* end for */
	            b.chr(CH_RANGLE) ;
	            if ((rs = b.getlen) >= 0) {
	                rs = shio_print(op->ofp,op->lbuf,rs) ;
	                wlen += rs ;
	            }
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	        op->wlen += wlen ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_abegin) */

int htm_aend(htm *op) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op)) >= 0) {
	    cchar	*tag = "a" ;
	    if ((rs = bufprintf(op->lbuf,op->llen,"</%s>",tag)) >= 0) {
	        rs = shio_print(op->ofp,op->lbuf,rs) ;
	        wlen += rs ;
	        op->wlen += wlen ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_aend) */

int htm_hr(htm *op,cchar *eclass,cchar *id) noex {
	return htm_tagalone(op,"hr",eclass,id) ;
}
/* end subroutine (htm_hr) */

int htm_br(htm *op,cchar *eclass,cchar *id) noex {
	return htm_tagalone(op,"br",eclass,id) ;
}
/* end subroutine (htm_br) */

int htm_img(htm *op,cc *eclass,cc *id,cc *src,cc *title,cc *alt,
		int w,int h) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op,src)) >= 0) {
	    rs = SR_INVALID ;
	    if (src[0]) {
	        if (sbuf b ; (rs = b.start(op->lbuf,op->llen)) >= 0) {
	            cchar	*tag = "img" ;
	            b.chr(CH_LANGLE) ;
	            b.strw(tag,-1) ;
	            for (int c = 0 ; c < 5 ; c += 1) {
	                cchar	*k = nullptr ;
	                cchar	*v = nullptr ;
	                switch (c) {
	                case 0:
	                    k = "class" ;
	                    v = eclass ;
	                    break ;
	                case 1:
	                    k = "id" ;
	                    v = id ;
	                    break ;
	                case 2:
	                    k = "src" ;
	                    v = src ;
	                    break ;
	                case 3:
	                    k = "title" ;
	                    v = title ;
	                    break ;
	                case 4:
	                    k = "alt" ;
	                    v = alt ;
	                    break ;
	                } /* end switch */
	                if ((v != nullptr) && (v[0] != '\0')) {
	                    rs = b.printf("\n %s=\"%s\"",k,v) ;
	                }
	                if (rs < 0) break ;
	            } /* end for */
	            if ((rs >= 0) && (w >= 0)) {
	                rs = b.printf(" width=\"%d\"",w) ;
	            }
	            if ((rs >= 0) && (h >= 0)) {
	                rs = b.printf(" height=\"%d\"",h) ;
	            }
	            b.strw(" /",2) ;
	            b.chr(CH_RANGLE) ;
	            if (rs >= 0) {
	                if ((rs = b.getlen) >= 0) {
	                    rs = shio_print(op->ofp,op->lbuf,rs) ;
	                    wlen += rs ;
	                }
	            }
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	        op->wlen += wlen ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_img) */

int htm_write(htm *op,cvoid *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op,lbuf)) >= 0) {
	    rs = shio_write(op->ofp,lbuf,llen) ;
	    wlen += rs ;
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_write) */

int htm_printline(htm *op,cchar *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op,lbuf)) >= 0) {
	    rs = shio_print(op->ofp,lbuf,llen) ;
	    wlen += rs ;
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_printline) */

int htm_printf(htm *op,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = htm_magic(op)) >= 0) {
	    va_begin(ap,fmt) ;
	    rs = htm_vprintf(op,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (htm_printf) */

int htm_vprintf(htm *op,cchar *fmt,va_list ap) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op)) >= 0) {
	    rs = shio_vprintf(op->ofp,fmt,ap) ;
	    wlen += rs ;
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_vprintf) */

int htm_putc(htm *op,int ch) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op)) >= 0) {
	    rs = shio_putc(op->ofp,ch) ;
	    wlen += rs ;
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_putc) */


/* private subroutines */

int htm_tagalone(htm *op,cchar *tag,cchar *eclass,cchar *id) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op,tag)) >= 0) {
	    rs = SR_INVALID ;
	    if (tag[0]) {
                if (sbuf b ; (rs = b.start(op->lbuf,op->llen)) >= 0) {
                    b.chr(CH_LANGLE) ;
                    b.strw(tag,-1) ;
                    for (int c = 0 ; c < 2 ; c += 1) {
                        cchar       *k = nullptr ;
                        cchar       *v = nullptr ;
                        switch (c) {
                        case 0:
                            k = "class" ;
                            v = eclass ;
                            break ;
                        case 1:
                            k  = "id" ;
                            v  = id ;
                            break ;
                        } /* end switch */
                        if ((v != nullptr) && (v[0] != '\0')) {
                            rs = b.printf(" %s=\"%s\"",k,v) ;
                        }
                        if (rs < 0) break ;
                    } /* end for */
                    b.strw(" /",2) ;
                    b.chr(CH_RANGLE) ;
                    if (rs >= 0) {
                        if ((rs = b.getlen) >= 0) {
                            rs = shio_print(op->ofp,op->lbuf,rs) ;
                            wlen += rs ;
                        }
                    }
                    rs1 = b.finish ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (sbuf) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_tagalone) */

static int htm_printout(htm *op,int c,cchar *bp,int bl) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (linefold f ; (rs = linefold_start(&f,c,1,bp,bl)) >= 0) {
	    int		ll ;
	    cchar	*lp ;
	    for (int i = 0 ; (ll = linefold_get(&f,i,&lp)) >= 0 ; i += 1) {
		if (i > 0) {
		    rs = shio_putc(op->ofp,CH_SP) ;
		    wlen += rs ;
		}
		if (rs >= 0) {
		    rs = shio_print(op->ofp,lp,ll) ;
		    wlen += rs ;
		} /* end if (ok) */
		if (rs < 0) break ;
	    } /* end for */
	    rs1 = linefold_finish(&f) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (linefold) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_printout) */

static int htm_wrfile(htm *op,cchar *cfname) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
        if (shio cf ; (rs = shio_open(&cf,cfname,"r",0)) >= 0) {
            while ((rs = shio_read(&cf,op->lbuf,op->llen)) > 0) {
                rs = shio_write(op->ofp,op->lbuf,rs) ;
                wlen += rs ;
                if (rs < 0) break ;
            } /* end while (reading lines) */
            rs1 = shio_close(&cf) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (copy-content-file) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_wrfile) */


