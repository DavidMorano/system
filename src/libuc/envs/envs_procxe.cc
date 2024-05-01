/* envs_procxe SUPPORT */
/* lang=C++20 */

/* process an environment file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine will read (process) an environment file and
	put all of the environment variables into an envs object
	(supplied). New environment variables just get added to the
	list. Old environment variables already on the list are
	deleted with a new definition is encountered.

	Synopsis:
	int envs_procxe(envs *nlp,expcook *clp,mainv ev,vs *dlp,cc *fn) noex

	Arguments:
	nlp		new-list-pointer, new (forming) environment list
	clp		|EC| cookie-list-pointer
	ev		process environment
	dlp		|vecstr| "defines" list pointer
	fname		file to process

	Returns:
	>=0		count of environment variables
	<0		bad

	Notes:
	- Assignments:
	=		any previous value is overwritten
	= Additions:
	+=		added with no separation
	­=		separated by ' '
	:=		separated by ':'
	¶=		separated by ':', must be a directory 
	µ=		separated by ' ', must be a directory

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>		/* <- for |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<expcook.h>
#include	<vecstr.h>
#include	<nulstr.h>
#include	<bfile.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<ascii.h>
#include	<buffer.h>
#include	<snwcpy.h>
#include	<sfx.h>
#include	<strn.h>
#include	<vstrkeycmpx.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"envs.h"


/* local defines */

#ifndef	ENVNAMELEN
#define	ENVNAMELEN	120
#endif

#define	NLINES		4		/* number of continuation lines */
#define	NDEBFN		"/tmp/envset.deb"

#define	VS		vecstr
#define	EC		expcook
#define	SI		struct subinfo
#define	AT		struct assigntypes


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct assigntypes {
	uint		add:1 ;		/* add (append) */
	uint		sep:1 ;		/* separator */
	uint		dir:1 ;		/* check if a directory */
	uint		path:1 ;	/* treat as a "path" */
	uint		ass:1 ;
	uint		uniqstr:1 ;	/* unique string */
    } ; /* end struct (assigntypes) */
    struct subinfo {
	envs		*nlp ;
	expcook		*clp ;
	vecstr		*dlp ;
	mainv		envv ;
	subinfo(envs *op,EC *aclp,mainv ev,vecstr *adlp) noex {
		    nlp = op ;
		    clp = aclp ;
		    dlp = adlp ;
		    envv = ev ;
	} ; /* end if (ctor) */
	int procer(cchar *) noex ;
	int expln(cchar *,int) noex ;
	int ln(cchar *,int) noex ;
	int lner(cchar *,int) noex ;
	int lner(cchar *,int,AT *,int,cchar *,int) noex ;
	int deps(cchar *,int) noex ;
	int vals(buffer *,cchar *,cchar *,int) noex ;
	int val(buffer *,cchar *,cchar *,int) noex ;
	int def(buffer *,cchar *,int) noex ;
    } ; /* end struct (subinfo) */
}


/* forward references */

template<typename ... Args>
static inline int envs_magic(envs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == ENVS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	mkterms() noex ;


/* local variables */

constexpr int		rsn = SR_NOTFOUND ;
constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;
constexpr int		envnamelen = ENVNAMELEN ;

static char		vterms[termsize] ;

static char		dterms[termsize] ;

static bufsizevar	maxlinelen(getbufsize_ml) ;

static constexpr cchar	ssp[] = {
	CH_LPAREN, 
	CH_RPAREN,
	0
} ;

static constexpr cchar	strassign[] = "+:;¶µ­Ð=-" ;


/* exported variables */


/* exported subroutines */

int envs_procxe(envs *op,EC *clp,mainv ev,VS *dlp,cchar *fn) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = envs_magic(op,clp,ev,dlp,fn)) >= 0) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		static cint	srs = mkterms() ;
	        if ((rs = srs) >= 0) {
		    subinfo	si(op,clp,ev,dlp) ;
		    rs = si.procer(fn) ;
		    c = rs ;
		} /* end if (mkterms) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (envs_procxe) */


/* local subroutines */

