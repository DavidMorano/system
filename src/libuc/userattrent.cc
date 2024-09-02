/* userattrent SUPPORT */
/* lang=C++20 */

/* subroutines for simple userattr object (from UNIX® library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	userattrent

	Description:
	These subroutines manage some simple tasks for the userattr
	object, referenced as 'userattr'. This object is defined
	by UNIX® (really Solaris®) standards.

	Used structures:

	typedef struct userattr_s {
		char   *name;		# user name
		char   *qualifier;	# reserved for future use
		char   *res1;		# reserved for future use
		char   *res2;		# reserved for future use
		kva_t  *attr;		# array of key-value pair attributes
	} userattr_t;

	typedef struct kva_s {
		int	length;		# array length
		kv_t    *data;		# array of key value pairs
	} kva_t;			# Key-value array

	typedef struct kv_s {
		char   *key;
		char   *value;
	} kv_t;				# a key-value pair


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vecstr.h>
#include	<sfx.h>
#include	<strn.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"userattrent.h"


/* local defines */

#define	SI		storeitem
#define	UA		USERATTR
#define	CUA		CUSERATTR


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int userattrent_parseattr(UA *,storeitem *,cchar *,int) noex ;
static int userattrent_parseattrload(UA *,storeitem *,vecstr *,int) noex ;

static int si_attrload(storeitem *,kv_t *,int,cchar *) noex ;
static int si_copystr(storeitem *,cchar **,cchar *) noex ;

static int sbuf_fmtattrs(sbuf *,kva_t *) noex ;


/* local variables */


/* exported subroutines */

