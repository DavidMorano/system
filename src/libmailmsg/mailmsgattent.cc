/* mailmsgattent SUPPORT */
/* lang=C++20 */

/* extra subroutines for the 'mailmsgattent' object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch to do what the
	previous program by the same name did.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines are extra methods to the MAILMSGATTENT
	object. See the code for details!

	Grammar:
		content_spec := type | { type "/" subtype } | file_extension
		attspec := filename | { content_spec "=" filename }

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<contypevals.h>
#include	<contentencodings.h>
#include	<ismmclass.h>
#include	<stdfnames.h>
#include	<xperm.h>
#include	<mktmp.h>
#include	<sfx.h>
#include	<six.h>
#include	<mkpathx.h>
#include	<matxstr.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"mailmsgattent.h"


/* local defines */

#define	MAILMSGATTENT_TMPCNAME	"mkmsgXXXXXXXXX"

#define	MME		MAILMSGATTENT


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

extern "C" {
    int		mailmsgattent_analyze(MME *,cchar *) noex ;
    int		mailmsgattent_finish(MME *) noex ;
    int		mailmsgattent_isplaintext(MME *) noex ;
}

template<typename ... Args>
static inline int mailmsgattent_magic(mailmsgattent *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MAILMSGATTENT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailmsgattent_magic) */

static int	mailmsgattent_startfn(MME *,cchar *,int) noex ;
static int	mailmsgattent_startct(MME *,cchar *,int) noex ;
static int	mailmsgattent_startctpri(MME *,cchar *,int) noex ;
static int	mailmsgattent_startctsub(MME *,cchar *,int) noex ;
static int	mailmsgattent_checkatt(MME *) noex ;
static int	mailmsgattent_analyzer(MME *,bfile *,bfile *) noex ;

