/* svcentry SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* build up a program entry piece-meal as it were */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was written.

	= 2024-09-26, David A­D­ Morano
	I am reviewing this code and making some minor refactoring
	(to C++20 standard).

*/

/* Copyright © 1998,2024 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	svcentry

	Description:
	This little object is used to create a "program entry"
	(whatever that means) and to populate aspects of it with
	various operations on the object.  This object is used in
	"server" types of programs.  This object is usually populated
	from elements taken from the parsing of a sevice-table
	(SVCTAB) file (which is used by various servers).

	Notes:
	= David A-D- Morano (2024-09-26)
	This code is quite (very) old.  It is not (I think) more
	than thirty (30) years old, but it is very likely more than
	twenty-five (25) years old.  The copyright year (above)
	says 1998.  That means that the code was originally written,
	or adapted from prior code, in or before 1998.  That seems
	about correct.  I say that this code is old because of the
	style and use of the in-file expansion code (subroutine
	below |args_expand()|).  I would not have written that
	custom in-file c-string expansion subroutine if the code
	was younger than at least twenty-five (25) years ago.  Also,
	I think I was a little bit fast-and-loose with my error
	handling in some places.  This (questionable error handling)
	is not necessarily a sign of age, but rather poor (or
	haphazard) thinking at the time of the coding.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |pid_t| */
#include	<sys/param.h>
#include	<unistd.h>
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<strlibval.hh>
#include	<vecstr.h>
#include	<varsub.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<sbuf.h>
#include	<svcfile.h>
#include	<strn.h>
#include	<snx.h>
#include	<snwcpy.h>
#include	<sfx.h>
#include	<mktmp.h>
#include	<strwcpy.h>
#include	<cfdec.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"svcentry.h"
#include	"svckey.h"

import libutil ;

/* local defines */

#define	TMPLEN		14		/* old SysV file-len */
#define	BUFMULT		10		/* expansion buffer path multiplier */

#define	SE		svcentry
#define	ARGS		svcentry_args
#define	ENT		svcfile_ent


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct vars {
	int		olen ;
} ;


/* forward references */

template<typename ... Args>
static int svcentry_ctor(svcentry *op,Args ... args) noex {
	SVCENTRY	*hop = (SVCENTRY *) op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->sap = new(nothrow) vecstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcentry_ctor) */