int userattrent_parse(UA *uap,char *uabuf,int ualen,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (uap && uabuf && sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(uap) ;
	    if ((sl > 0) && (sp[0] != '#')) {
	        storeitem	ib, *ibp = &ib ;
	        if ((rs = storeitem_start(ibp,uabuf,ualen)) >= 0) {
	            int		fi = 0 ;
	            cchar	*tp ;
	            while ((tp = strnchr(sp,sl,':')) != nullptr) {
	                cchar	**vpp = nullptr ;
	                switch (fi++) {
	                case 0:
	                    vpp = (cchar **) &uap->name ;
	                    break ;
	                case 1:
	                    vpp = (cchar **) &uap->qualifier ;
	                    break ;
	                case 2:
	                    vpp = (cchar **) &uap->res1 ;
	                    break ;
	                case 3:
	                    vpp = (cchar **) &uap->res1 ;
	                    break ;
	                case 4:
	                    rs = userattrent_parseattr(uap,ibp,sp,(tp-sp)) ;
	                    break ;
	                } /* end switch */
	                if ((rs >= 0) && vpp) {
	                    int		cl ;
	                    cchar	*cp ;
	                    if ((cl = sfshrink(sp,(tp-sp),&cp)) >= 0) {
	                        rs = storeitem_strw(ibp,cp,cl,vpp) ;
	                    }
	                }
	                sl -= ((tp+1)-sp) ;
	                sp = (tp+1) ;
	                if (rs < 0) break ;
	            } /* end while */
	            if ((rs >= 0) && (fi == 4) && sl && sp[0]) {
	                rs = userattrent_parseattr(uap,ibp,sp,sl) ;
	            }
	            rs1 = storeitem_finish(ibp) ;
	            if (rs >= 0) rs = rs1 ;
		    wlen = rs1 ;
	        } /* end if (storeitem) */
	    } /* end if (non-zero non-comment) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (userattrent_parse) */

int userattrent_load(UA *uap,char *uabuf,int ualen,CUA *suap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	if (uap && uabuf && suap) {
	    storeitem	ib, *ibp = &ib ;
	    memcpy(uap,suap,sizeof(userattr)) ;
	    if ((rs = storeitem_start(ibp,uabuf,ualen)) >= 0) {
	        if (suap->attr != nullptr) {
	            cint	ksize = sizeof(kva_t) ;
	            cint	al = sizeof(void *) ;
	            cint	n = suap->attr->length ;
	            void	*p ;
	            if ((rs = storeitem_block(ibp,ksize,al,&p)) >= 0) {
	                kva_t	*kvap = (kva_t *) p ;
	                int	dsize = (n*sizeof(kv_t)) ;
	                uap->attr = kvap ;
	                if ((rs = storeitem_block(ibp,dsize,al,&p)) >= 0) {
	                    kv_t	*kvp = (kv_t *) p ;
	                    uap->attr->length = n ;
	                    uap->attr->data = kvp ;
	                    for (int i = 0 ; i < n ; i += 1) {
	                        cchar	*dp = suap->attr->data[i].key ;
	                        cchar	*rp ;
	                        if ((rs = si_copystr(ibp,&rp,dp)) >= 0) {
	                            kvp[i].key = (char *) rp ;
	                            dp = suap->attr->data[i].value ;
	                            if ((rs = si_copystr(ibp,&rp,dp)) >= 0) {
	                                kvp[i].value = (char *) rp ;
	                            }
	                        }
	                        if (rs < 0) break ;
	                    } /* end for */
	                } /* end if (storeitem_block) */
	            } /* end if (storeitem_block) */
	        } /* end if (attr) */
	        if (rs >= 0) {
	            cchar	*rp ;
	            si_copystr(ibp,&rp,suap->name) ;
	            uap->name = (char *) rp ;
	        }
	        rs1 = storeitem_finish(ibp) ;
	        if (rs >= 0) rs = rs1 ;
		wlen = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (userattrent_load) */

int userattrent_format(UA *uap,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uap && rbuf) {
	    rs = SR_INVALID ;
	    if (rlen > 0) {
	        sbuf	b ;
	        if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	            for (int i = 0 ; i < 5 ; i += 1) {
	                if (i > 0) rs = sbuf_char(&b,':') ;
	                if (rs >= 0) {
	                    switch (i) {
	                    case 0:
	                        rs = sbuf_strw(&b,uap->name,-1) ;
	                        break ;
	                    case 1:
	                        if (uap->qualifier) {
	                            rs = sbuf_strw(&b,uap->qualifier,-1) ;
	                        }
	                        break ;
	                    case 2:
	                        if (uap->res1) {
	                            rs = sbuf_strw(&b,uap->res1,-1) ;
	                        }
	                        break ;
	                    case 3:
	                        if (uap->res2) {
	                            rs = sbuf_strw(&b,uap->res2,-1) ;
	                        }
	                        break ;
	                    case 4:
	                        if (uap->attr) {
	                            rs = sbuf_fmtattrs(&b,uap->attr) ;
	                        }
	                        break ;
	                    } /* end switch */
	                } /* end if */
	                if (rs < 0) break ;
	            } /* end for */
	            rs1 = sbuf_finish(&b) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (userattrent_format) */

int userattrent_size(UA *uap) noex {
	int		rs = SR_FAULT ;
	if (uap) {
	    int		size = 1 ;
	    kva_t	*kvap = uap->attr ;
	    if (uap->name) {
	        size += (strlen(uap->name)+1) ;
	    }
	    if (uap->attr) {
	        kv_t	*kvp = kvap->data ;
	        int	n = kvap->length ;
	        size += sizeof(kva_t) ;
	        for (int i = 0 ; i < n ; i += 1) {
	            size += (strlen(kvp[i].key)+1) ;
	            size += (strlen(kvp[i].value)+1) ;
	        } /* end for */
	        size += ((n+1)*sizeof(kv_t)) ;
	    } /* end if */
	    rs = iceil(size,sizeof(const char *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (userattrent_size) */


/* local subroutines */

static int userattrent_parseattr(UA *uap,SI *ibp,cc *sp,int sl) noex {
	vecstr		attrs, *alp = &attrs ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecstr_start(alp,0,0)) >= 0) {
	    cint	sch = ';' ;
	    cchar	*tp ;
	    while ((tp = strnchr(sp,sl,sch)) != nullptr) {
	        if ((tp-sp) > 0) {
		    c += 1 ;
	            rs = vecstr_add(alp,sp,(tp-sp)) ;
	        }
	        sl -= ((tp+1)-sp) ;
	        sp = (tp+1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (sl > 0)) {
		c += 1 ;
	        rs = vecstr_add(alp,sp,sl) ;
	    }
	    if ((rs >= 0) && ((rs = vecstr_count(alp)) > 0)) {
	        rs = userattrent_parseattrload(uap,ibp,alp,rs) ;
	    }
	    rs1 = vecstr_finish(alp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return rs ;
}
/* end subroutine (userattrent_parseattr) */

static int userattrent_parseattrload(UA *uap,SI *ibp,vecstr *alp,int n) noex {
	cint		ksize = sizeof(kva_t) ;
	cint		al = sizeof(void *) ;
	int		rs ;
	void		*p ;
	if ((rs = storeitem_block(ibp,ksize,al,&p)) >= 0) {
	    kva_t	*kvap = (kva_t *) p ;
	    int		dsize = (n*sizeof(kv_t)) ;
	    uap->attr = kvap ;
	    if ((rs = storeitem_block(ibp,dsize,al,&p)) >= 0) {
	        kv_t	*kvp = (kv_t *) p ;
	        cchar	*ep ;
	        uap->attr->length = n ;
	        uap->attr->data = kvp ;
	        for (int i = 0 ; vecstr_get(alp,i,&ep) >= 0 ; i += 1) {
	            if (ep) {
	                rs = si_attrload(ibp,kvp,i,ep) ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (storeitem_block) */
	} /* end if (storeitem_block) */
	return rs ;
}
/* end subroutine (userattrent_parseattrload) */

static int si_attrload(SI *ibp,kv_t *kvp,int i,cchar *ep) noex {
	int		rs ;
	int		el = -1 ;
	const char	*tp ;
	const char	*rp ;
	const char	*vp ;
	if ((tp = strchr(ep,'=')) != nullptr) {
	    vp = (tp+1) ;
	    el = (tp-ep) ;
	} else {
	    vp = (ep+strlen(ep)) ;
	}
	if ((rs = storeitem_strw(ibp,ep,el,&rp)) >= 0) {
	    kvp[i].key = (char *) rp ;
	    if ((rs = si_copystr(ibp,&rp,vp)) >= 0) {
	        kvp[i].value = (char *) rp ;
	    }
	}
	return rs ;
}
/* end subroutine (si_attrload) */

static int si_copystr(SI *ibp,cchar **pp,cchar *p1) noex {
	int		rs = SR_OK ;
	cchar		**cpp = (cchar **) pp ;
	*cpp = nullptr ;
	if (p1) {
	    rs = storeitem_strw(ibp,p1,-1,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copystr) */

static int sbuf_fmtattrs(sbuf *bp,kva_t *attr) noex {
	int		rs = SR_FAULT ;
	if (bp && attr) {
	    kv_t	*kv = attr->data ;
	    cint	n = attr->length ;
	    cint	sch = ';' ;
	    rs = SR_OK ;
	    for (int i = 0 ; i < n ; i += 1) {
	        if (kv[i].key != nullptr) {
	            if (i > 0) rs = sbuf_char(bp,sch) ;
	            if (rs >= 0) rs = sbuf_strw(bp,kv[i].key,-1) ;
	            if ((rs >= 0) && (kv[i].value != nullptr)) {
	                rs = sbuf_char(bp,'=') ;
	                if (rs >= 0) rs = sbuf_strw(bp,kv[i].value,-1) ;
	            }
	        } /* end if */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (non-null attr) */
	return rs ;
}
/* end subroutine (sbuf_fmtattrs) */


