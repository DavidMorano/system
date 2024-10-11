/* mimetypes SUPPORT */
/* encoding=ISO8859-1 */
/* version %I% last-modified %G% */

/* manage a MIME-type database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-19, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module manages a MIME-type database. It reads
	in MIME-type formatted files with the |mimetypes_file()|
	subroutine and provides other subroutines for database
	queries.

	Note on file parsing:

	The MIME content type field (the first field of the file)
	*must* be a single contiguous string without any white-space
	in it. This may be obvious but it should be observed when
	creating 'mimetypes' files. This code does not (currently)
	try to piece together a content type from pieces separated
	by white space (we can't do everything!).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<hdb.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<strn.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<localmisc.h>

#include	"mimetypes.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */

typedef mimetypes	mt ;
typedef mimetypes_cur	mt_cur ;
typedef mimetypes_dat	mt_dat ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	exttypespec(cchar *,int,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mimetypes_start(mt *dbp) noex {
	int		rs = SR_FAULT ;
	if (dbp) {
	    cint	ne = MIMETYPES_NUMKEYS ;
	    rs = hdb_start(dbp,ne,1,nullptr,nullptr) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mimetypes_start) */

int mimetypes_finish(mt *dbp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbp) {
	    mt_cur	cur ;
	    mt_dat	key ;
	    mt_dat	data ;
	    if ((rs = hdb_curbegin(dbp,&cur)) >= 0) {
	        while (hdb_enum(dbp,&cur,&key,&data) >= 0) {
	            if (key.buf != nullptr) {
		        void	*vp = voidp(key.buf) ;
	                rs1 = uc_free(vp) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end while */
	        rs1 = hdb_curend(dbp,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	    {
	        rs1 = hdb_finish(dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mimetypes_finish) */

/* read a whole file into the database */
int mimetypes_file(mt *dbp,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
        int             c = 0 ;
	if (dbp && fname) {
	    cnullptr	np{} ;
            int             ctl ;
            int             size ;
            int             cl, fl ;
            cchar   *fp ;
            cchar   *cp ;
            cchar   *ctp ;
	    char	*lbuf{} ;
	    if ((rs = malloc_ml(&lbuf)) >= 0) {
                bfile	mfile, *mfp = &mfile ;
                char	terms[32] ;
		cint	llen = rs ;
                fieldterms(terms,false," \t#") ;
                if ((rs = bopen(mfp,fname,"r",0666)) >= 0) {
                    mt_dat      key ;
                    mt_dat      data ;
                    while ((rs = breadln(mfp,lbuf,llen)) > 0) {
                        field       fb ;
                        int		len = rs ;
                        if (lbuf[len - 1] == '\n') len -= 1 ;
                        lbuf[len] = '\0' ;
                        if (len == 0) continue ;
    /* extract the typespec (MIME content type) */
                        ctl = exttypespec(lbuf,len,&ctp) ;
                        if (ctl <= 0) continue ;
    /* get the file suffixes for this MIME content type */
                        cp = ctp + ctl ;
                        cl = len - (cp - lbuf) ;
                        if ((rs = field_start(&fb,cp,cl)) >= 0) {
                            cchar       *kp{} ;
                            while ((fl = field_get(&fb,terms,&fp)) >= 0) {
                                if (fl > 0) {
                                    key.buf = fp ;
                                    key.len = fl ;
                                    if (hdb_fetch(dbp,key,np,&data) < 0) {
                                        char    *bkp, *bvp ;
                                        size = key.len + 1 + ctl + 1 ;
                                        rs = uc_malloc(size,&bkp) ;
                                        if (rs < 0) break ;
                                        kp = charp(key.buf) ;
                                        bvp = strwcpy(bkp,kp,key.len) + 1 ;
                                        key.buf = bkp ;
                                        strwcpy(bvp,ctp,ctl) ;
                                        data.len = ctl ;
                                        data.buf = bvp ;
                                        rs = hdb_store(dbp,key,data) ;
                                        if (rs < 0) {
                                            uc_free(bkp) ;
                                            break ;
                                        }
                                        c += 1 ;
                                    } /* end if */
                                } /* end if */
                                if (fb.term == '#') break ;
                                if (rs < 0) break ;
                            } /* end while (reading fields) */
                            rs1 = field_finish(&fb) ;
                            if (rs >= 0) rs = rs1 ;
                        } /* end if (field) */
                        if (rs < 0) break ;
                    } /* end while (reading lines) */
                    rs1 = bclose(mfp) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (bfile) */
		rs1 = uc_free(lbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mimetypes_file) */

/* find a MIME type for the given extension */
int mimetypes_find(mt *dbp,char *typespec,cchar *ext) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ext) {
	    cchar	*tp = ext ;
	    rs = SR_NOTFOUND ;
	    if (typespec) typespec[0] = '\0' ;
	    if ((tp = strrchr(ext,'.')) != nullptr) {
	        tp += 1 ;
	    }
	    if (tp[0] != '\0') {
	        mt_dat	key ;
	        mt_dat	data ;
	        key.len = -1 ;
	        key.buf = tp ;
	        if ((rs = hdb_fetch(dbp,key,nullptr,&data)) >= 0) {
	            if (typespec != nullptr) {
	                cint	typelen = MIMETYPES_TYPELEN ;
		        cchar	*valp = charp(data.buf) ;
	                rs = snwcpy(typespec,typelen,valp,data.len) ;
	                len = rs ;
	            }
	        }
	    } /* end if (found) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mimetypes_find) */

int mimetypes_curbegin(mt *dbp,mt_cur *cp) noex {
	return hdb_curbegin(dbp,cp) ;
}
/* end subroutine (mimetypes_curbegin) */

int mimetypes_curend(mt *dbp,mt_cur *cp) noex {
	return hdb_curend(dbp,cp) ;
}
/* end subroutine (mimetypes_curend) */

/* enumerate all of the key-val pairs */
int mimetypes_enum(mt *dbp,mt_cur *curp,char *ext,char *ts) noex {
	int		rs = SR_FAULT ;
	if (dbp && curp && ext) {
	    mt_dat	key ;
	    mt_dat	val ;
	    ext[0] = '\0' ;
	    if (ts) ts[0] = '\0' ;
	    if ((rs = hdb_enum(dbp,curp,&key,&val)) >= 0) {
	        cint	ml = min(key.len,MIMETYPES_TYPELEN) ;
	        cchar	*kp = charp(key.buf) ;
	        strwcpy(ext,kp,ml) ;
	        rs = min(val.len,MIMETYPES_TYPELEN) ;
	        if (ts != nullptr) {
		    cchar	*valp = charp(val.buf) ;
		    strwcpy(ts,valp,rs) ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mimetypes_enum) */


/* fetch the next val by extension and a possible cursor */
int mimetypes_fetch(mt *dbp,mt_cur *curp,
		char *ext,char *ts) noex {
	mt_dat	key, val ;
	int		rs ;

	if (dbp == nullptr) return SR_FAULT ;
	if ((ext == nullptr) || (ts == nullptr)) return SR_FAULT ;

	key.buf = ext ;
	key.len = -1 ;
	ts[0] = '\0' ;
	if ((rs = hdb_fetch(dbp,key,curp,&val)) >= 0) {
	    cint	ml = min(val.len,MIMETYPES_TYPELEN) ;
	    cchar	*mp = charp(val.buf) ;
	    rs = (strwcpy(ts,mp,ml) - ts) ;
	}

	return rs ;
}
/* end subroutine (mimetypes_fetch) */

/* OLDER API: find a MIME type given an extension */
int mimetypes_get(mt *dbp,char *typespec,cchar *ext) noex {
	return mimetypes_find(dbp,typespec,ext) ;
}
/* end subroutine (mimetypes_get) */


/* private subroutines */

/* extract the typespec from this buffer */
static int exttypespec(cchar *tbuf,int tlen,cchar **rpp) noex {
	int		spec = -1 ;
	int		cl = tlen ;
	cchar		*cp = tbuf ;
	cchar		*sp ;
	while (CHAR_ISWHITE(*cp) && (cl > 0)) {
	    cp += 1 ;
	    cl -= 1 ;
	}
	if (*cp != '#') {
	    sp = cp ;
	    while (*cp && (cl > 0) && (! CHAR_ISWHITE(*cp)) && (*cp != '#')) {
	        cp += 1 ;
	        cl -= 1 ;
	    }
	    *rpp = sp ;
	    spec = (cp - sp) ;
	} /* end if */
	return spec ;
}
/* end subroutine (exttypespec) */


