/* mimetypes SUPPORT */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* manage a MIME-type database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-19, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mimetypes

	Description:
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<hdb.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<strn.h>
#include	<sfx.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<localmisc.h>

#include	"mimetypes.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mimetypes	mt ;
typedef mimetypes_cur	mt_cur ;
typedef mimetypes_dat	mt_dat ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		typelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static inline int mimetypes_magic(mimetypes *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MIMETYPES_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mimetypes_magic) */

static int	mimetypes_fileln(mt *,cchar *,int) noex ;

static int	mkterms() noex ;
static int	exttypespec(cchar *,int,cchar **) noex ;


/* local variables */

static vars		var ;

static char		terms[fieldterms_termsize] ;


/* exported variables */


/* exported subroutines */

int mimetypes_start(mt *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
		cint	hsz = szof(hdb) ;
		op->typelen = var.typelen ;
		if (void *vp ; (rs = libmem.mall(hsz,&vp)) >= 0) {
	            cint	ne = MIMETYPES_NUMKEYS ;
		    op->dbp = (hdb *) vp ;
	            if ((rs = hdb_start(op->dbp,ne,1,nullptr,nullptr)) >= 0) {
			op->magic = MIMETYPES_MAGIC ;
		    }
		    if (rs < 0) {
			libmem.free(op->dbp) ;
			op->dbp = nullptr ;
		    } /* end if (error) */
		} /* end if (memory-allocation) */
	    } /* end if (vars) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mimetypes_start) */

