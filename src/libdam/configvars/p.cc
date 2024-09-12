/* configvars_parse SUPPORT */
/* lang=C++20 */

/* Configuration-Variables - Parse */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an object that reads configuration files and organizes
	the content into the object for structured access.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<usupport.h>
#include	<bfile.h>
#include	<bfliner.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<field.h>
#include	<fieldterm.h>		/* |fieldterm_termsize(3uc)| */
#include	<matostr.h>
#include	<char.h>
#include	<localmisc.h>

#include	"configvarsobj.hh"


/* local defines */

#define	KEYBUFLEN	20


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine (template) */
using std::max ;			/* subroutine (template) */
using std::nothrow ;			/* constant */

using namespace		configvars_obj ;


/* local typedefs */

typedef	CV_FILE *	filep ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct keytyper {
	cchar	*type[keytype_overlast + 1] ;
	constexpr keytyper() noex {
	    type[keytype_define]	= "define" ;
	    type[keytype_export]	= "export" ;
	    type[keytype_set]		= "set" ;
	    type[keytype_unset]		= "unset" ;
	    type[keytype_overlast]	= nullptr ;
	} ;
    } ; /* end struct (keytyper) */
}

namespace {
    struct confparser {
        CV		*cvp ;
	vecobj		*eep ;
	int		fi ;
        confparser(CV *cp,int idx,vecobj *ep) noex : cvp(cp), eep(ep) {
	    fi = idx ;
	} ;
	operator int () noex ;
	int parsing(filep,bfile *) noex ;
	int parseln(filep,cchar *,int) noex ;
	int parse_define(filep,field *,cc *,int) noex ;
	int parse_export(filep,field *,cc *,int) noex ;
	int parse_set(filep,field *,cc *,int) noex ;
	int parse_unset(filep,field *,cc *,int) noex ;
    } ; /* end struct (confparser) */
}


/* forward references */


/* local variables */

constexpr cchar		fterms[] = {
	0x00, 0x1B, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

static const keytyper	key ;

constexpr int		keylen = KEYBUFLEN ;


/* exported variables */


/* exported subroutines */

namespace configvars_obj {
    int configvars_parse(CV *cvp,int fi,vecobj *eep) noex {
	confparser	po(cvp,fi,eep) ;
	return po ;
    }
}

confparser::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	void		*vp{} ;
	if ((rs = vecobj_get(cvp->fesp,fi,&vp)) >= 0) {
	    CV_FILE	*fep = filep(vp) ;
	    if (vp) {
	        bfile	cfile, *fp = &cfile ;
	        cchar	*fn = fep->filename ;
	        if ((rs = bopen(fp,fn,"r",0664)) >= 0) {
		    USTAT	sb ;
		    if ((rs = bcontrol(fp,BC_STAT,&sb)) >= 0) {
			if (sb.st_mtime >= fep->mtime) {
			    fep->mtime = sb.st_mtime ;
			    rs = parsing(fep,fp) ;
			    rv = rs ;
			}
		    } /* end if (bcontrol) */
		    rs1 = bclose(fp) ;
		    if (rs >= 0) rs = rs1 ;
	 	} /* end if (bfile) */
	    }
	} /* end if (vecobj_get) */
	return (rs >= 0) ? rv : rs ;
} /* end method (confparser::operator) */

int confparser::parsing(filep fep,bfile *fp) noex {
	bfliner		bl ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = bl.start(fp)) >= 0) {
	    cchar	*lp ;
	    while ((rs = bl.getlns(&lp)) > 0) {
	        cchar	*cp ;
	        if (int cl ; (cl = sfcontent(lp,rs,&cp)) > 0) {
	            rs = parseln(fep,cp,cl) ;
	            rv += rs ;
	        }
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = bl.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfliner) */
	return (rs >= 0) ? rv : rs ;
} /* end mthod (confparser::parsing) */