static int svcentry_dtor(svcentry *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    {
		delete op->sap ;
		op->sap = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcentry_dtor) */

template<typename ... Args>
static inline int svcentry_magic(svcentry *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SVCENTRY_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (svcentry_magic) */

static int	svcentry_starter(SE *,svckey *,ARGS *) noex ;
static int	svcentry_proc(SE *,cchar *,ARGS *,char *,int) noex ;
static int	svcentry_mkfile(SE *,cchar *,int) noex ;

static int	args_expand(ARGS *,char *,int,cchar *,int) noex ;
static int	vecstr_procargs(vecstr *,char *) noex ;
static int	mkfile(char *,cchar *,int) noex ;
static int	mkpat(char *,int,cchar *,int) noex ;
static int	mkvars() noex ;

static void	delfreeit(cchar *&) noex ;
static void	freeit(cchar **) noex ;


/* external variables */


/* local variables */

constexpr fieldterminit		pt(" \t") ;

constexpr int			tmplen = TMPLEN ;

static vars			var ;

static strlibval		deftmpdname(strlibval_tmpdir) ;


/* exported variables */


/* exported subroutines */

int svcentry_start(SE *op,varsub *ssp,ENT *sep,ARGS * esap) noex {
	int		rs ;
	if ((rs = svcentry_ctor(op,sep)) >= 0) {
	    static cint		rsv = mkvars() ;
	    if ((rs = rsv) >= 0) {
	        if (char *bp ; (rs = malloc_mn(&bp)) >= 0) {
		    cint	namelen = rs ;
		    op->name = bp ;
	            op->ssp = ssp ;
	            op->atime = esap->daytime ;	/* job arrival time */
		    op->jobid[0] = '\0' ;
	            if (svckey sk ; (rs = svckey_load(&sk,sep)) >= 0) {
		        strwcpy(op->name,sk.svc,namelen) ;
		        {
			    rs = svcentry_starter(op,&sk,esap) ;
		        }
		    } /* end if (svckey_load) */
	        } /* end if (memory-allocation) */
	    } /* end if (mkvars) */
	    if (rs < 0) {
		svcentry_dtor(op) ;
	    }
	} /* end if (svcentry_ctor) */
	return rs ;
}
/* end subroutine (svcentry_start) */

int svcentry_finish(SE *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = svcentry_magic(op)) >= 0) {
	    if (op->ofname != nullptr) {
	        if (op->ofname[0] != '\0') {
		    uc_unlink(op->ofname) ;
		}
		{
	            rs1 = uc_free(op->ofname) ;
	            if (rs >= 0) rs = rs1 ;
	            op->ofname = nullptr ;
		}
	    }
	    if (op->efname != nullptr) {
	        if (op->efname[0] != '\0') {
		    uc_unlink(op->efname) ;
		}
		{
	            rs1 = uc_free(op->efname) ;
	            if (rs >= 0) rs = rs1 ;
	            op->efname = nullptr ;
		}
	    }
	    if (op->f.srvargs) {
	        op->f.srvargs = false ;
	        rs1 = vecstr_finish(op->sap) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->program != nullptr) {
	        rs1 = uc_free(op->program) ;
	        if (rs >= 0) rs = rs1 ;
	        op->program = nullptr ;
	    }
	    if (op->username != nullptr) {
	        rs1 = uc_free(op->username) ;
	        if (rs >= 0) rs = rs1 ;
	        op->username = nullptr ;
	    }
	    if (op->groupname != nullptr) {
	        rs1 = uc_free(op->groupname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->groupname = nullptr ;
	    }
	    if (op->options != nullptr) {
	        rs1 = uc_free(op->options) ;
	        if (rs >= 0) rs = rs1 ;
	        op->options = nullptr ;
	    }
	    if (op->access != nullptr) {
	        rs1 = uc_free(op->access) ;
	        if (rs >= 0) rs = rs1 ;
	        op->access = nullptr ;
	    }
	    if (op->name) {
	        rs1 = uc_free(op->name) ;
	        if (rs >= 0) rs = rs1 ;
	        op->name = nullptr ;
	    }
	    {
		rs1 = svcentry_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->program = nullptr ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_finish) */

int svcentry_getaccess(SE *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = svcentry_magic(op)) >= 0) {
	    if (rpp) *rpp = op->access ;
	    rs = (op->access) ? SR_OK : SR_EMPTY ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getaccess) */

/* retrieve for caller the execution interval for this entry */
int svcentry_getinterval(SE *op,int *rp) noex {
	int		rs ;
	if ((rs = svcentry_magic(op)) >= 0) {
	    if (rp) {
		*rp = op->interval ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getinterval) */

int svcentry_getargs(SE *op,mainv avp) noex {
	int		rs ;
	if ((rs = svcentry_magic(op,avp)) >= 0) {
	    rs = vecstr_getvec(op->sap,&avp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getargs) */

namespace {
    struct expander {
	SE		*op ;
	ENT		*sep ;
	ARGS		*esap ;
	cchar		*argz{} ;
	char		*obuf{} ;
	svckey		sk ;
	cint		olen = var.olen ;
	expander(SE *o,ENT *s,ARGS *e) noex : op(o), sep(s), esap(e) { } ;
	expander() = delete ;
	operator int () noex ;
	int mkfiles() noex ;
	int mkcomps() noex ;
	int comp_p() noex ;
	int comp_a() noex ;
	int comp_u() noex ;
	int comp_g() noex ;
	int comp_o() noex ;
	int comp_prog() noex ;
	int comp_def() noex  ;
	void compfrees() noex ;
    } ; /* end struct (expander) */
    typedef int (expander::*expander_m)() noex ;
}

constexpr expander_m	compmems[] = {
	&expander::comp_p,
	&expander::comp_a,
	&expander::comp_u,
	&expander::comp_g,
	&expander::comp_o,
	&expander::comp_prog,
	&expander::comp_def
} ;

int svcentry_expand(SE *op,ENT *sep,ARGS *esap) noex {
	int		rs ;
	if ((rs = svcentry_magic(op,sep,esap)) >= 0) {
	    expander	eo(op,sep,esap) ;
	    rs = eo ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_expand) */

expander::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = svckey_load(&sk,sep)) >= 0) {
	    cchar	*oldservice = esap->service ;
	    cchar	*oldinterval = esap->interval ;
	    esap->service = sk.svc ;
	    esap->interval = sk.interval ;
	    if ((rs = uc_malloc((olen + 1),&obuf)) >= 0) {
		{
		    rs = mkfiles() ;
		    rv = rs ;
		}
		rs1 = uc_free(obuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    esap->interval = oldinterval ;
	    esap->service = oldservice ;
	} /* end if (svckey_load) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (svcentry_expand) */

int expander::mkfiles() noex {
	int		rs = SR_OK ;
	cchar		*tmpdname ;
	if (esap->jobid) {
	    strwcpy(op->jobid,esap->jobid,SVCENTRY_IDLEN) ;
	}
	/* did they supply a TMPDIR? */
	tmpdname = (esap->tmpdname) ? esap->tmpdname : deftmpdname ;
	/* make some temporary files for program file input and output */
	if ((rs = svcentry_mkfile(op,tmpdname,'o')) >= 0) {
	    if ((rs = svcentry_mkfile(op,tmpdname,'e')) >= 0) {
		{
		    rs = mkcomps() ;
		    if (rs < 0) {
			compfrees() ;
		    }
		}
		if (rs < 0) {
		    delfreeit(op->efname) ;
		} /* end if (error) */
	    } /* end if (svcentry_mkfile) */
	    if (rs < 0) {
		delfreeit(op->ofname) ;
	    } /* end if (error) */
	} /* end if (svcentry_mkfile) */
	return rs ;
}
/* end method (expander::mkfiles) */

int expander::mkcomps() noex {
	int		rs = SR_OK ;
	for (auto m : compmems) {
	    rs = (this->*m)() ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end method (expander::mkcomps) */

int expander::comp_p() noex {
	int		rs = SR_OK ;
	if (sk.p) {
	    if ((rs = svcentry_proc(op,sk.p,esap,obuf,olen)) >= 0) {
		cchar	*cp ;
	        if (int cl ; (cl = sfshrink(obuf,rs,&cp)) > 0) {
	            if (cchar *ap ; (rs = uc_mallocstrw(cp,cl,&ap)) >= 0) {
		        op->program = ap ;
		    }
	        } else {
		    rs = SR_INVALID ;
	        }
	    } /* end if */
	} /* end if (program path) */
	return rs ;
}
/* end method (expander::comp_p) */

int expander::comp_a() noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	argz = nullptr ;
	if (sk.a) {
	    if ((rs = svcentry_proc(op,sk.a,esap,obuf,olen)) >= 0) {
		cint	vn = 6 ;
	        cint	vo = VECSTR_OCOMPACT ;
	        if ((rs = vecstr_start(op->sap,vn,vo)) >= 0) {
	            op->f.srvargs = true ;
	            if ((rs = vecstr_procargs(op->sap,obuf)) > 0) {
			if ((rs = vecstr_get(op->sap,0,&argz)) >= 0) {
			    rs = SR_OK ;
			} else if (rs == rsn) {
			    rs = SR_OK ;
			    argz = nullptr ;
			}
		    } /* end if (vecstr_procargs) */
		} /* end if (vecstr_start) */
	    } /* end if (svcentry_proc) */
	} /* end if (program arguments) */
	return rs ;
}
/* end method (expander::comp_a) */

int expander::comp_u() noex {
	int		rs = SR_OK ;
	if (sk.u) {
	    if ((rs = svcentry_proc(op,sk.u,esap,obuf,olen)) >= 0) {
	        if (cchar *cp ; (rs = uc_mallocstrw(obuf,rs,&cp)) >= 0) {
		    op->username = cp ;
	        }
	    } /* end if (svcentry_proc) */
	} /* end if (username field) */
	return rs ;
}
/* end method (expander::comp_u) */

int expander::comp_g() noex {
	int		rs = SR_OK ;
	if (sk.g) {
	    if ((rs = svcentry_proc(op,sk.g,esap,obuf,olen)) >= 0) {
	        if (cchar *cp ; (rs = uc_mallocstrw(obuf,rs,&cp)) >= 0) {
		    op->groupname = cp ;
	        }
	    } /* end if (svcentry_proc) */
	}
	return rs ;
}
/* end method (expander::comp_g) */

int expander::comp_o() noex {
	int		rs = SR_OK ;
	if (sk.opts) {
	    if ((rs = svcentry_proc(op,sk.opts,esap,obuf,olen)) >= 0) {
	        if (cchar *cp ; (rs = uc_mallocstrw(obuf,rs,&cp)) >= 0) {
		    op->options = cp ;
	        }
	    } /* end if (svcentry_proc) */
	}
	return rs ;
}
/* end method (expander::comp_o) */

int expander::comp_prog() noex {
	int		rs = SR_OK ;
	if ((op->program == nullptr) && (argz != nullptr)) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(argz,-1,&cp)) > 0) {
	        if (cchar *pp ; (rs = uc_mallocstrw(cp,cl,&pp)) >= 0) {
		    op->program = pp ;
	        }
	    } /* end if (non-zero) */
	}
	return rs ;
}
/* end method (expander::comp_prog) */

int expander::comp_def() noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->f.srvargs) {
	    rs = vecstr_count(op->sap) ;
	    c = rs ;
	}
	if ((rs == 0) && (op->program != nullptr)) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfbasename(op->program,-1,&cp)) > 0) {
	        if (! op->f.srvargs) {
	            if ((rs = vecstr_start(op->sap,2,0)) >= 0) {
	                op->f.srvargs = true ;
		    }
	        }
	        if (op->f.srvargs) {
	            rs = vecstr_add(op->sap,cp,cl) ;
		    c = rs ;
		}
	    } /* end if (non-zero) */
	} /* end if (setting 'argv[0]') */
	return (rs >= 0) ? c : rs ;
}
/* end method (expander::comp_def) */

