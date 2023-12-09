/* envs_procxe */
/* lang=C++20 */

/* process an environment file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A�D� Morano
	This program was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine will read (process) an environment file and
	put all of the environment variables into an envs object
	(supplied). New environment variables just get added to the
	list. Old environment variables already on the list are
	deleted with a new definition is encountered.

	Synopsis:
	int envs_procxe(envs *nlp,expcook *clp,cc **ev,vs *dlp,cc *fn)

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
	�=		separated by ' '
	:=		separated by ':'
	�=		separated by ':', must be a directory 
	�=		separated by ' ', must be a directory

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>		/* <- for |UCHAR_MAX| */
#include	<unistd.h>
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
#include	<char.h>
#include	<mkchar.h>
#include	<ascii.h>
#include	<buffer.h>
#include	<snwcpy.h>
#include	<sfx.h>
#include	<ischarx.h>
#include	<strn.h>
#include	<vstrkeycmpx.h>
#include	<localmisc.h>

#include	"envs.h"


/* local defines */

#ifndef	DEFNAMELEN
#define	DEFNAMELEN	120
#endif

#ifndef	ENVNAMELEN
#define	ENVNAMELEN	120
#endif

#define	NLINES		4		/* number of continuation lines */
#define	NDEBFN		"/tmp/envset.deb"

#define	VS		vecstr
#define	EC		expcook
#define	SI		struct subinfo
#define	AT		struct assigntypes


/* local namespaces */

using std::nullptr_t ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	envs		*nlp ;
	EC		*clp ;
	cchar		**envv ;
	vecstr		*dlp ;
} ;

struct assigntypes {
	uint		add:1 ;		/* add (append) */
	uint		sep:1 ;		/* separator */
	uint		dir:1 ;		/* check if a directory */
	uint		path:1 ;	/* treat as a "path" */
	uint		ass:1 ;
	uint		uniqstr:1 ;	/* unique string */
} ;


/* forward references */

template<typename ... Args>
static inline int envs_magic(envs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == ENVS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int envs_procxer(SI *,cchar *) noex ;

static int subexpln(SI *,cchar *,int) noex ;
static int subln(SI *,cchar *,int) noex ;
static int sublner(SI *,cchar *,int,AT *,int,cchar *,int) noex ;
static int subdeps(SI *,cchar *,int) noex ;
static int subvals(SI *,buffer *,cchar *,cchar *,int) noex ;
static int procvalue(SI *,buffer *,cchar *,cchar *,int) noex ;
static int procsubdef(SI *,buffer *,cchar *,int) noex ;

static int	mkterms() noex ;


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

static char		vterms[termsize] ;

static char		dterms[termsize] ;

static bufsizevar	maxlinelen(getbufsize_ml) ;

static constexpr cchar	ssp[] = {
	CH_LPAREN, CH_RPAREN, 0
} ;

static constexpr cchar	strassign[] = "+:;����=-" ;


/* exported subroutines */

int envs_procxe(envs *op,EC *clp,cchar **ev,VS *dlp,cchar *fn) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = envs_magic(op,clp,ev,dlp,fn)) >= 0) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		static cint	srs = mkterms() ;
	        if ((rs = srs) >= 0) {
		    SI	li, *sip = &li ;
		    sip->nlp = op ;
		    sip->clp = clp ;
		    sip->envv = ev ;
		    sip->dlp = dlp ;
		    rs = envs_procxer(sip,fn) ;
		    c = rs ;
		} /* end if (mkterms) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (envs_procxe) */


/* local subroutines */

