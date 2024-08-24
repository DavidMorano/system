/* htm SUPPORT (HTML creation and output) */
/* lang=C++20 */

/* hack to output HTML */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a hack to create and output some basic HTML.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<dlfcn.h>
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
#include	<localmisc.h>

#include	"htm.h"


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	COLUMNS
#define	COLUMNS		80
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


/* external subroutines */


/* local structures */


/* forward references */

template<typename ... Args>
static int htm_ctor(htm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
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

static int	htm_tagalone(HTM *,cchar *,cchar *,cchar *) noex ;
static int	htm_printout(HTM *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int htm_start(HTM *op,shio *ofp,cchar *lang) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = htm_ctor(op,ofp)) >= 0) {
	    op->ofp = ofp ;
	    if ((rs = shio_print(op->ofp,"<!doctype html>",-1)) >= 0) {
	        char	*lbuf{} ;
	        wlen += rs ;
		if ((rs = malloc_ml(&lbuf)) >= 0) {
		    cint	llen = rs ;
		    cchar	*fmt ;
	            if ((lang != NULL) && (lang[0] != '\0')) {
			fmt = "<html lang=\"%s\">" ;
	                rs = bufprintf(lbuf,llen,fmt,lang) ;
	            } else {
			fmt = "<html>" ;
	                rs = bufprintf(lbuf,llen,fmt) ;
	            }
	            if (rs >= 0) {
	                if ((rs = shio_print(op->ofp,lbuf,rs)) >= 0) {
	                    wlen += rs ;
	    		    op->magic = HTM_MAGIC ;
		        }
	            }
		    rs1 = uc_free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (doctype) */
	    op->wlen += wlen ;
	    if (rs < 0) {
		htm_dtor(op) ;;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_start) */

int htm_finish(HTM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen ;

	if (op == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;

	rs1 = htm_printline(op,"</html>",-1) ;
	if (rs >= 0) rs = rs1 ;

	wlen = op->wlen ;
	op->ofp = NULL ;
	op->magic = 0 ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_finish) */

int htm_headbegin(HTM *op,cchar *cfname) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar	*sp = "<head>" ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	if ((rs = shio_print(op->ofp,sp,-1)) >= 0) {
	    wlen += rs ;
	    if ((cfname != NULL) && (cfname[0] != '\0')) {
	        shio	cf ;
	        if ((rs = shio_open(&cf,cfname,"r",0666)) >= 0) {
	            cint	llen = LINEBUFLEN ;
	            char	lbuf[LINEBUFLEN+1] ;
	            while ((rs = shio_read(&cf,lbuf,llen)) > 0) {
	                rs = shio_write(op->ofp,lbuf,rs) ;
	                wlen += rs ;
	                if (rs < 0) break ;
	            } /* end while (reading lines) */
	            rs1 = shio_close(&cf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (copy-content-file) */
	    } /* end if (have content-file) */
	} /* end if (shio_print) */

	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_headbegin) */

int htm_headend(HTM *op) noex {
	int		rs ;
	int		wlen = 0 ;
	cchar	*sp = "</head>" ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	rs = shio_print(op->ofp,sp,-1) ;
	wlen += rs ;
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_headend) */

int htm_bodybegin(HTM *op,cchar *cfname) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar	*sp = "<body>" ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	if ((rs = shio_print(op->ofp,sp,-1)) >= 0) {
	    wlen += rs ;
	    if ((cfname != NULL) && (cfname[0] != '\0')) {
	        shio	cf ;
	        if ((rs = shio_open(&cf,cfname,"r",0666)) >= 0) {
	            cint	llen = LINEBUFLEN ;
	            char	lbuf[LINEBUFLEN+1] ;
	            while ((rs = shio_read(&cf,lbuf,llen)) > 0) {
	                rs = shio_write(op->ofp,lbuf,rs) ;
	                wlen += rs ;
	                if (rs < 0) break ;
	            } /* end while (reading lines) */
	            rs1 = shio_close(&cf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (copy-content-file) */
	    } /* end if (have content-file) */
	} /* end if (shio_print) */

	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_bodybegin) */

int htm_bodyend(HTM *op) noex {
	int		rs ;
	int		wlen = 0 ;
	cchar	*sp = "</body>" ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	rs = shio_print(op->ofp,sp,-1) ;
	wlen += rs ;
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_bodyend) */

int htm_tagbegin(HTM *op,cc *tag,cc *eclass,cc *id,cc *(*kv)[2]) noex {
	buffer		b ;
	cint	c = COLUMNS ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (op == NULL) return SR_FAULT ;
	if (tag == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	if (tag[0] == '\0') return SR_INVALID ;
	if ((rs = buffer_start(&b,c)) >= 0) {
	    int		i ;
	    cchar	*k, *v ;
	    buffer_chr(&b,CH_LANGLE) ;
	    buffer_strw(&b,tag,-1) ;
	    for (i = 0 ; i < 2 ; i += 1) {
	        v = NULL ;
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
	        if ((v != NULL) && (v[0] != '\0')) {
	            rs = buffer_printf(&b," %s=\"%s\"",k,v) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (kv != NULL)) {
		cchar	*fmt ;
	        for (i = 0 ; kv[i][0] != NULL ; i += 1) {
		    if (kv[i][1] != NULL) {
			if (kv[i][1][0] != '\0') {
	                    fmt = " %s=\"%s\"" ;
			} else {
	                    fmt = " %s=" ;
			}
		    } else {
	                fmt = " %s" ;
		    }
	            rs = buffer_printf(&b,fmt,kv[i][0],kv[i][1]) ;
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (key-vals) */
	    buffer_chr(&b,CH_RANGLE) ;
	    if (rs >= 0) {
		cchar	*bp ;
	        if ((rs = buffer_get(&b,&bp)) >= 0) {
		    rs = htm_printout(op,c,bp,rs) ;
		    wlen += rs ;
	        } /* enbd if (buffer_get) */
	    } /* end if (ok) */
	    rs1 = buffer_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (buffer) */
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_tagbegin) */

int htm_tagend(HTM *op,cchar *tag) noex {
	cint	llen = LINEBUFLEN ;
	int		rs ;
	int		wlen = 0 ;
	char		lbuf[LINEBUFLEN+1] ;
	if (op == NULL) return SR_FAULT ;
	if (tag == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	if (tag[0] == '\0') return SR_INVALID ;
	if ((rs = bufprintf(lbuf,llen,"</%s>",tag)) >= 0) {
	    rs = shio_print(op->ofp,lbuf,rs) ;
	    wlen += rs ;
	}
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_tagend) */

int htm_textbegin(HTM *op,cc *eclass,cc *id,cc *title,
		int r,int c,cchar *(*tkv)[2]) noex {
	cint	dlen = DIGBUFLEN ;
	int		rs ;
	int		rs1 ;
	int		bsz = 0 ;
	int		kvsize = 0 ;
	cchar	*tag = "textarea" ;
	void		*vp ;

	if (tkv != NULL) {
	    int	i ;
	    for (i = 0 ; tkv[i][0] != NULL ; i += 1) ;
	    kvsize = (i*(2*sizeof(cchar *))) ;
	}
	kvsize += (6*(2*sizeof(cchar *))) ;
	bsz += kvsize ;

	bsz += ((dlen+1)*2) ;
	if ((rs = uc_malloc(bsz,&vp)) >= 0) {
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
		    if (title != NULL) {
	                kv[i][0] = "title" ;
	                kv[i][1] = title ;
		        i += 1 ;
		    }
		    if (tkv != NULL) {
			int	j ;
			for (j = 0 ; tkv[j][0] != NULL ; j += 1) {
			    kv[i][0] = tkv[j][0] ;
			    kv[i][1] = tkv[j][1] ;
			    i += 1 ;
			} /* end for */
		    } /* end if (have extras) */
	            kv[i][0] = NULL ;
	            kv[i][1] = NULL ;
	            rs = htm_tagbegin(op,tag,eclass,id,kv) ;
	        } /* end if (ctdeci) */
	    } /* end if (ctdeci) */

	    rs1 = uc_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */

	return rs ;
}
/* end subroutine (htm_textbegin) */

int htm_textend(HTM *op) noex {
	cchar	*tag = "textarea" ;
	return htm_tagend(op,tag) ;
}
/* end subroutine (htm_textend) */

int htm_abegin(HTM *op,cc *eclass,cc *id,cc *href,cc *title) noex {
	sbuf		b ;
	cint	llen = LINEBUFLEN ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar	*tag = "a" ;
	char		lbuf[LINEBUFLEN+1] ;
	if (op == NULL) return SR_FAULT ;
	if (href == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	if (href[0] == '\0') return SR_INVALID ;
	if ((rs = sbuf_start(&b,lbuf,llen)) >= 0) {
	    int		c ;
	    cchar	*k, *v ;
	    sbuf_chr(&b,CH_LANGLE) ;
	    sbuf_strw(&b,tag,-1) ;
	    for (c = 0 ; c < 4 ; c += 1) {
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
	        if ((v != NULL) && (v[0] != '\0')) {
	            rs = sbuf_printf(&b,"\n %s=\"%s\"",k,v) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	    sbuf_chr(&b,CH_RANGLE) ;
	    if ((rs = sbuf_getlen(&b)) >= 0) {
	        rs = shio_print(op->ofp,lbuf,rs) ;
	        wlen += rs ;
	    }
	    rs1 = sbuf_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_abegin) */

int htm_aend(HTM *op) noex {
	cint	llen = LINEBUFLEN ;
	int		rs ;
	int		wlen = 0 ;
	cchar	*tag = "a" ;
	char		lbuf[LINEBUFLEN+1] ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	if ((rs = bufprintf(lbuf,llen,"</%s>",tag)) >= 0) {
	    rs = shio_print(op->ofp,lbuf,rs) ;
	    wlen += rs ;
	}
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_aend) */

int htm_hr(HTM *op,cchar *eclass,cchar *id) noex {
	return htm_tagalone(op,"hr",eclass,id) ;
}
/* end subroutine (htm_hr) */

int htm_br(HTM *op,cchar *eclass,cchar *id) noex {
	return htm_tagalone(op,"br",eclass,id) ;
}
/* end subroutine (htm_br) */

int htm_img(HTM *op,cc *eclass,cc *id,cc *src,cc *title,cc *alt,
		int w,int h) noex {
	sbuf		b ;
	cint	llen = LINEBUFLEN ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar	*tag = "img" ;
	char		lbuf[LINEBUFLEN+1] ;
	if (op == NULL) return SR_FAULT ;
	if (src == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	if (tag[0] == '\0') return SR_INVALID ;
	if ((rs = sbuf_start(&b,lbuf,llen)) >= 0) {
	    int		c ;
	    cchar	*k ;
	    cchar	*v ;
	    sbuf_chr(&b,CH_LANGLE) ;
	    sbuf_strw(&b,tag,-1) ;
	    for (c = 0 ; c < 5 ; c += 1) {
	        v = NULL ;
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
	        if ((v != NULL) && (v[0] != '\0')) {
	            rs = sbuf_printf(&b,"\n %s=\"%s\"",k,v) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (w >= 0)) {
	        rs = sbuf_printf(&b," width=\"%d\"",w) ;
	    }
	    if ((rs >= 0) && (h >= 0)) {
	        rs = sbuf_printf(&b," height=\"%d\"",h) ;
	    }
	    sbuf_strw(&b," /",2) ;
	    sbuf_chr(&b,CH_RANGLE) ;
	    if (rs >= 0) {
	        if ((rs = sbuf_getlen(&b)) >= 0) {
	            rs = shio_print(op->ofp,lbuf,rs) ;
	            wlen += rs ;
	        }
	    }
	    rs1 = sbuf_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_img) */

int htm_write(HTM *op,cvoid *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if (op == NULL) return SR_FAULT ;
	if (lbuf == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	rs = shio_write(op->ofp,lbuf,llen) ;
	wlen += rs ;
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_write) */

int htm_printline(HTM *op,cchar *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if (op == NULL) return SR_FAULT ;
	if (lbuf == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	rs = shio_print(op->ofp,lbuf,llen) ;
	wlen += rs ;
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_printline) */

int htm_printf(HTM *op,cchar *fmt,...) noex {
	int		rs = SR_FAULT ;
	va_list		ap ;
	if (op) {
	    va_begin(ap,fmt) ;
	    rs = htm_vprintf(op,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}
/* end subroutine (htm_printf) */

int htm_vprintf(HTM *op,cchar *fmt,va_list ap) noex {
	int		rs ;
	int		wlen = 0 ;
	if (op == NULL) return SR_FAULT ;
	if (op->magic != HTM_MAGIC) return SR_NOTOPEN ;
	rs = shio_vprintf(op->ofp,fmt,ap) ;
	wlen += rs ;
	op->wlen += wlen ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_vprintf) */

int htm_putc(HTM *op,int ch) noex {
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

int htm_tagalone(HTM *op,cchar *tag,cchar *eclass,cchar *id) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = htm_magic(op,tag)) >= 0) {
	    rs = SR_INVALID ;
	    if (tag[0]) {
	        char	*lbuf{} ;
	        if ((rs = malloc_ml(&lbuf)) >= 0) {
	            sbuf	b ;
		    cint	llen = rs ;
	            if ((rs = sbuf_start(&b,lbuf,llen)) >= 0) {
	                int		c ;
	                cchar	*k ;
	                cchar	*v ;
	                sbuf_chr(&b,CH_LANGLE) ;
	                sbuf_strw(&b,tag,-1) ;
	                for (c = 0 ; c < 2 ; c += 1) {
	                    v = NULL ;
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
	                    if ((v != NULL) && (v[0] != '\0')) {
	                        rs = sbuf_printf(&b," %s=\"%s\"",k,v) ;
	                    }
	                    if (rs < 0) break ;
	                } /* end for */
	                sbuf_strw(&b," /",2) ;
	                sbuf_chr(&b,CH_RANGLE) ;
	                if (rs >= 0) {
	                    if ((rs = sbuf_getlen(&b)) >= 0) {
	                        rs = shio_print(op->ofp,lbuf,rs) ;
	                        wlen += rs ;
	                    }
	                }
	                rs1 = sbuf_finish(&b) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (sbuf) */
		    op->wlen += wlen ;
		    rs1 = uc_free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (htm_tagalone) */

static int htm_printout(HTM *op,int c,cchar *bp,int bl) noex {
	linefold	f ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = linefold_start(&f,c,1,bp,bl)) >= 0) {
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