void expander::compfrees() noex {
	freeit(&op->options) ;
	freeit(&op->groupname) ;
	freeit(&op->username) ;
	if (op->f.srvargs) {
	    op->f.srvargs = false ;
	    vecstr_finish(op->sap) ;
	}
	freeit(&op->program) ;
}
/* end method (expander::compfrees) */

int svcentry_arrival(SE *op,time_t *tp) noex {
	int		rs ;
	if ((rs = svcentry_ctor(op)) >= 0) {
	    if (tp) *tp = op->atime ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_arrival) */

int svcentry_stime(SE *op,time_t daytime) noex {
	int		rs ;
	if ((rs = svcentry_ctor(op)) >= 0) {
	    op->stime = daytime ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_stime) */


/* private subroutines */

static int svcentry_starter(SE *op,svckey *skp,ARGS *esap) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    cint	olen = (rs * BUFMULT) ;
	    if (char *obuf ; (rs = uc_malloc((olen + 1),&obuf)) >= 0) {
	        if (skp->acc) {
	            cchar	*sp = skp->acc ;
	            if ((rs = svcentry_proc(op,sp,esap,obuf,olen)) >= 0) {
			cchar	*cp ;
	                if ((rs = uc_mallocstrw(obuf,rs,&cp)) >= 0) {
		            op->access = cp ;
		        }
	            }
		} /* end if (field-access) */
	        /* process the interval field */
	        if ((rs >= 0) && skp->interval) {
	            cchar	*sp = skp->interval ;
	            if ((rs = svcentry_proc(op,sp,esap,obuf,olen)) >= 0) {
		        if (int	v{} ; (rs = cfdecti(obuf,rs,&v)) >= 0) {
	    	            op->interval = v ;
		        } else {
		            op->interval = -1 ;
	    	            rs = SR_OK ;
		        }
	            } else {
	                op->interval = -1 ;
	            }
	            if (rs >= 0) {
	                op->magic = SVCENTRY_MAGIC ;
	            } else {
	                if (op->access) {
	                    uc_free(op->access) ;
	                    op->access = nullptr ;
	                }
	            } /* end if */
		} /* end if (field-interval) */
	        rs1 = uc_free(obuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (getbufsize) */
	return rs ;
}
/* end subroutie (svcentry_starter) */

static int svcentry_proc(SE *op,cc *inbuf,ARGS *esap,char *obuf,int olen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		elen = 0 ;
	if (inbuf) {
	    int		vlen = var.olen ;
	    if (char *vbuf ; (rs = uc_malloc((vlen + 1),&vbuf)) >= 0) {
		int	ibl = 0 ;
	        cchar	*ibp = inbuf ;
		if (op->ssp) {
	    	    if ((rs = varsub_exp(op->ssp,vbuf,vlen,inbuf,-1)) >= 0) {
	                ibl = rs ;
	                ibp = vbuf ;
	    	    }
		} else {
	    	    ibl = lenstr(ibp) ;
		}
		if (rs >= 0) {
	            if ((rs = args_expand(esap,obuf,olen,ibp,ibl)) >= 0) {
	                elen = rs ;
	            }
	        } /* end if (ok) */
		rs1 = uc_free(vbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? elen : rs ;
}
/* end subroutine (svcentry_proc) */

static int svcentry_mkfile(SE *op,cchar *tmpdname,int type) noex {
	int		rs ;
	int		rs1 ;
	if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkfile(tbuf,tmpdname,type)) >= 0) {
	        cint	tlen = rs ;
	        if (cchar *cp ; (rs = uc_mallocstrw(tbuf,tlen,&cp)) >= 0) {
		    switch (type) {
		    case 'o':
		        op->ofname = cp ;
		        break ;
		    case 'e':
		        op->efname = cp ;
		        break ;
		    } /* end switch */
	        } /* end if (malloc-str) */
	    } /* end if (mkfile) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (svcentry_mkfile) */

/* expand out a program argument with the substitution parameters */

/****
#	The following substitutions are made on command strings:
#
#		%V	Directory-Watcher version string
#		%R	program root
#		%N	machine nodename
#		%D	machine DNS domain
#		%H	machine hostname
#		%U	invoking username
#		%G	invoking groupname
#		%s	service
#		%i	interval (decimal seconds)
#
****/

static int args_expand(ARGS *esap,char *rbuf,int rlen,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (esap && rbuf && sp) {
	    cnullptr	np{} ;
	    rbuf[0] = '\0' ;
	    if ((sl != 0) && sp[0]) {
	        if (sl < 0) sl = lenstr(sp) ;
	        if (char *hbuf ; (rs = malloc_hn(&hbuf)) >= 0) {
		    cint	sch = '%' ;
		    cint	hlen = rs ;
		    int		bl = rlen ;
		    char	*bp = rbuf ;
		    for (cc *tp ; (tp = strnchr(sp,sl,sch)) != np ; ) {
			cint tl = intconv(tp - sp) ;
			if ((rs = snwcpy(bp,bl,sp,tl)) >= 0) {
	    		    cint	ch = mkchar(*tp) ;
		    	    int		cl = -1 ;
		    	    cchar	*cp = nullptr ;
			    bp += (rs + 1) ;
			    bl -= (rs + 1) ;
	                    switch (ch) {
			    case '%':
	                        cp = "%" ;
	                        cl = 1 ;
				break ;
	                    case 'V':
	                        cp = esap->version ;
	                        cl = lenstr(cp) ;
	                        break ;
	                    case 'R':
	                        cp = esap->programroot ;
	                        cl = lenstr(cp) ;
	                        break ;
	                    case 'N':
	                        cp = esap->nodename ;
	                        cl = lenstr(cp) ;
	                        break ;
	                    case 'D':
	                        cp = esap->domainname ;
	                        cl = lenstr(cp) ;
	                        break ;
	                    case 'H':
	                        cl = -1 ;
	                        if (esap->hostname == nullptr) {
			            cchar	*nn = esap->nodename ;
			            cchar	*dn = esap->domainname ;
	                            cp = hbuf ;
	                            rs = snsds(hbuf,hlen,nn,dn) ;
				    cl = rs ;
	                        } else {
	                            cp = esap->hostname ;
		                }
	                        if (cl < 0) sl = lenstr(cp) ;
	                        break ;
	                    case 'U':
	                        cp = esap->username ;
	                        cl = lenstr(cp) ;
	                        break ;
	                    case 'G':
	                        cp = esap->groupname ;
	                        cl = lenstr(cp) ;
	                        break ;
	                    case 's':
	                        cp = esap->service ;
	                        if (cp) cl = lenstr(cp) ;
	                        break ;
	                    case 'i':
	                        if (esap->interval != nullptr) {
	                            cp = esap->interval ;
	                            if (cp) cl = lenstr(cp) ;
	                        } else {
	                            cp = "1" ;
	                            cl = 1 ;
	                        }
	                        break ;
	                    default:
	                        cp = "¿" ;
	                        cl = 1 ;
		                break ;
	                    } /* end switch */
		            if ((rs >= 0) && (cl > 0)) {
		                if ((rs = snwcpy(bp,bl,cp,cl)) > 0) {
			            bp += rs ;
			            bl -= rs ;
		                }
		            }
		        } /* end if (snwcpy) */
		 	sl -= intconv((tp + 1) - sp) ;
			sp = (tp + 1) ;
		        if (rs < 0) break ;
		    } /* end while */
		    if ((rs >= 0) && (sl > 0)) {
			if ((rs = snwcpy(bp,bl,sp,sl)) > 0) {
			    bp += rs ;
			    bl -= rs ;
			}
		    } /* end if (remainder) */
	            *bp = '\0' ;
		    rl = intconv(bp - rbuf) ;
		    rs1 = uc_free(hbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (non-empty source c-string) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (args_expand) */

/* process an argument list */
static int vecstr_procargs(vecstr *alp,char *abuf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (alp && abuf) {
	    cint	alen = lenstr(abuf) ;
	    if (abuf[0]) {
	        cint	flen = alen ;
		if (char *fbuf ; (rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	            if (field fsb ; (rs = fsb.start(abuf,alen)) >= 0) {
			int	fl ;
	                while ((fl = fsb.sharg(pt.terms,fbuf,flen)) > 0) {
	                    c += 1 ;
	                    rs = vecstr_add(alp,fbuf,fl) ;
		            if (fsb.term == '#') break ;
	                    if (rs < 0) break ;
	                } /* end while */
	                rs1 = fsb.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (field) */
		    rs1 = uc_free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (non-empty arguments) */
	} /* end if (non-null) */
	return (rs >= 0) ? c: rs ;
}
/* end subroutine (processargs) */

/* make our little files for input and output of the server programs */
static int mkfile(char *obuf,cc *tmpdname,int type) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (obuf && tmpdname) {
	    if (char *pbuf ; (rs = malloc_mp(&pbuf)) >= 0) {
		if ((rs = mkpat(pbuf,rs,tmpdname,type)) >= 0) {
		    rs = mktmpfile(obuf,pbuf,0600) ;
		}
		rs1 = uc_free(pbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkfile) */

static int mkpat(char *pbuf,int plen,cc *tmpdname,int type) noex {
	int		rs ;
	int		rs1 ;
	if (sbuf b ; (rs = b.start(pbuf,plen)) >= 0) {
	    b.str(tmpdname) ;
	    b.chr('/') ;
	    {
		int	nu = 0 ;
		nu += b.str("pcspoll")  ;
		nu += b.chr(type) ;
		b.chrs('X',(tmplen - nu)) ;
	    }
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */
	return rs ;
}
/* end subroutine (mkpat) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    var.olen = (rs * BUFMULT) ;
	}
	return rs ;
}
/* end subroutine (mkvars) */

static void delfreeit(cchar *&fn) noex {
	if (fn) {
	    if (fn[0]) uc_unlink(fn) ;
	    freeit(&fn) ;
	}
}
/* end subroutine (delfreeit) */

static void freeit(cchar **pp) noex {
	if (*pp != nullptr) {
	    uc_free(*pp) ;
	    *pp = nullptr ;
	}
}
/* end subroutine (freeit) */