int subinfo::procer(cchar *fn) noex {
	const nullptr_t	np{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = maxlinelen) >= 0) {
	    cint	sz = ((rs + 1) * NLINES) ;
	    char	*lbuf{} ;
	    if ((rs = uc_malloc((sz + 1),&lbuf)) > 0) {
		cint	llen = sz ;
	        bfile	xefile, *xfp = &xefile ;
	        if ((rs = bopen(xfp,fn,"r",0666)) >= 0) {
	            int		cl ;
	            int		len ;
	            cchar	*cp ;
	            while ((rs = breadlns(xfp,lbuf,llen,-1,np)) > 0) {
	        	len = rs ;
	        	if (lbuf[len - 1] == '\n') len -= 1 ;
	        	lbuf[len] = '\0' ;
	        	if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
		    	    if (cp[0] != '#') {
	                	c += 1 ;
	                	rs = expln(cp,cl) ;
		    	    }
			}
	        	if (rs < 0) break ;
	    	    } /* end while (reading lines) */
	    	    rs1 = bclose(xfp) ;
	    	    if (rs >= 0) rs = rs1 ;
		} /* end if (file-open) */
		rs1 = uc_free(lbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (maxlinelen) */
	return (rs >= 0) ? c : rs ;
}
/* end method (subinfo::procer) */

int subinfo::expln(cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (sl < 0) sl = strlen(sp) ;
	if (strnchr(sp,sl,'%') != nullptr) {
	    buffer	b ;
	    cint	bsize = (sl+20) ;
	    if ((rs = buffer_start(&b,bsize)) >= 0) {
	        if ((rs = expcook_expbuf(clp,0,&b,sp,sl)) >= 0) {
	            cint	bl = rs ;
	            cchar	*bp ;
	            if ((rs = buffer_get(&b,&bp)) >= 0) {
	                rs = ln(bp,bl) ;
	                len = rs ;
	            } /* end if (buffer-get) */
	        } /* end if (expand-buf) */
	        rs1 = buffer_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (buffer) */
	} else {
	    rs = ln(sp,sl) ;
	    len = rs ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::expln) */

/****
	LOCAL ?		PATH ¶= 		$(LOCAL)/bin
	LOCAL ?		PATH ¶= 		$(LOCAL)/bin
	PATH 		     ¶= 		$(LOCAL)/bin
	PATH  					$(LOCAL)/bin
****/

int subinfo::ln(cchar *sp,int sl) noex {
	AT		at{} ;
	cnullptr	np{} ;
	int		rs = 1 ; /* something positive (see code below) */
	int		el ;
	int		enl = 0 ;
	int		len = 0 ;
	cchar		*scl = "?+:;¶µ­=#\t " ;
	cchar		*tp, *ep ;
	cchar		*enp = nullptr ;
	char		envnamebuf[envnamelen+ 1] ;
	while (sl && char_iswhite(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
/* extract any dependencies (if we have any) */
	if (((tp = strnpbrk(sp,sl,scl)) != np) && (*tp == '?')) {
	    if ((rs = deps(sp,(tp - sp))) > 0) {
	        sl -= ((tp+1) - sp) ;
	        sp = (tp+1) ;
	        while (sl && char_iswhite(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	    } /* end if (subinfo::deps) */
	} /* end if (getting dependencies) */
	if (rs > 0) { /* greater-than */
	    if ((el = sfbrk(sp,sl,strassign,&ep)) > 0) {
	        int	sch = 0 ;
	        bool	f_exit = false ;
	        bool	f_go = true ;
	        sl -= ((ep+el)-sp) ;
	        sp = (ep+el) ;
	        while (sl && char_iswhite(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        while ((tp = strnpbrk(sp,1,strassign)) != nullptr) {
	            cint	ach = mkchar(tp[0]) ;
	            switch (ach) {
	            case '+':
	                at.add = true ;
	                break ;
	            case ':':
	            case ';':
	                at.add = true ;
	                at.sep = true ;
	                sch = ach ;
	                break ;
	            case '-':
	            case '=':
	                at.ass = true ;
	                f_exit = true ;
	                break ;
	            case mkchar('Ð'):
	                at.dir = true ;
	                break ;
	            case mkchar('¶'):
	                at.add = true ;
	                at.sep = true ;
	                at.dir = true ;
	                at.uniqstr = false ;
	                at.path = true ;
	                if (sch == 0) sch = ':' ;
	                break ;
	            case mkchar('µ'):
	                at.add = true ;
	                at.sep = true ;
	                at.dir = true ;
	                at.uniqstr = true ;
	                if (sch == 0) sch = ' ' ;
	                break ;
	            case mkchar('­'):
	                at.add = true ;
	                at.sep = true ;
	                if (sch == 0) sch = ' ' ;
	                break ;
	            default:
	                f_exit = true ;
	                break ;
	            } /* end switch */
	            sp += 1 ;
	            sl -= 1 ;
	            if (f_exit) break ;
	        } /* end while */
	        while (sl && char_iswhite(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        {
	            cchar	*cp ;
	            if (int cl ; (cl = sfshrink(ep,el,&cp)) > 0) {
	                enp = envnamebuf ;
	                enl = cl ;
	                if (snwcpy(envnamebuf,envnamelen,cp,cl) < 0) {
	                    f_go = false ;
			}
	            }
	        } /* end block */
/* do we already have this definition? */
/* loop processing the values */
	        if (f_go) {
	            rs = lner(enp,enl,&at,sch,sp,sl) ;
		}
	    } /* end if (sfbrk) */
	} /* end if (positive) */
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::ln) */

int subinfo::lner(cc *enp,int enl,AT *atp,int sch,cc *sp,int sl) noex {
	buffer		b ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if ((sl >= 0) && ((rs = buffer_start(&b,sl)) >= 0)) {
	    if ((rs = vals(&b,ssp,sp,sl)) >= 0) {
	        bool	f_store = true ;
	        cchar	*cp{} ;
	        if ((rs = buffer_get(&b,&cp)) >= 0) {
	            cint	cl = rs ;
	            if ((rs >= 0) && f_store && atp->uniqstr) {
	                if ((rs = envs_substr(nlp,enp,enl,sp,sl)) > 0) {
			    f_store = false ;
	                } else if (rs == SR_NOTFOUND) {
	                    rs = SR_OK ;
	                }
		    } /* end if (unique string) */
	            if ((rs >= 0) && f_store && atp->dir) {
	                if (cl > 0) {
			    nulstr	ns ;
			    cchar	*dname{} ;
			    if ((rs = ns.start(cp,cl,&dname)) >= 0) {
	                        USTAT	sb ;
	                        if ((rs = u_stat(dname,&sb)) >= 0) {
	                            if (! S_ISDIR(sb.st_mode)) {
	                                f_store = false ;
				    }
	                        } else if (rs == rsn) {
				    rs = SR_OK ;
	                            f_store = false ;
			        }
				rs1 = ns.finish ;
				if (rs >= 0) rs = rs1 ;
	                    } /* end if (nulstr) */
	                } else {
	                    f_store = false ;
		        }
	            } /* end if (directory check) */
	            if ((rs >= 0) && f_store) {
	                if (atp->sep) {
	                    if ((rs = envs_present(nlp,enp,enl)) > 0) {
	                        char	sbuf[2] ;
	                        sbuf[0] = sch ;
	                        sbuf[1] = '\0' ;
	                        rs = envs_store(nlp,enp,true,sbuf,1) ;
	                    } else if (rs == SR_NOTFOUND) {
	                        rs = SR_OK ;
	                    }
	                } /* end if (sep) */
	                if (rs >= 0) {
	                    bool	f_append = atp->add ;
	                    rs = envs_store(nlp,enp,f_append,cp,cl) ;
	                } /* end if */
	            } /* end if (store) */
	        } /* end if (buffer_get) */
	    } /* end if (subinfo::vals) */
	    len = buffer_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (buffer initialization) */
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::lner) */

int subinfo::deps(cchar *sp,int sl) noex {
	cnullptr	np{} ;
	field		fsb ;
	int		rs ;
	int		rs1 = 0 ;
	int		f = true ;
	if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	    int		fl ;
	    cchar	*fp{} ;
	    while ((fl = field_get(&fsb,dterms,&fp)) >= 0) {
	        if (fl > 0) {
		    auto	vs = vstrkeycmp ;
	            if ((rs = vecstr_searchl(dlp,fp,fl,vs,np)) >= 0) {
			f = true ;
		    } else if (rs == rsn) {
			rs = SR_OK ;
		    }
		} /* end if (non-zero positive) */
		if (rs < 0) break ;
	    } /* end while */
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? f : rs ;
}
/* end method (subinfo::deps) */

/* process definition values */
int subinfo::vals(buffer *bp,cchar *ss,cchar *sp,int sl) noex {
	cint		flen = sl ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*fbuf{} ;
	if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	    field	fsb ;
	    if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	        int	fl ;
	        int	c = 0 ;
	        while ((fl = field_sharg(&fsb,vterms,fbuf,flen)) >= 0) {
	            if (fl > 0) {
	                if (c++ > 0) {
	                    rs = buffer_char(bp,' ') ;
	                    len += rs ;
	                }
	                if (rs >= 0) {
	                    rs = val(bp,ss,fbuf,fl) ;
	                    len += rs ;
	                }
	            } /* end if (non-zero) */
	            if (fsb.term == '#') break ;
	            if (rs < 0) break ;
	        } /* end while (looping over values) */
	        rs1 = field_finish(&fsb) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	    rs1 = uc_free(fbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::vals) */

int subinfo::val(buffer *bp,cchar *ss,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		kl, cl ;
	int		len = 0 ;
	cchar		*kp, *cp ;
	while ((kl = sfthing(sp,sl,ss,&kp)) >= 0) {
	    cp = sp ;
	    cl = ((kp - 2) - sp) ;
	    if (cl > 0) {
	        rs = buffer_strw(bp,cp,cl) ;
	        len += rs ;
	    }
	    if ((rs >= 0) && (kl > 0)) {
	        rs = def(bp,kp,kl) ;
	        len += rs ;
	    }
	    sl -= ((kp + kl + 1) - sp) ;
	    sp = (kp + kl + 1) ;
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && (sl > 0)) {
	    rs = buffer_strw(bp,sp,sl) ;
	    len += rs ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::val) */

int subinfo::def(buffer *bp,cchar *kp,int kl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (kl != 0) {
	    int		al = 0 ;
	    cchar	*ap = nullptr ;
	    cchar	*tp ;
	    if ((tp = strnchr(kp,kl,'=')) != nullptr) {
	        ap = (tp + 1) ;
	        al = (kp + kl) - (tp + 1) ;
	        kl = (tp - kp) ;
	    }
	    {
		int	cl{} ;
		cchar	*cp{} ;
	        auto	vs = vstrkeycmp ;
		bool	f_found = false ;
	        if ((rs = vecstr_searchl(dlp,kp,kl,vs,&cp)) >= 0) {
		    f_found = true ;
	            cl = strlen(cp) ;
	            if ((tp = strchr(cp,'=')) != nullptr) {
	                cl -= ((tp + 1) - cp) ;
	                cp = (tp + 1) ;
	            }
	        } else if (rs == rsn) {
		    rs = SR_OK ;
		}
/* perform any appropriate substitution */
		if (rs >= 0) {
	            if (f_found) {
	                rs = buffer_strw(bp,cp,cl) ;
	                len += rs ;
	            } else if (al > 0) {
	                rs = buffer_strw(bp,ap,al) ;
	                len += rs ;
	            }
		} /* end if (ok) */
	    } /* end block */
	} /* end if (non-zero) */
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::def) */

static int mkterms() noex {
	int		rs ;
	if ((rs = fieldterms(vterms,false,'\t',' ','#',':',';')) >= 0) {
	    rs = fieldterms(dterms,false,'\t',' ',',') ;
	}
	return rs ;
}
/* end subroutine (mkterms) */


