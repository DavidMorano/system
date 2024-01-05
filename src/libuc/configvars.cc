/* configvars SUPPORT */
/* lang=C++20 */

/* Configuration-Variables */
/* version %I% last-modified %G% */

#define	CF_ALLOCFILENAME	1

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This subroutine was originally written.

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
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<usupport.h>
#include	<mallocstuff.h>
#include	<bfile.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<field.h>
#include	<matostr.h>
#include	<localmisc.h>

#include	"configvars.h"


/* local defines */

#define	CONFxIGVARS_WSETS	0
#define	CONFxIGVARS_WVARS	1

#define	CV		configvars
#define	CV_VAR		configvars_var
#define	CV_FILE		configvars_file

#undef	BUFLEN
#define	BUFLEN		(LINEBUFLEN * 2)

#define	KEYBUFLEN	20

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		configvars_addfile(CV *,cchar *,vecitem *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int configvars_magic(configvars *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CONFIGUVARS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (configvars_magic) */

static int configvars_parsefile(CV *,int,vecitem *) noex ;
static int configvars_finishallvars(CV *) noex ;
static int configvars_finishfiles(CV *) noex ;
static int configvars_addvar(CV *,int,int,char *,int,char *,int) noex ;

static int file_start(CV_FILE *,cchar *) noex ;
static int file_addvar(CV_FILE *,int,int,char *,int,char *,int) noex ;
static int file_finish(CV_FILE *) noex ;

static int var_start(CV_FILE *,int,char *,int,char *,int) noex ;
static int var_finish(CV_FILE *) noex ;

static void	badline(vecitem *,char *,int) noex ;
static void	freeit() noex ;


/* local variables */

static constexpr cchar 	fterms[32] = {
	0x00, 0x1B, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

enum configkeys {
	configkey_define,
	configkey_export,
	configkey_set,
	configkey_unset,
	configkey_overlast
} ;

static constexpr cchar	*configkeys[] = {
	"define",
	"export",
	"set",
	"unset",
	NULL
} ;

enum vartypes {
	vartype_set,
	vartype_var,
	vartype_export,
	vartype_define,
	vartype_unset,
	vartype_overlast
} ;


/* exported variables */


/* exported subroutines */

int configvars_open(configvars *cvp,cchar *cfn,vecitem *eep) noex {
	int		rs = SR_FAULT ;
	if (cvp && cfn && eep) {
	    vecitem	*vip ;
	    cint	ne = 10 ;
	    memclear(cvp) ;
	    cvp->magic = 0 ;
	    cvp->checktime = time(NULL) ;
	    vip = &cvp->fes ;
	    if ((rs = vecitem_start(vip,ne,0)) >= 0) {
	        vip = &cvp->defines ;
	        if ((rs = vecitem_start(vip,ne,0)) >= 0) {
	            vip = &cvp->exports ;
	            if ((rs = vecitem_start(vip,ne,0)) >= 0) {
	                vip = &cvp->unsets ;
		        if ((rs = vecitem_start(vip,ne,0)) >= 0) {
			    cint	vo = VECITEM_OSORTED ;
	                    vip = &cvp->vars ;
			    if ((rs = vecitem_start(vip,ne,vo)) >= 0) {
	                        vip = &cvp->sets ;
				if ((rs = vecitem_start(vip,ne,vo)) >= 0) {
				    cvp->magic = CONFIGVARS_MAGIC ;
				    if (cfn && cfn[0]) {
					auto ca = configvars_addfile ;
	    				if ((rs = ca(cvp,cfn,eep)) >= 0) {
					    op->magic = CONFIGVARS_MAGIC ;
					}
				    }
				    if (rs < 0) {
					vecitem_finish(&cvp->sets) ;
				    }
				} /* end if (sets) */
				if (rs < 0) {
				    vecitem_finish(&cvp->vars) ;
				}
			    } /* end if (vars) */
			    if (rs < 0) {
				vecitem_finish(&cvp->unsets) ;
			    }
			} /* end if (unsets) */
			if (rs < 0) {
			    vecitem_finish(&cvp->exports) ;
			}
		    } /* end if (exports) */
		    if (rs < 0) {
			vecitem_finish(&cvp->defines) ;
		    }
		} /* end if (defines) */
		if (rs < 0) {
		    vecitem_finish(&cvp->fes) ;
		}
	    } /* end if (fes) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (configvars_open) */

int configvars_addfile(CV *cvp,cchar *cfname,vecitem *eep) noex {
	int	rs = SR_FAULT ;
	if (cvp && cfname) {
	    rs = SR_NOTOPEN ;
	    if (cvp->magic == CONFIGVARS_MAGIC) {
		rs = SR_INVALID ;
		if (cfname[0]) {
	            if ((rs = vecitem_count(&cvp->fes)) >= 0) {
			cint	isz = sizeof(int) ;
			cint	vr = rs ;
	    	        rs = SR_TOOBIG ;
			if (vr < isz) {
			    CV_FILE	fe ;
			    if ((rs = file_start(&fe,cfname)) >= 0) {
				cint	fsz = sizeof(CV_FILE) ;
				vecitem	*vip = &cvp->fes ;
				if ((rs = vecitem_add(vip,&fe,fsz)) >= 0) {
				    cint	fi = rs ;
				    rs = configvars_parsefile(cvp,fi,eep) ;
				    if (rs < 0) {
					vecitem_del(&cvp->fes,fi) ;
				    }
				} /* end if (fe) */
				if (rs < 0) {
				    file_finish(&fe) ;
				}
			    } /* end if (file-) */
			} /* end if (size ok) */
		    } /* end if (count) */
	        } /* end if (valid) */
	    } /* end if (magic) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (configvars_addfile) */

int configvars_close(CV *cvp) noex {
	int	rs 
	int	rs1 ;
	if ((rs = configuvars_magic(cvp)) >= 0) {
	    {
	        rs1 = configvars_finishallvars(cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = configvars_finishfiles(cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    cvp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (configvars_close) */


/* private subroutines */

static int configvars_parsefile(cvp,fi,eep)
CONFIGVARS	*cvp ;
int		fi ;
vecitem		*eep ;
{
	struct ustat	sb ;

	CV_FILE	*fep ;

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

	if ((rs = vecitem_get(&cvp->fes,fi,&fep)) < 0)
	    return rs ;

	if (fep == NULL)
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

#ifdef	COMMENT
	        f_trunc = TRUE ;
#endif
	        while ((c = bgetc(fp)) >= 0) {

	            if (c == '\n') 
			break ;

		}

	        continue ;

	    } else
		len -= 1 ;

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

	    fsb.lp = cp ;
	    fsb.rlen = cl ;
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
	                rs = file_addvar(fep,type,fi,fsb.fp,fsb.flen,NULL,0) ;

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
	                if (eep != NULL)
	                    badline(eep,fep->filename,line) ;

	                break ;
	            }

	            key = fsb.fp ;
	            klen = fsb.flen ;

/* get second part */

	            field_get(&fsb,fterms) ;

	            value = (fsb.flen >= 0) ? fsb.fp : NULL ;
	            vlen = fsb.flen ;

	            if (i == configkey_set) {
	                rs = configvars_addvar(cvp,fi,CONFIGVARS_WSETS,
	                    key,klen,value,vlen) ;

	            } else
	                rs = file_addvar(fep,type,fi,key,klen,value,vlen) ;

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
/* end subroutine (configvars_parsefile) */


static int configvars_addvar(cvp,fi,w,key,klen,value,vlen)
CONFIGVARS	*cvp ;
int		fi, w ;
char		key[], value[] ;
int		klen, vlen ;
{
	CV_FILE	v ;

	vecitem		*slp ;

	int	rs ;


	if ((rs = var_start(&v,fi,key,klen,value,vlen)) < 0)
	    return rs ;

	slp = (w) ? &cvp->sets : &cvp->vars ;
	if ((rs = vecitem_add(slp,&v,sizeof(CV_FILE))) < 0)
	    var_finish(&v) ;

	return rs ;
}
/* end subroutine (configvars_addvar) */


/* free up all of the varaiable entries in this CV object */
static int configvars_finishallvars(cvp)
CONFIGVARS	*cvp ;
{
	CV_FILE	*cep ;

	vecitem		*slp ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	i, j ;


	for (j = 0 ; j < 5 ; j += 1) {

	    switch (j) {

	    case vartype_set:
	        slp = &cvp->sets ;
	        break ;

	    case vartype_var:
	        slp = &cvp->vars ;
	        break ;

	    case vartype_export:
	        slp = &cvp->exports ;
	        break ;

	    case vartype_define:
	        slp = &cvp->defines ;
	        break ;

	    case vartype_unset:
	        slp = &cvp->unsets ;
	        break ;

	    } /* end switch */

	    for (i = 0 ; vecitem_get(slp,i,&cep) >= 0 ; i += 1) {
	        if (cep == NULL) continue ;

	        rs1 = var_finish(cep) ;
		if (rs >= 0) rs = rs1 ;

	    } /* end for */

	    rs1 = vecitem_finish(slp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end for */

	return rs ;
}
/* end subroutine (configvars_finishallvars) */

static int configvars_finishfes(CV *cvp,int fi) noex {
	CV_FILE		*cep ;
	vecitem		*slp ;
	int	rs = SR_OK ;
	for (int j = 0 ; j < 2 ; j += 1) {
	    int		i = 0 ;
	    slp = (j == 0) ? &cvp->sets : &cvp->vars ;
	    while (vecitem_get(slp,i,&cep) >= 0) {
	        if (cep && ((cep->fi == fi) || (fi < 0))) {
	            var_finish(cep) ;
	            vecitem_del(slp,i) ;
	        } else {
	            i += 1 ;
		}
	    } /* end while */
	} /* end for */
	return rs ;
}
/* end subroutine (configvars_finishfes) */

static int configvars_finishfiles(CV *cvp) noex {
	vecitem		*slp = &cvp->fes ;
	CV_FILE		*cfp{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    for (int i = 0 ; vecitem_get(slp,i,&cfp) >= 0 ; i += 1) {
	        if (cfp) {
	            rs1 = file_finish(cfp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	}
	{
	    rs1 = vecitem_finish(slp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (configvars_finishfiles) */

static int file_start(CV_FILE *cfp,cchar *filename) noex {
	int		rs ;
	memclear(cfp) ;
	cfp->mtime = 0 ;
	if ((cfp->filename = mallocstr(filename)) == NULL)
	    return SR_NOMEM ;

	if ((rs = vecitem_start(&cfp->defines,0,0)) < 0)
	    goto bad2 ;

	if ((rs = vecitem_start(&cfp->exports,0,0)) < 0)
	    goto bad3 ;

	if ((rs = vecitem_start(&cfp->unsets,0,0)) < 0)
	    goto bad4 ;

ret0:
	return rs ;

/* bad stuff */
bad4:
	vecitem_finish(&cfp->exports) ;

bad3:
	vecitem_finish(&cfp->defines) ;

bad2:
	uc_free(cfp->filename) ;

bad1:
	goto ret0 ;
}
/* end subroutine (file_start) */


/* free up all of the stuff within a file entry */
static int file_finish(cfp)
CV_FILE	*cfp ;
{
	vecitem		*slp ;

	CV_FILE	*vep ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	i ;


	slp = &cfp->defines ;
	for (i = 0 ; vecitem_get(slp,i,&vep) >= 0 ; i += 1) {
	    if (vep == NULL) continue ;

	    rs1 = var_finish(vep) ;
	    if (rs >= 0) rs = rs1 ;

	} /* end for */

	rs1 = vecitem_finish(slp) ;
	if (rs >= 0) rs = rs1 ;

	slp = &cfp->exports ;
	for (i = 0 ; vecitem_get(slp,i,&vep) >= 0 ; i += 1) {
	    if (vep == NULL) continue ;

	    rs1 = var_finish(vep) ;
	    if (rs >= 0) rs1 ;

	} /* end for */

	rs1 = vecitem_finish(slp) ;
	if (rs >= 0) rs = rs1 ;

	slp = &cfp->unsets ;
	for (i = 0 ; vecitem_get(slp,i,&vep) >= 0 ; i += 1) {
	    if (vep == NULL) continue ;

	    rs1 = var_finish(vep) ;
	    if (rs >= 0) rs = rs1 ;

	} /* end for */

	rs1 = vecitem_finish(slp) ;
	if (rs >= 0) rs = rs1 ;

	if (cfp->filename != NULL) {
	    uc_free(cfp->filename) ;
	    cfp->filename = NULL ;
	}

	return rs ;
}
/* end subroutine (file_finish) */


/* add a variable to this file */
static int file_addvar(cfp,type,fi,key,klen,value,vlen)
CV_FILE	*cfp ;
int		type ;
int		fi ;
char		key[], value[] ;
int		klen, vlen ;
{
	CV_FILE	ve ;

	vecitem		*vlp ;

	int	rs = SR_NOMEM ;


	rs = var_start(&ve,0,key,klen,value,vlen) ;

	if (rs < 0)
	    goto bad1 ;

	switch (type) {

	case vartype_define:
	    vlp = &cfp->defines ;
	    break ;

	case vartype_export:
	    vlp = &cfp->exports ;
	    break ;

	case vartype_unset:
	    vlp = &cfp->unsets ;
	    break ;

	} /* end switch */

	if ((rs = vecitem_add(vlp,&ve,sizeof(CV_FILE))) < 0)
	    goto bad2 ;

ret0:
	return rs ;

bad2:
	var_finish(&ve) ;

bad1:
	goto ret0 ;
}
/* end subroutine (file_addvar) */


static int var_start(cep,fi,k,klen,v,vlen)
CV_FILE	*cep ;
int		fi ;
char		*k, *v ;
int		klen, vlen ;
{
	int	rs = SR_OK ;
	int	len ;

	char	*vb, *cp ;


	memset(cep,0,sizeof(CV_FILE)) ;

	if (k == NULL)
	    return SR_FAULT ;

	memset(&cep->f,0,sizeof(struct configvars_vflags)) ;

	if (klen < 0)
	    klen = strlen(k) ;

	if (v != NULL) {

	    if (vlen < 0)
	        vlen = strlen(v) ;

	} else
	    vlen = 0 ;

	len = klen + vlen + 2 ;
	if ((rs = uc_malloc(len,&vb)) < 0)
	    return rs ;

	cp = strwcpy(vb,k,klen) + 1 ;

	cep->fi = fi ;
	cep->key = vb ;
	cep->klen = klen ;
	cep->value = NULL ;
	cep->vlen = 0 ;
	if (v != NULL) {

	    strwcpy(cp,v,vlen) ;

	    cep->value = cp ;
	    cep->vlen = vlen ;

	} else
	    *cp = '\0' ;

	cep->fmask = 0 ;
	return SR_OK ;
}
/* end subroutine (var_start) */


/* free up an entry */
static int var_finish(cep)
CV_FILE	*cep ;
{


	freeit(&cep->key) ;

/* there is no explicit "value" to free since it is part of the key string */

	return 0 ;
}
/* end subroutine (var_finish) */


/* free up the resources occupied by a CONFIG_STRUCTURE */
static void freeit(vp)
char		**vp ;
{

	if (*vp != NULL) {
	    uc_free(*vp) ;
	    *vp = NULL ;
	}

}
/* end subroutine (freeit) */


static void badline(vecitem eep,char *fname,int line) noex {
	struct configvars_errline	e ;

	if (eep == NULL)
	    return ;

	e.line = line ;
	strwcpy(e.filename,fname,MAXPATHLEN) ;

	if (eep != NULL)
	(void) vecitem_add(eep,&e,sizeof(struct configvars_errline)) ;

}
/* end subroutine (badline) */