int confparser::parseln(filep fep,cchar *lp,int ll) noex {
	field		fsb ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = fsb.start(lp,ll)) >= 0) {
	    if (cchar *fp ; ((rs = fsb.get(fterms,&fp)) > 0)) {
		char	keybuf[keylen + 1] ;
	        cint	kl = min(rs,keylen) ;
	        strwcpylc(keybuf,fp,kl) ;
	        if (int idx ; (idx = matostr(key.type,2,keybuf,kl)) > 0) {
		    cchar	*kp = keybuf ;
		    rv = 1 ;
		    switch (idx) {
	            case keytype_define:
			rs = parse_define(fep,&fsb,kp,kl) ;
			break ;
	            case keytype_export:
			rs = parse_export(fep,&fsb,kp,kl) ;
			break ;
	            case keytype_set:
			rs = parse_set(fep,&fsb,kp,kl) ;
			break ;
	            case keytype_unset:
			rs = parse_unset(fep,&fsb,kp,kl) ;
			break ;
		    } /* end switch */
		} else {
		    auto	av = configvars_addvar ;
		    cint	w = configvarsw_vars ;
	            rs = av(cvp,fi,w,keybuf,kl,fsb.lp,fsb.ll) ;
		    rv = 1 ;
		} /* end if (matocasestr) */
	    } /* end if (field_get) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? rv : rs ;
} /* end mthod (confparser::parseln) */

int confparser::parse_define(filep fep,field *,cc *,int) noex {
	int		rs = SR_OK ;
	(void) fep ;
	return rs ;
}

int confparser::parse_export(filep fep,field *,cc *,int) noex {
	int		rs = SR_OK ;
	(void) fep ;
	return rs ;
}

int confparser::parse_set(filep fep,field *,cc *,int) noex {
	(void) fep ;
	int		rs = SR_OK ;
	return rs ;
}

int confparser::parse_unset(filep fep,field *fsbp,cc *,int) noex {
	int		rs ;
	cchar		*cp{} ;
	if ((rs = fsbp->get(fterms,&cp)) > 0) {
	    cint	type = keytype_unset ;
	    rs = file_addvar(fep,type,fi,cp,rs,nullptr,0) ;
	}
	return rs ;
} /* end method (confparser::parse_unset) */



#ifdef	COMMENT
    int configvars_parse(CV *cvp,int fi,vecobj *eep) noex {
	CV_FILE	ve ;
	FIELD		fsb ;
	bfile		file, *fp = &file ;
	cint		llen = LINEBUFLEN ;
	int	rs = SR_OK ;
	int	i ;
	int	c, type ;
	int	sl, cl, len, line = 0 ;
	int	klen, vlen ;

	char	lbuf[LINEBUFLEN + 1] ;
	char	keybuf[KEYBUFLEN + 1], *bp ;
	char	*key, *value ;
	char	*cp ;

/* get the pointer to our own file structure */

	if ((rs = vecobj_get(cvp->fesp,fi,&fep)) < 0)
	    return rs ;

	if (fep == nullptr)
	    return SR_NOTFOUND ;

	if ((rs = bopen(fp,fep->filename,"r",0664)) < 0)
	    return rs ;

	if ((rs = bcontrol(fp,BC_STAT,&sb)) < 0)
	    goto done ;

/* have we already parsed this one? */

	rs = SR_OK ;
	if (fep->mtime >= sb.st_mtime)
	    goto done ;

	fep->mtime = sb.st_mtime ;

/* start processing the configuration file */

	while ((rs = breadlns(fp,lbuf,llen,-1,&line)) > 0) {
	    len = rs ;

	    if (len == 1) continue ;	/* blank line */

	    if (lbuf[len - 1] != '\n') {
	        while ((c = bgetc(fp)) >= 0) {
	            if (c == '\n') break ;
		}
	        continue ;
	    } else {
		len -= 1 ;
	    }

/* pre-process this a little bit */

	    cp = lbuf ;
	    cl = len ;
	    while ((cl > 0) && CHAR_ISWHITE(*cp)) {
		cp += 1 ;
		cl -= 1 ;
	    }

	    if ((cl == 0) || (cp[0] == '#'))
		continue ;

/* do this line */

	    field_get(&fsb,fterms) ;

/* empty or comment only line */

	    if (fsb.flen <= 0) continue ;

/* convert key to lower case */

	    cl = MIN(fsb.flen,KEYBUFLEN) ;
	    strwcpylc(keybuf,fsb.fp,cl) ;

/* check if key is a built-in one */

	    i = matostr(keytypes,2,keybuf,cl) ;

	    if (i >= 0) {
	        type = -1 ;
	        switch (i) {

/* unsets */
	        case keytype_unset:
		    type = keytype_unset ;
	            field_get(&fsb,fterms) ;
	            if (fsb.flen > 0) {
	                type = 3 ;
	                rs = file_addvar(fep,type,fi,fsb.fp,fsb.flen,nullptr,0) ;
	            }

	            break ;

/* export environment */
	        case keytype_set:
			if (type < 0)
				type = keytype_set ;

/* FALLTHROUGH */

	        case keytype_define:
			if (type < 0)
				type = keytype_define ;

/* FALLTHROUGH */

	        case keytype_export:
			if (type < 0)
				type = keytype_export ;

/* get first part */

	            field_get(&fsb,fterms) ;
	            if (fsb.flen <= 0) {
	                rs = SR_INVALID ;
	                if (eep != nullptr) {
	                    badline(eep,fep->filename,line) ;
			}
	                break ;
	            }

	            key = fsb.fp ;
	            klen = fsb.flen ;

/* get second part */

	            field_get(&fsb,fterms) ;

	            value = (fsb.flen >= 0) ? fsb.fp : nullptr ;
	            vlen = fsb.flen ;

	            if (i == keytype_set) {
	                rs = configvars_addvar(cvp,fi,CONFIGVARS_WSETS,
	                    key,klen,value,vlen) ;

	            } else {
	                rs = file_addvar(fep,type,fi,key,klen,value,vlen) ;
		    }

	            if (rs < 0)
	                goto badalloc ;

	            break ;

	        default:
	            rs = SR_NOTSUP ;
	            goto badprogram ;

	        } /* end switch */

	    } else {
		int	alen ;
		char	abuf[LINEBUFLEN + 1] ;

/* unknown keyword, it is just another variable ! */

	        key = fsb.fp ;
	        klen = fsb.flen ;

/* store the key along with the remainder of this "line" */

	        rs = configvars_addvar(cvp,fi,CONFIGVARS_WVARS,
	            key,klen,fsb.lp,fsb.rlen) ;

	        if (rs < 0)
	            goto badalloc ;

	    } /* end if */

	} /* end while (reading lines) */

/* we're out of here one way or another! */
badprogram:
badalloc:
badconfig:

/* done with configuration file processing */
done:

	bclose(fp) ;

	return rs ;
}
/* end subroutine (configvars_parse) */

#endif /* COMMENT */