static int	freeit(void *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsgattent_start(MME *op,cc *ct,cc *ce,cc *nbuf,int nlen) noex {
	int		rs = SR_FAULT ;
	if (op && ct && nbuf) {
	    if (nlen < 0) nlen = strlen(nbuf) ;
	    rs = memclear(op) ; /* dangerous */
	    op->cte = -1 ;
	    op->clen = -1 ;
	    op->clines = -1 ;
	    if (int si ; (si = sisub(nbuf,nlen,"=")) >= 0) {
	        cint	fl = (nlen-si-1) ;
	        cchar	*fp = (nbuf+si+1) ;
	        if ((rs = mailmsgattent_startfn(op,fp,fl)) >= 0) {
	    	    rs = mailmsgattent_startct(op,nbuf,si) ;
	        }
	    } else {
	        rs = mailmsgattent_startfn(op,nbuf,nlen) ;
	    }
	    if (rs >= 0) {
	       if (ct && (op->ext == nullptr) && (op->type == nullptr)) {
	            cint	cl = strlen(ct) ;
		    rs = mailmsgattent_startct(op,ct,cl) ;
	        } /* end if */
	    } /* end if (ok) */
/* content_encoding */
	    if ((rs >= 0) && (ce != nullptr)) {
	        cchar	*enc ;
	        if ((rs = uc_mallocstrw(ce,-1,&enc)) >= 0) {
		    op->encoding = enc ;
	        }
	    }
	    if (rs >= 0) {
	        op->magic = MAILMSGATTENT_MAGIC ;
	    }
	    if (rs < 0) {
	        mailmsgattent_finish(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgattent_start) */

int mailmsgattent_finish(MME *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailmsgattent_magic(op)) >= 0) {
	    op->clen = -1 ;
	    op->clines = -1 ;
	    op->cte = -1 ;
	    {
	        rs1 = freeit(&op->type) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = freeit(&op->subtype) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = freeit(&op->encoding) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = freeit(&op->attfname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = freeit(&op->ext) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = freeit(&op->description) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->auxfname) {
	        if (op->auxfname[0] != '\0') {
	            u_unlink(op->auxfname) ;
	        }
	        rs1 = freeit(&op->auxfname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgattent_finish) */

/* find the (minimally required) content-type for this particular attachment */
int mailmsgattent_type(MME *op,MIMETYPES *mtp) noex {
	int		rs ;
	int		f = false ;
	if ((rs = mailmsgattent_magic(op,mtp)) >= 0) {
	    if ((op->type != nullptr) && (op->type[0] == '\0')) {
	        uc_free(op->type) ;
	        op->type = nullptr ;
	    }
	    if (op->type == nullptr) {
	        cchar	*tp ;
	        char	typespec[MIMETYPES_TYPELEN + 1] ;
/* we didn't have a type so we look the hard way */
	        if (op->subtype != nullptr) {
	            uc_free(op->subtype) ;
	            op->subtype = nullptr ;
	        }
	        if ((op->ext != nullptr) && (op->ext[0] == '\0')) {
	            uc_free(op->ext) ;
	            op->ext = nullptr ;
	        }
/* if there is no explicit extension, then we get it from the filename */
	        if ((op->ext == nullptr) && (op->attfname != nullptr)) {
	            if ((tp = strrchr(op->attfname,'.')) != nullptr) {
		        cchar	*cp ;
		        if ((rs = uc_mallocstrw((tp+1),-1,&cp)) >= 0) {
	                    op->ext = cp ;
		        }
	            } /* end if */
	        } /* end if (extracting filename extension) */
/* continue */
	        if (rs >= 0) {
		    cchar	*vp = nullptr ;
		    int	vl = -1 ;
	            if (op->ext != nullptr) {
	                if ((rs = mimetypes_find(mtp,typespec,op->ext)) >= 0) {
	                    if ((tp = strchr(typespec,'/')) != nullptr) {
			        cchar	*cp ;
			        vp = typespec ;
			        vl = (tp-typespec) ;
			        if ((rs = uc_mallocstrw((tp+1),-1,&cp)) >= 0) {
	                            op->subtype = cp ;
			        }
	                    } else {
	                        vp = typespec ;
		            }
	                } else if (rs == SR_NOENT) {
			    rs = SR_OK ;
	                    vp = "binary" ;
	                }
	            } else {
		        vp = "binary" ;
	            }
		    if ((rs >= 0) && (vp != nullptr)) {
		        cchar	*cp ;
		        if ((rs = uc_mallocstrw(vp,vl,&cp)) >= 0) {
			    f = true ;
			    op->type = cp ;
		        }
		    }
	        } /* end if (ok) */
	    } /* end if (needed a type) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailmsgattent_type) */

int mailmsgattent_typeset(MME *op,cchar *tstr,cchar *ststr) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailmsgattent_magic(op)) >= 0) {
	    if (op->type != nullptr) {
	        rs1 = uc_free(op->type) ;
	        if (rs >= 0) rs = rs1 ;
	        op->type = nullptr ;
	    }
	    if (op->subtype != nullptr) {
	        rs1 = uc_free(op->subtype) ;
	        if (rs >= 0) rs = rs1 ;
	        op->subtype = nullptr ;
	    }
	    if (rs >= 0) {
	        if ((tstr != nullptr) && (tstr[0] != '\0')) {
	            cchar	*cp ;
	            if ((rs = uc_mallocstrw(tstr,-1,&cp)) >= 0) {
	                op->type = cp ;
	                if ((ststr != nullptr) && (ststr[0] != '\0')) {
	                    if ((rs = uc_mallocstrw(ststr,-1,&cp)) >= 0) {
	                        op->subtype = cp ;
	                    } /* end if (memory-allocation) */
	                }
	                if (rs < 0) {
	                    uc_free(op->type) ;
	                    op->type = nullptr ;
	                }
	            } /* end if (memory-allocation) */
	        } /* end if (tstr) */
	        if (rs >= 0) {
	            rs = mailmsgattent_isplaintext(op) ;
	        }
	    } /* end if (ok) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgattent_typeset) */

int mailmsgattent_isplaintext(MME *op) noex {
	int		rs ;
	int		f = false ;
	if ((rs = mailmsgattent_magic(op)) >= 0) {
	    if ((op->type != nullptr) && (op->subtype != nullptr)) {
	        cchar	*str_text = contypevals[contypeval_text] ;
	        cchar	*str_plain = contypevals[contypeval_plain] ;
	        f = (strcmp(op->type,str_text) == 0) ;
	        f = f && (strcmp(op->subtype,str_plain) == 0) ;
	    }
	    op->f_plaintext = f ;
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailmsgattent_isplaintext) */

int mailmsgattent_code(MME *op,cchar *tmpdname) noex {
	int		rs ;
	int		code = 0 ;
	if ((rs = mailmsgattent_magic(op)) >= 0) {
	    if (op->type != nullptr) {
	        rs = mailmsgattent_isplaintext(op) ;
	    } else {
	        op->f_plaintext = true ;
	    }
	    if (! op->f_plaintext) {
		code = CE_BASE64 ;
	    }
	    if ((rs >= 0) && (op->encoding == nullptr)) {
	        cchar	*enc = contentencodings[contentencoding_base64] ;
	        if (op->f_plaintext) {
	            if ((rs = mailmsgattent_analyze(op,tmpdname)) >= 0) {
	                code = rs ;
	                if ((code >= CE_7BIT) && (code < CE_OVERLAST)) {
			    cchar	*enc = contentencodings[code] ;
			    cchar	*cp ;
			    if ((rs = uc_mallocstrw(enc,-1,&cp)) >= 0) {
	                        op->encoding = cp ;
			    }
		        }
		    }
	        } else {
		    cchar	*cp ;
	            code = CE_BASE64 ;
		    if ((rs = uc_mallocstrw(enc,-1,&cp)) >= 0) {
	                op->encoding = cp ;
		    }
	        } /* end if */
	    } else {
	        int		si ;
	        cchar	*enc = op->encoding ;
	        if ((si = matstr(contentencodings,enc,-1)) >= 0) {
	            code = si ;
	        }
	    }
	    if (rs < 0) {
	        if (op->encoding != nullptr) {
	            uc_free(op->encoding) ;
	            op->encoding = nullptr ;
	        }
	    }
	    if (rs >= 0) {
	        op->cte = code ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? code : rs ;
}
/* end subroutine (mailmsgattent_code) */

int mailmsgattent_setcode(MME *op,int code) noex {
	int		rs ;
        if ((rs = mailmsgattent_magic(op)) >= 0) {
            if ((code >= 0) && (code < CE_OVERLAST)) {
                op->cte = code ;
                if (op->encoding == nullptr) {
                    cchar       *cp ;
                    cchar       *enc = contentencodings[code] ;
                    if ((rs = uc_mallocstrw(enc,-1,&cp)) >= 0) {
                        op->encoding = cp ;
                    }
                }
            } else {
                rs = SR_INVALID ;
            }
        } /* end if (magic) */
        return rs ;
}
/* end subroutine (mailmsgattent_setcode) */

/* analyze (as best as we can) an attachment */
int mailmsgattent_analyze(MME *op,cchar *tmpdname) noex {
        int             rs ;
        int             rs1 ;
        int             code = 0 ;
	if ((rs = mailmsgattent_magic(op,tmpdname)) >= 0) {
            bfile	infile, *ifp = &infile ;
	    cchar	*atf = bfilestdfname(op->attfname) ;
            if ((rs = bopen(ifp,atf,"r",0666)) >= 0) {
                USTAT       sb ;
                if ((rs = bcontrol(ifp,BC_STAT,&sb)) >= 0) {
                    bfile   auxfile, *afp = &auxfile ;
                    int     f_needaux = true ;
                    char    auxfname[MAXPATHLEN+1] ;
                    auxfname[0] = '\0' ;
                    if (S_ISREG(sb.st_mode)) {
                        op->clen = (int) sb.st_size ;
                        f_needaux = false ;
                    }
                    if (f_needaux) {
                        cchar       *tmpcname = MAILMSGATTENT_TMPCNAME ;
                        char        tmpfname[MAXPATHLEN + 1] ;
                        if (tmpdname == nullptr) {
                            rs = SR_FAULT ;
			}
                        if (rs >= 0) {
                            if (tmpdname[0] != '\0') {
                                rs = mkpath2(tmpfname,tmpdname,tmpcname) ;
                            } else {
                                rs = mkpath1(tmpfname,tmpcname) ;
                            }
                        }
                        if (rs >= 0) {
                            cmode   om = 0660 ;
                            if ((rs = mktmpfile(auxfname,tmpfname,om)) >= 0) {
                                cchar       *afn = auxfname ;
                                cchar       *cp ;
                                if ((rs = uc_mallocstrw(afn,-1,&cp)) >= 0) {
                                    op->auxfname = cp ;
                                    rs = bopen(afp,op->auxfname,"wct",0666) ;
                                    if (rs < 0) {
                                        uc_free(op->auxfname) ;
                                        op->auxfname = nullptr ;
                                    }
                                } /* end if (m-a) */
                                if (rs < 0) {
                                    uc_unlink(auxfname) ;
                                    auxfname[0] = '\0' ;
                                }
                            } /* end if (mktmpfile) */
                        } /* end if (ok) */
                    } /* end if (needed an auxillary file) */
    /* finally! perform the analysis */
                    if (rs >= 0) {
                        if (! f_needaux) afp = nullptr ;
                        rs = mailmsgattent_analyzer(op,afp,ifp) ;
                        code = rs ;
                        if (rs < 0) {
                            if (op->auxfname != nullptr) {
                                uc_free(op->auxfname) ;
                                op->auxfname = nullptr ;
                            }
                            if (auxfname[0] != '\0') {
                                u_unlink(auxfname) ;
                            }
                        }
                    } /* end if (ok) */
                    if (f_needaux) {
                        if (afp != nullptr) bclose(afp) ;
                    } else {
                        bseek(ifp,0L,SEEK_SET) ;
                    }
                } /* end if (stat) */
                rs1 = bclose(ifp) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (file-open) */
	} /* end if (non-null) */
        return (rs >= 0) ? code : rs ;
}
/* end subroutine (mailmsgattent_analyze) */


/* private subroutines */

static int mailmsgattent_startfn(MME *op,cchar *sp,int sl) noex {
        int             rs = SR_OK ;
        int             cl ;
        cchar           *cp ;
        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
            cchar       *fn ;
            if ((rs = uc_mallocstrw(cp,cl,&fn)) >= 0) {
                op->attfname = fn ;
                rs = mailmsgattent_checkatt(op) ;
                if (rs < 0) {
                    uc_free(op->attfname) ;
                    op->attfname = nullptr ;
                }
            }
        }
        return rs ;
}
/* end subroutine (mailmsgattent_startfn) */

static int mailmsgattent_startct(MME *op,cchar *sp,int sl) noex {
        int             rs = SR_OK ;
        if (int si ; (si = sisub(sp,sl,"/")) >= 0) {
            if ((rs = mailmsgattent_startctpri(op,sp,si)) >= 0) {
                cint    cl = (sl-(si+1)) ;
                cchar   *cp = (sp+(si+1)) ;
                rs = mailmsgattent_startctsub(op,cp,cl) ;
            }
        } else {
            int         el ;
            cchar       *ep ;
            if ((el = sfshrink(sp,sl,&ep)) > 0) {
                int     oi ;
                if ((oi = matcasestr(contypevals,ep,el)) >= 0) {
                    rs = mailmsgattent_startctpri(op,ep,el) ;
                } else {
                    cchar       *ext ;
                    if ((rs = uc_mallocstrw(ep,el,&ext)) >= 0) {
                        op->ext = ext ;
                    }
                }
            }
        } /* end if */
        return rs ;
}
/* end subroutine (mailmsgattent_startct) */

static int mailmsgattent_startctpri(MME *op,cchar *sp,int sl) noex {
        int             rs = SR_OK ;
        int             cl ;
        cchar           *cp ;
        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
            cchar       *fn ;
            if ((rs = uc_mallocstrw(cp,cl,&fn)) >= 0) {
                op->type = fn ;
            }
        }
        return rs ;
}
/* end subroutine (mailmsgattent_startctpri) */

static int mailmsgattent_startctsub(MME *op,cchar *sp,int sl) noex {
        int             rs = SR_OK ;
        int             cl ;
        cchar           *cp ;
        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
            cchar       *fn ;
            if ((rs = uc_mallocstrw(cp,cl,&fn)) >= 0) {
                op->subtype = fn ;
            }
        }
        return rs ;
}
/* end subroutine (mailmsgattent_startctsub) */

static int mailmsgattent_checkatt(MME *op) noex {
        int             rs = SR_OK ;
        cchar           *fn = op->attfname ;
        if (fn != nullptr) {
            if (strcmp(fn,"-") != 0) {
                rs = perm(fn,-1,-1,nullptr,R_OK) ;
            }
        }
        return rs ;
}
/* end subroutine (mailmsgattent_checkatt) */

static int mailmsgattent_analyzer(MME *op,bfile *afp,bfile *ifp) noex {
        int             rs ;
	int		rs1 ;
        int             code = 0 ;
        char            *lbuf{} ;
	if ((rs = malloc_ml(&lbuf)) >= 0) {
            cint	llen = rs ;
            int		lines = 0 ;
            int		clen = 0 ;
            while ((rs = breadln(ifp,lbuf,llen)) > 0) {
                cint	len = rs ;
                if (code < CE_BINARY) {
                    for (int i = 0 ; i < len ; i += 1) {
                        cint	ch = mkchar(lbuf[i]) ;
                        if (ismmclass_binary(ch)) {
                            code = CE_BINARY ;
                        } else if ((code < CE_8BIT) && ismmclass_8bit(ch)) {
                            code = CE_8BIT ;
                        }
                        if (code >= CE_BINARY) break ;
                    } /* end for (class characterization) */
                } /* end if (continue) */
                if (afp != nullptr) {
                    rs = bwrite(afp,lbuf,len) ;
                } else {
                    if (code >= CE_BINARY) break ;
                }
                clen += len ;
                if (lbuf[len - 1] == '\n') lines += 1 ;
                if (rs < 0) break ;
            } /* end while */
            if (rs >= 0) {
                if (op->clen < 0) op->clen = clen ;
                op->clines = (code < CE_BINARY) ? lines : -1 ;
            }
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
        return (rs >= 0) ? code : rs ;
}
/* end subroutine (mailmsgattent_analyzer) */

static int freeit(void *p) noex {
        int             rs = SR_OK ;
        void            **pp = (void **) p ;
        if (*pp != nullptr) {
            rs = uc_free(*pp) ;
            *pp = nullptr ;
        }
        return rs ;
}
/* end subroutine (freeit) */