int mimetypes_finish(mt *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rs2 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbp) {
	        hdb	*dbp = op->dbp ;
	        if (mt_cur cur ; (rs = hdb_curbegin(dbp,&cur)) >= 0) {
	            mt_dat	key ;
	            mt_dat	data ;
	            while ((rs2 = hdb_curenum(dbp,&cur,&key,&data)) >= 0) {
	                if (key.buf != nullptr) {
		            void	*vp = voidp(key.buf) ;
	                    rs1 = libmem.free(vp) ;
	                    if (rs >= 0) rs = rs1 ;
	                }
	            } /* end while */
		    if ((rs >= 0) && (rs2 != SR_NOTFOUND)) rs = rs2 ;
	            rs1 = hdb_curend(dbp,&cur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if */
	        {
	            rs1 = hdb_finish(dbp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
		{
		    rs1 = libmem.free(op->dbp) ;
		    if (rs >= 0) rs = rs1 ;
		    op->dbp = nullptr ;
		}
	    }
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mimetypes_finish) */

/* read a whole file into the database */
int mimetypes_file(mt *op,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
        int             c = 0 ;
	if ((rs = mimetypes_magic(op,fname)) >= 0) {
	    if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
		cint	llen = rs ;
                if (bfile mf ; (rs = mf.open(fname,"r",0666)) >= 0) {
                    while ((rs = mf.readln(lbuf,llen)) > 0) {
			cchar	*cp ;
			if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
			    rs = mimetypes_fileln(op,cp,cl) ;
			    c = rs ;
			}
                        if (rs < 0) break ;
                    } /* end while (reading lines) */
                    rs1 = mf.close ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (bfile) */
		rs1 = malloc_free(lbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mimetypes_file) */

static int mimetypes_fileln(mt *op,cchar *lbuf,int ll) noex {
	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
    	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*ctp{} ;
	/* extract the typespec (MIME content type) */
	if (int ctl ; (ctl = exttypespec(lbuf,ll,&ctp)) > 0) {
            int		cl, fl ;
            cchar	*cp = ctp + ctl ;
            /* get the file suffixes for this MIME content type */
            cl = ll - intconv(cp - lbuf) ;
	    if (field fb ; (rs = fb.start(cp,cl)) >= 0) {
		cchar	*fp ;
		while ((fl = fb.get(terms,&fp)) >= 0) {
		    if (fl > 0) {
            		mt_dat	key ;
            		mt_dat	data ;
			cchar	*kp{} ;
			key.buf = fp ;
			key.len = fl ;
                        if ((rs = hdb_fetch(op->dbp,key,np,&data)) == rsn) {
            		    cint	sz = (key.len + 1 + ctl + 1) ;
                            char    *bkp ;
                            char    *bvp ;
                            rs = libmem.mall(sz,&bkp) ;
                            if (rs < 0) break ;
                            kp = charp(key.buf) ;
                            bvp = strwcpy(bkp,kp,key.len) + 1 ;
                            key.buf = bkp ;
                            strwcpy(bvp,ctp,ctl) ;
                            data.len = ctl ;
                            data.buf = bvp ;
                            rs = hdb_store(op->dbp,key,data) ;
                            if (rs < 0) {
                                libmem.free(bkp) ;
                                break ;
                            } /* end if (error) */
                            c += 1 ;
			} /* end if */
                    } /* end if (non-zero field) */
 		    if (fb.term == '#') break ;
		    if (rs < 0) break ;
		} /* end while (reading fields) */
		rs1 = fb.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	} /* end if (exttypespec) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mimetypes_fileln) */

/* find a MIME type for the given extension */
int mimetypes_find(mt *op,char *typespec,cchar *ext) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = mimetypes_magic(op,ext)) >= 0) {
	    cchar	*tp ;
	    rs = SR_NOTFOUND ;
	    if (typespec) typespec[0] = '\0' ;
	    if ((tp = strrchr(ext,'.')) != nullptr) {
	        tp += 1 ;
	    } else {
		tp = ext ;
	    }
	    if (tp[0] != '\0') {
	        mt_dat	key ;
	        mt_dat	data ;
	        key.len = -1 ;
	        key.buf = tp ;
	        if ((rs = hdb_fetch(op->dbp,key,nullptr,&data)) >= 0) {
	            if (typespec != nullptr) {
	                cint	typelen = var.typelen ;
		        cchar	*valp = charp(data.buf) ;
	                rs = snwcpy(typespec,typelen,valp,data.len) ;
	                len = rs ;
	            }
	        }
	    } /* end if (found) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mimetypes_find) */

int mimetypes_curbegin(mt *op,mt_cur *curp) noex {
    	int		rs ;
	if ((rs = mimetypes_magic(op,curp)) >= 0) {
	    rs = hdb_curbegin(op->dbp,curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mimetypes_curbegin) */

int mimetypes_curend(mt *op,mt_cur *curp) noex {
    	int		rs ;
	if ((rs = mimetypes_magic(op,curp)) >= 0) {
	    rs = hdb_curend(op->dbp,curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mimetypes_curend) */

/* enumerate all of the key-val pairs */
int mimetypes_enum(mt *op,mt_cur *curp,char *ext,char *ts) noex {
	int		rs ;
	if ((rs = mimetypes_magic(op,curp,ext)) >= 0) {
	    mt_dat	key{} ;
	    mt_dat	val{} ;
	    ext[0] = '\0' ;
	    if (ts) ts[0] = '\0' ;
	    if ((rs = hdb_curenum(op->dbp,curp,&key,&val)) >= 0) {
	        cint	ml = min(key.len,var.typelen) ;
	        cchar	*kp = charp(key.buf) ;
	        strwcpy(ext,kp,ml) ;
	        rs = min(val.len,var.typelen) ;
	        if (ts != nullptr) {
		    cchar	*valp = charp(val.buf) ;
		    strwcpy(ts,valp,rs) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mimetypes_enum) */

/* fetch the next val by extension and a possible cursor */
int mimetypes_fetch(mt *op,mt_cur *curp,char *ext,char *ts) noex {
	int		rs ;
	if ((rs = mimetypes_magic(op,curp,ext,ts)) >= 0) {
	    mt_dat	key ;
	    mt_dat	val{} ;
	    key.buf = ext ;
	    key.len = -1 ;
	    ts[0] = '\0' ;
	    if ((rs = hdb_fetch(op->dbp,key,curp,&val)) >= 0) {
	        cint	ml = min(val.len,var.typelen) ;
	        cchar	*mp = charp(val.buf) ;
	        rs = intconv(strwcpy(ts,mp,ml) - ts) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mimetypes_fetch) */

/* OLDER API: find a MIME type given an extension */
int mimetypes_get(mt *op,char *typespec,cchar *ext) noex {
	return mimetypes_find(op,typespec,ext) ;
}
/* end subroutine (mimetypes_get) */


/* private subroutines */

/* extract the typespec from this buffer */
static int exttypespec(cchar *tbuf,int tlen,cchar **rpp) noex {
	int		spec = -1 ;
	int		cl = tlen ;
	cchar		*cp = tbuf ;
	while (CHAR_ISWHITE(*cp) && (cl > 0)) {
	    cp += 1 ;
	    cl -= 1 ;
	}
	if (*cp != '#') {
	    cchar	*sp = cp ;
	    while (*cp && (cl > 0) && (! CHAR_ISWHITE(*cp)) && (*cp != '#')) {
	        cp += 1 ;
	        cl -= 1 ;
	    }
	    *rpp = sp ;
	    spec = intconv(cp - sp) ;
	} /* end if */
	return spec ;
}
/* end subroutine (exttypespec) */

static int mkterms() noex {
    	return fieldterms(terms,false," \t#") ;
}

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	    typelen = rs ;	/* "TYPELEN" is set to MAXNAMELEN */
	    rs = mkterms() ;
	}
	return rs ;
}
/* end method (vars::operator) */