static int envs_procxer(SI *sip,cchar *fn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = maxlinelen) >= 0) {
	    char	*lbuf{} ;
	    cint	sz = ((rs+1)*NLINES) ;
	    if ((rs = uc_malloc((sz+1),&lbuf)) > 0) {
		cint	llen = sz ;
	        bfile	xefile, *xfp = &xefile ;
	        if ((rs = bopen(xfp,fn,"r",0666)) >= 0) {
		    nullptr_t	n{} ;
	            int		cl ;
	            int		len ;
	            cchar	*cp ;
	            while ((rs = breadlns(xfp,lbuf,llen,-1,n)) > 0) {
	        	len = rs ;
	        	if (lbuf[len - 1] == '\n') len -= 1 ;
	        	lbuf[len] = '\0' ;
	        	if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
		    	    if (cp[0] != '#') {
	                	c += 1 ;
	                	rs = subexpln(sip,cp,cl) ;
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
/* end subroutine (envs_procxer) */

static int subexpln(SI *sip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (sl < 0) sl = strlen(sp) ;
	if (strnchr(sp,sl,'%') != nullptr) {
	    buffer	b ;
	    EC		*clp = sip->clp ;
	    cint	bsize = (sl+20) ;
	    if ((rs = buffer_start(&b,bsize)) >= 0) {
	        if ((rs = expcook_expbuf(clp,0,&b,sp,sl)) >= 0) {
	            cint	bl = rs ;
	            cchar	*bp ;
	            if ((rs = buffer_get(&b,&bp)) >= 0) {
	                rs = subln(sip,bp,bl) ;
	                len = rs ;
	            } /* end if (buffer-get) */
	        } /* end if (expand-buf) */
	        rs1 = buffer_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (buffer) */
	} else {
	    rs = subln(sip,sp,sl) ;
	    len = rs ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subexpln) */

/****
	LOCAL ?		PATH �= 		$(LOCAL)/bin
	LOCAL ?		PATH �= 		$(LOCAL)/bin
	PATH 		     �= 		$(LOCAL)/bin
	PATH  					$(LOCAL)/bin
****/

static int subln(SI *sip,cchar *sp,int sl) noex {
	AT		at{} ;
	int		rs = 1 ; /* something positive (see code below) */
	int		el ;
	int		enl = 0 ;
	int		len = 0 ;
	nullptr_t	n{} ;
	cchar		*scl = "?+:;���=#\t " ;
	cchar		*tp, *ep ;
	cchar		*enp = nullptr ;
	char		envname[ENVNAMELEN + 1] ;
	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
/* extract any dependencies (if we have any) */
	if (((tp = strnpbrk(sp,sl,scl)) != n) && (*tp == '?')) {
	    if ((rs = subdeps(sip,sp,(tp - sp))) > 0) {
	        sl -= ((tp+1) - sp) ;
	        sp = (tp+1) ;
	        while (sl && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	    } /* end if (subdeps) */
	} /* end if (getting dependencies) */
	if (rs > 0) { /* greater-than */
	    if ((el = sfbreak(sp,sl,strassign,&ep)) > 0) {
	        int	ach ;
	        int	sch = 0 ;
	        bool	f_exit = false ;
	        bool	f_go = true ;
	        sl -= ((ep+el)-sp) ;
	        sp = (ep+el) ;
	        while (sl && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        while ((tp = strnpbrk(sp,1,strassign)) != nullptr) {
	            ach = MKCHAR(tp[0]) ;
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
	            case MKCHAR('�'):
	                at.dir = true ;
	                break ;
	            case MKCHAR('�'):
	                at.add = true ;
	                at.sep = true ;
	                at.dir = true ;
	                at.uniqstr = false ;
	                at.path = true ;
	                if (sch == 0) sch = ':' ;
	                break ;
	            case MKCHAR('�'):
	                at.add = true ;
	                at.sep = true ;
	                at.dir = true ;
	                at.uniqstr = true ;
	                if (sch == 0) sch = ' ' ;
	                break ;
	            case MKCHAR('�'):
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
	        while (sl && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        {
	            int		cl ;
	            cchar	*cp ;
	            if ((cl = sfshrink(ep,el,&cp)) > 0) {
	                enp = envname ;
	                enl = cl ;
	                if (snwcpy(envname,ENVNAMELEN,cp,cl) < 0) {
	                    f_go = false ;
			}
	            }
	        }
/* do we already have this definition? */
/* loop processing the values */
	        if (f_go) {
	            rs = sublner(sip,enp,enl,&at,sch,sp,sl) ;
		}
	    } /* end if (sfbreak) */
	} /* end if (positive) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subln) */

static int sublner(SI *sip,cc *enp,int enl,AT *atp,int sch,cc *sp,int sl) noex {
	buffer		b ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if ((sl >= 0) && ((rs = buffer_start(&b,sl)) >= 0)) {
	    if ((rs = subvals(sip,&b,ssp,sp,sl)) >= 0) {
	        bool	f_store = true ;
	        cchar	*cp ;
	        if ((rs = buffer_get(&b,&cp)) >= 0) {
	            int	cl = rs ;
	            if ((rs >= 0) && f_store && atp->uniqstr) {
	                envs	*nlp = sip->nlp ;
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
	                        if (u_stat(dname,&sb) >= 0) {
	                            if (! S_ISDIR(sb.st_mode)) {
	                                f_store = false ;
				    }
	                        } else {
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
	                envs	*nlp = sip->nlp ;
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
	    } /* end if (subvals) */
	    len = buffer_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (buffer initialization) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sublner) */

static int subdeps(SI *sip,cchar *sp,int sl) noex {
	field		fsb ;
	int		rs ;
	int		rs1 = 0 ;
	int		f = true ;
	if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	    cint	dlen = DEFNAMELEN ;
	    int		fl ;
	    cchar	*fp ;
	    char	dbuf[DEFNAMELEN + 1] ;
	    while ((fl = field_get(&fsb,dterms,&fp)) >= 0) {
	        if (fl > 0) {
	            if ((rs1 = snwcpy(dbuf,dlen,fp,fl)) >= 0) {
			nullptr_t	n{} ;
			auto vs = vstrkeycmp ;
	                rs1 = vecstr_search(sip->dlp,dbuf,vs,n) ;
		    }
		}
	        if (rs1 < 0) break ;
	    } /* end while */
	    f = (rs1 >= 0) ;
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subdeps) */

/* process definition values */
static int subvals(SI *sip,buffer *bp,cchar *ss,cchar *sp,int sl) noex {
	field		fsb ;
	int		rs ;
	int		len = 0 ;
	if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	    cint	flen = sl ;
	    char	*fbuf ;
	    if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	        int	fl ;
	        int	c = 0 ;
	        while ((fl = field_sharg(&fsb,vterms,fbuf,flen)) >= 0) {
	            if (fl > 0) {
	                if (c++ > 0) {
	                    rs = buffer_char(bp,' ') ;
	                    len += rs ;
	                }
	                if (rs >= 0) {
	                    rs = procvalue(sip,bp,ss,fbuf,fl) ;
	                    len += rs ;
	                }
	            } /* end if (non-zero) */
	            if (fsb.term == '#') break ;
	            if (rs < 0) break ;
	        } /* end while (looping over values) */
	        uc_free(fbuf)  ;
	    } /* end if (m-a) */
	    field_finish(&fsb) ;
	} /* end if (field) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subvals) */

static int procvalue(SI *sip,buffer *bp,cchar *ss,cchar *sp,int sl) noex {
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
	        rs = procsubdef(sip,bp,kp,kl) ;
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
/* end subroutine (procvalue) */

static int procsubdef(SI *sip,buffer *bp,cchar *kp,int kl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (kl != 0) {
	    int		al = 0 ;
	    int		cl ;
	    cchar	*ap = nullptr ;
	    cchar	*tp, *cp ;
	    char	defbuf[DEFNAMELEN + 1] ;
	    if ((tp = strnchr(kp,kl,'=')) != nullptr) {
	        ap = (tp + 1) ;
	        al = (kp + kl) - (tp + 1) ;
	        kl = (tp - kp) ;
	    }
	    if ((rs1 = snwcpy(defbuf,DEFNAMELEN,kp,kl)) >= 0) {
	        auto	vs = vstrkeycmp ;
/* lookup the define key-name that we have */
	        if ((rs1 = vecstr_search(sip->dlp,defbuf,vs,&cp)) >= 0) {
	            cl = strlen(cp) ;
	            if ((tp = strchr(cp,'=')) != nullptr) {
	                cl -= ((tp + 1) - cp) ;
	                cp = (tp + 1) ;
	            }
	        }
/* perform any appropriate substitution */
	        if (rs1 >= 0) {
	            rs = buffer_strw(bp,cp,cl) ;
	            len += rs ;
	        } else if (al > 0) {
	            rs = buffer_strw(bp,ap,al) ;
	            len += rs ;
	        }
	    } /* end if (snwcpy) */
	} /* end if (non-zero) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procsubdef) */

static int mkterms() noex {
	int		rs ;
	if ((rs = fieldterms(vterms,false,'\t',' ','#',':',';')) >= 0) {
	    rs = fieldterms(dterms,false,'\t',' ',',') ;
	}
	return rs ;
}
/* end subroutine (mkterms) */


