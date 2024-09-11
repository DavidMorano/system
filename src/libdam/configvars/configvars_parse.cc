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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
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


/* forward references */


/* local variables */

enum configkeys {
	configkey_define,
	configkey_export,
	configkey_set,
	configkey_unset,
	configkey_overlast
} ;

constexpr cpcchar	configkeys[] = {
	"define",
	"export",
	"set",
	"unset",
	nullptr
} ;

enum vartypes {
	vartype_set,
	vartype_var,
	vartype_export,
	vartype_define,
	vartype_unset,
	vartype_overlast
} ; /* end enum (vartypes) */

constexpr cpcchar	fterms[] = {
	0x00, 0x1B, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

namespace configvars_obj {
    int configvars_parse(CV *cvp,int fi,vecobj *eep) noex {
	int		rs ;
	int		rv = 0 ;
	CV_FILE		*fep ;
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
			    rs = configvars_parser(cvp,fi,eep,fp) ;
			    rv = rs ;
			}
		    } /* end if (bcontrol) */
		    rs1 = bclose(fp) ;
		    if (rs >= 0) rs = rs1 ;
	 	} /* end if (bfile) */
	    }
	} /* end if (vecobj_get) */
	return (rs >= 0) ? rv : rs ;
    }
    int configvars_parser(CV *cvp,int fi,vecobj *eep,bfile *fp) noex {
	bfliner		bl ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = bl.start(fp,0z,-1)) >= 0) {
	    {
		rs = configvars_parsing(cvp,fi,eep,&bl) ;
		rv = rs ;
	    }
	    rs1 = bl.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rv : rs ;
    }
    int configvars_parsing(CV *cvp,int fi,vecobj *eep,bfliner *blp) noex {
	int		rs ;
	int		rv = 0 ;
	int		*lp ;
	while ((rs = blp->getlns(&lp)) > 0) {
	    cchar	*cp ;
	    if (int cl ; (cl = sfcontent(lp,rs,&cp)) > 0) {
	        rs = configvars_parseln(cvp,fi,eep,cp,cl) ;
	        rv += rs ;
	    }
	    if (rs < 0) break ;
	} /* end while */
	return (rs >= 0) ? rv : rs ;
    }
    int configvars_parseln(CV *cvp,int fi,vecobj *eep,cchar *lp,int ll) noex {
	field		fsb ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = fsb.start(lp,ll)) >= 0) {
	    int	idx ;
	    if (cchar *fp ; ((rs = fsb.get(fterms,&fp)) > 0) {
		cint	fl = rs ;
	        if ((idx = matocasestr(configkeys,2,fp,fl)) > 0) {


		} else {
		    auto	av = configvars_addvar ;
		    cint	w = CONFIGVARS_WVARS ;
	            rs = av(cvp,fi,w,fp,fl,fsb.lp,fsb.ll) ;
		    rv = 1 ;
		} /* end if (matocasestr) */
	    } /* end if (field_get) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? rv : rs ;
    }
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

	    i = matostr(configkeys,2,keybuf,cl) ;

	    if (i >= 0) {
	        type = -1 ;
	        switch (i) {

/* unsets */
	        case configkey_unset:
		    type = vartype_unset ;
	            field_get(&fsb,fterms) ;

	            if (fsb.flen > 0) {

	                type = 3 ;
	                rs = file_addvar(fep,type,fi,fsb.fp,fsb.flen,nullptr,0) ;

	                if (rs < 0)
	                    goto badalloc ;

	            }

	            break ;

/* export environment */
	        case configkey_set:
			if (type < 0)
				type = vartype_set ;

/* FALLTHROUGH */

	        case configkey_define:
			if (type < 0)
				type = vartype_define ;

/* FALLTHROUGH */

	        case configkey_export:
			if (type < 0)
				type = vartype_export ;

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

	            if (i == configkey_set) {
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

} /* end namespace (configvars_obj) */


