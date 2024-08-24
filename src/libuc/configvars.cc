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
#include	<cstring>		/* for |strlen(3c)| */
#include	<ctime>
#include	<algorithm>		/* for |min(3c++)| */
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
#define	CV_CUR		configvars_cur
#define	CV_ERR		configvars_err

#undef	BUFLEN
#define	BUFLEN		(LINEBUFLEN * 2)

#define	KEYBUFLEN	20

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


/* local namespaces */

using std::min ;			/* subroutine (template) */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		configvars_addfile(CV *,cchar *,vecitem *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int configvars_ctor(configvars *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    nullptr_t	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->checktime = time(nullptr) ;
	    if ((op->fesp = new(nothrow) vecitem) != np) {
	        if ((op->defp = new(nothrow) vecitem) != np) {
	            if ((op->unvp = new(nothrow) vecitem) != np) {
	                if ((op->expp = new(nothrow) vecitem) != np) {
	                    if ((op->setp = new(nothrow) vecitem) != np) {
	                        if ((op->varp = new(nothrow) vecitem) != np) {
				    rs = SR_OK ;
	                        } /* end if (new-var) */
				if (rs < 0) {
				    delete op->setp ;
				    op->setp = np ;
				}
	                    } /* end if (new-set) */
			    if (rs < 0) {
				delete op->expp ;
				op->expp = np ;
			    }
	                } /* end if (new-exp) */
			if (rs < 0) {
			    delete op->unvp ;
			    op->unvp = np ;
			}
	            } /* end if (new-unv) */
		    if (rs < 0) {
			delete op->defp ;
			op->defp = np ;
		    }
	        } /* end if (new-def) */
		if (rs < 0) {
		    delete op->fesp ;
		    op->fesp = np ;
		}
	    } /* end if (new-fes) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (configvars_ctor) */

static int configvars_dtor(configvars *op) noex {
	nullptr_t	np{} ;
	int		rs = SR_OK ;
	if (op->varp) {
	    delete op->varp ;
	    op->varp = np ;
	}
	if (op->setp) {
	    delete op->setp ;
	    op->setp = np ;
	}
	if (op->expp) {
	    delete op->expp ;
	    op->expp = np ;
	}
	if (op->unvp) {
	    delete op->unvp ;
	    op->unvp = np ;
	}
	if (op->defp) {
	    delete op->defp ;
	    op->defp = np ;
	}
	if (op->fesp) {
	    delete op->fesp ;
	    op->fesp = np ;
	}
	return rs ;
}
/* end subroutine (configvars_dtor) */

template<typename ... Args>
static inline int configvars_magic(configvars *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CONFIGVARS_MAGIC) ? SR_OK : SR_NOTOPEN ;
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
static int file_finone(CV_FILE *,int) noex ;

static int var_start(CV_VAR *,int,char *,int,char *,int) noex ;
static int var_finish(CV_VAR *) noex ;

static void	badline(vecitem *,char *,int) noex ;
static void	freeit(char **) noex ;


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


/* exported variables */


/* exported subroutines */

int configvars_open(configvars *cvp,cchar *cfn,vecitem *eep) noex {
	int		rs ;
	if ((rs = configvars_ctor(cvp,cfn)) >= 0) {
	    vecitem	*vip ;
	    cint	ne = 10 ;
	    vip = cvp->fesp ;
	    if ((rs = vecitem_start(vip,ne,0)) >= 0) {
	        vip = cvp->defp ;
	        if ((rs = vecitem_start(vip,ne,0)) >= 0) {
	            vip = cvp->expp ;
	            if ((rs = vecitem_start(vip,ne,0)) >= 0) {
	                vip = cvp->unvp ;
		        if ((rs = vecitem_start(vip,ne,0)) >= 0) {
			    cint	vo = VECITEM_OSORTED ;
	                    vip = cvp->varp ;
			    if ((rs = vecitem_start(vip,ne,vo)) >= 0) {
	                        vip = cvp->setp ;
				if ((rs = vecitem_start(vip,ne,vo)) >= 0) {
				    cvp->magic = CONFIGVARS_MAGIC ;
				    if (cfn && cfn[0]) {
					auto ca = configvars_addfile ;
	    				if ((rs = ca(cvp,cfn,eep)) >= 0) {
					    rs = SR_OK ;
					}
				    }
				    if (rs < 0) {
					vecitem_finish(cvp->setp) ;
				    }
				} /* end if (sets) */
				if (rs < 0) {
				    vecitem_finish(cvp->varp) ;
				}
			    } /* end if (vars) */
			    if (rs < 0) {
				vecitem_finish(cvp->unvp) ;
			    }
			} /* end if (unsets) */
			if (rs < 0) {
			    vecitem_finish(cvp->expp) ;
			}
		    } /* end if (exports) */
		    if (rs < 0) {
			vecitem_finish(cvp->defp) ;
		    }
		} /* end if (defines) */
		if (rs < 0) {
		    vecitem_finish(cvp->fesp) ;
		}
	    } /* end if (fes) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (configvars_open) */

int configvars_close(CV *cvp) noex {
	int		rs  ;
	int		rs1 ;
	if ((rs = configvars_magic(cvp)) >= 0) {
	    {
	        rs1 = configvars_finishallvars(cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = configvars_finishfiles(cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = configvars_dtor(cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    cvp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (configvars_close) */

int configvars_addfile(CV *cvp,cchar *cfname,vecitem *eep) noex {
	int		rs ;
	if ((rs = configvars_magic(cvp,cfname)) >= 0) {
		rs = SR_INVALID ;
		if (cfname[0]) {
	            if ((rs = vecitem_count(cvp->fesp)) >= 0) {
			cint	isz = sizeof(int) ;
			cint	vr = rs ;
	    	        rs = SR_TOOBIG ;
			if (vr < isz) {
			    CV_FILE	fe ;
			    if ((rs = file_start(&fe,cfname)) >= 0) {
				cint	fsz = sizeof(CV_FILE) ;
				vecitem	*vip = cvp->fesp ;
				if ((rs = vecitem_add(vip,&fe,fsz)) >= 0) {
				    cint	fi = rs ;
				    rs = configvars_parsefile(cvp,fi,eep) ;
				    if (rs < 0) {
					vecitem_del(cvp->fesp,fi) ;
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
	return rs ;
}
/* end subroutine (configvars_addfile) */


/* private subroutines */

static int configvars_parsefile(CV *cvp,int fi,vecitem *eep) noex {
	int		rs = SR_FAULT ;
	if (cvp && fi && eep) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (configvars_parsefile) */

static int configvars_addvar(CV *cvp,int fi,int w,
		char *kp,int kl,char *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		idx = 0 ;
	if (cvp && kp) {
	    CV_VAR	v{} ;
	    cint	vsz = sizeof(CV_VAR) ;
	    if ((rs = var_start(&v,fi,kp,kl,vp,vl)) >= 0) {
	        vecitem	*slp = (w) ? cvp->setp : cvp->varp ;
	        if ((rs = vecitem_add(slp,&v,vsz)) >= 0) {
		    idx = rs ;
		}
	        if (rs < 0) {
	            var_finish(&v) ;
	        }
	    } /* end if (var-) */
	} /* end if (non-null) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (configvars_addvar) */

static int configvars_finishallvars(CV *cvp) noex {
	int	rs = SR_OK ;
	int	rs1 ;
	for (int j = 0 ; j < 5 ; j += 1) {
	    vecitem	*slp{} ;
	    CV_VAR	*cep{} ;
	    switch (j) {
	    case vartype_set: slp = cvp->setp ; break ;
	    case vartype_var: slp = cvp->varp ; break ;
	    case vartype_export: slp = cvp->expp ; break ;
	    case vartype_define: slp = cvp->defp ; break ;
	    case vartype_unset: slp = cvp->unvp ; break ;
	    } /* end switch */
	    if (slp) {
	        for (int i = 0 ; vecitem_get(slp,i,&cep) >= 0 ; i += 1) {
	            if (cep) {
	                rs1 = var_finish(cep) ;
		        if (rs >= 0) rs = rs1 ;
	 	    }
	        } /* end for */
	        {
	            rs1 = vecitem_finish(slp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end if (slp) */
	} /* end for */
	return rs ;
}
/* end subroutine (configvars_finishallvars) */

static int configvars_finishfiles(CV *cvp) noex {
	vecitem		*slp = cvp->fesp ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    CV_FILE		*cfp{} ;
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
	int		rs = SR_FAULT ;
	if (cfp && filename) {
	    memclear(cfp) ;
	    rs = SR_NOMEM ;
	    if ((cfp->filename = mallocstr(filename)) != nullptr) {
	        vecitem		*vip{} ;
	        vip = &cfp->defines ;
	        if ((rs = vecitem_start(vip,0,0)) >= 0) {
	            vip = &cfp->exports ;
	            if ((rs = vecitem_start(vip,0,0)) >= 0) {
	                vip = &cfp->unsets ;
		        if ((rs = vecitem_start(vip,0,0)) >= 0) {
			    rs = strlen(filename) ;
		        }
		        if (rs < 0) {
			    vecitem_finish(&cfp->exports) ;
		        }
		    } /* end if (exports) */
		    if (rs < 0) {
		        vecitem_finish(&cfp->defines) ;
		    }
	        } /* end if (defines) */
	        if (rs < 0) {
		    uc_free(cfp->filename) ;
		    cfp->filename = nullptr ;
	        }
	    } /* end if (filename) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(CV_FILE *cfp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < 3 ; i += 1) {
	    rs1 = file_finone(cfp,i) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end for */
	if (cfp->filename) {
	    rs1 = uc_free(cfp->filename) ;
	    if (rs >= 0) rs = rs1 ;
	    cfp->filename = nullptr ;
	}
	return rs ;
}
/* end subroutine (file_finish) */

static int file_finone(CV_FILE *cfp,int type) noex {
	vecitem		*vip{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	switch (type) {
	case 0: vip = &cfp->defines ; break ;
	case 1: vip = &cfp->exports ; break ;
	case 2: vip = &cfp->unsets ; break ;
	default: rs = SR_BUGCHECK ; break ;
	} /* end switch */
	if ((rs >= 0) && vip) {
	    CV_VAR	*vep{} ;
	    for (int i = 0 ; vecitem_get(vip,i,&vep) >= 0 ; i += 1) {
	        if (vep) {
	            rs1 = var_finish(vep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    {
	        rs1 = vecitem_finish(vip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (file_finone) */

static int file_addvar(CV_FILE *cfp,int type,int fi,
		char *key,int klen,char *value,int vlen) noex {
	CV_VAR		ve{} ;
	int		rs ;
	if ((rs = var_start(&ve,fi,key,klen,value,vlen)) >= 0) {
	    cint	vsz = sizeof(CV_FILE) ;
	    vecitem	*vlp{} ;
	    switch (type) {
	    case vartype_define: vlp = &cfp->defines ; break ;
	    case vartype_export: vlp = &cfp->exports ; break ;
	    case vartype_unset: vlp = &cfp->unsets ; break ;
	    default: rs = SR_BUGCHECK ; break ;
	    } /* end switch */
	    if ((rs >= 0) && vlp) {
	        rs = vecitem_add(vlp,&ve,vsz) ;
	    }
	    if (rs < 0) {
	        var_finish(&ve) ;
	    }
	} /* end if (var_start) */
	return rs ;
}
/* end subroutine (file_addvar) */

static int var_start(CV_VAR *cep,int fi,char *kp,int kl,char *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (cep && kp) {
	    int		len{} ;
	    char	*vb{} ;
	    memclear(cep) ;
	    if (kl < 0) kl = strlen(kp) ;
	    if (vp) {
	        if (vl < 0) vl = strlen(vp) ;
	    } else {
		vl = 0 ;
	    }
	    len = (kl + vl + 2) ;
	    if ((rs = uc_malloc(len,&vb)) >= 0) {
	        char	*cp = (strwcpy(vb,kp,kl) + 1) ;
	        cep->fi = fi ;
	        cep->key = vb ;
	        cep->klen = kl ;
	        cep->value = nullptr ;
	        cep->vlen = 0 ;
	        if (vp) {
	            strwcpy(cp,vp,vl) ;
	            cep->value = cp ;
	            cep->vlen = vl ;
	        } else {
	            *cp = '\0' ;
	        }
	        cep->fmask = 0 ;
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (var_start) */

static int var_finish(CV_VAR *cep) noex {
	int		rs = SR_FAULT ;
	if (cep) {
	    rs = SR_OK ;
	    freeit(&cep->key) ;
	}
	return rs ;
}
/* end subroutine (var_finish) */

static void freeit(char **vp) noex {
	if (*vp) {
	    uc_free(*vp) ;
	    *vp = nullptr ;
	}
}
/* end subroutine (freeit) */

static void badline(vecitem *eep,char *fname,int line) noex {
	if (eep && fname) {
	    configvars_err	e ;
	    e.line = line ;
	    strwcpy(e.filename,fname,MAXPATHLEN) ;
	    if (eep) {
		cint	esz = sizeof(struct configvars_errline) ;
	        (void) vecitem_add(eep,&e,esz) ;
	    }
	} /* end if (non-null) */
}
/* end subroutine (badline) */


