/* svcentry SUPPORT */
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
#include	<cstring>		/* |strlen(3c)| */
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


/* local defines */

#define	TMPLEN		14		/* old SysV file-len */
#define	BUFMULT		10		/* expansion buffer path multiplier */

#undef	OUTBUFLEN
#define	OUTBUFLEN	(10 * MAXPATHLEN)

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
	            svckey	sk ;
		    cint	namelen = rs ;
		    op->name = bp ;
	            op->ssp = ssp ;
	            op->atime = esap->daytime ;	/* job arrival time */
		    op->jobid[0] = '\0' ;
		    if ((rs = svckey_load(&sk,sep)) >= 0) {
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

#ifdef	COMMENT
int svcentry_expand(SE *op,ENT *sep,ARGS *esap) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = svcentry_magic(op,sep,esap)) >= 0) {
	    svckey	sk ;
	    if ((rs = svckey_load(&sk,sep)) >= 0) {
		cint		olen = var.olen ;
		cchar		*oldservice = esap->service ;
		cchar		*oldinterval = esap->interval ;
		esap->service = sk.svc ;
		esap->interval = sk.interval ;
		char		*obuf{} ;
		if ((rs = uc_malloc((olen + 1),&obuf)) >= 0) {
	int		sl, cl ;
	int		opts ;
	cchar		*argz ;
	cchar		*tmpdname ;
	cchar		*ccp ;
	cchar		*cp ;
	/* load the job ID if one was supplied */
	if (esap->jobid) {
	    strwcpy(op->jobid,esap->jobid,SVCENTRY_IDLEN) ;
	}
	/* did they supply a TMPDIR? */
	tmpdname = (esap->tmpdname) ? esap->tmpdname : deftmpdname ;
	/* make some temporary files for program file input and output */
	rs = svcentry_mkfile(op,tmpdname,'o') ;
	if (rs < 0) goto bad0 ;

	rs = svcentry_mkfile(op,tmpdname,'e') ;
	if (rs < 0) goto bad1 ;

/* process them */

	if (sk.p != nullptr) {

	    rs = svcentry_proc(op,sk.p,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad2 ;

	    if ((cl = sfshrink(obuf,sl,&cp)) > 0) {
	        if (cchar *ap{} ; (rs = uc_mallocstrw(cp,cl,&ap)) >= 0) {
		    op->program = ap ;
		}
	    } else {
		rs = SR_INVALID ;
	    }
	    if (rs < 0) goto bad2 ;

	} /* end if (program path) */

	argz = nullptr ;
	if (sk.a != nullptr) {

	    rs = svcentry_proc(op,sk.a,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad3 ;

	    opts = VECSTR_OCOMPACT ;
	    rs = vecstr_start(op->sap,6,opts) ;
	    if (rs < 0)
	        goto bad3 ;

	    op->f.srvargs = true ;
	    if ((rs = vecstr_procargs(op->sap,obuf)) > 0) {

	        rs1 = vecstr_get(op->sap,0,&argz) ;
	        if (rs1 < 0)
	            argz = nullptr ;

	    } /* end if */

	} /* end if (program arguments) */

	if (rs < 0) goto bad4 ;

	if (sk.u != nullptr) {

	    rs = svcentry_proc(op,sk.u,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad4 ;

	    if ((rs = uc_mallocstrw(obuf,sl,&ccp)) >= 0) {
		op->username = ccp ;
	    }
	    if (rs < 0)
	        goto bad4 ;

	} /* end if (username field) */

	if (sk.g != nullptr) {

	    rs = svcentry_proc(op,sk.g,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad5 ;

	    if ((rs = uc_mallocstrw(obuf,sl,&ccp)) >= 0) {
		op->groupname = ccp ;
	    }
	    if (rs < 0)
	        goto bad5 ;

	}

	if (sk.opts != nullptr) {

	    rs = svcentry_proc(op,sk.opts,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad6 ;

	    if ((rs = uc_mallocstrw(obuf,sl,&ccp)) >= 0) {
		op->options = ccp ;
	    }
	    if (rs < 0)
	        goto bad6 ;

	}

/* OK, perform some fixups */

	if ((op->program == nullptr) && (argz != nullptr)) {

	    cl = sfshrink(argz,-1,&cp) ;

	    if ((rs = uc_mallocstrw(cp,cl,&ccp)) >= 0) {
		op->program = ccp ;
	    }
	    if (rs < 0)
	        goto bad7 ;

	}

/* are we OK for a go? */

	if (op->program == nullptr)
	    goto bad7 ;

/* set at least one program argument if we have none so far */

	rs = SR_OK ;
	if (op->f.srvargs) {
	    rs = vecstr_count(op->sap) ;
	}

	if ((rs == 0) && (op->program != nullptr)) {

	    if (sfbasename(op->program,-1,&cp) > 0) {

	        if (! op->f.srvargs) {

	            rs = vecstr_start(op->sap,2,0) ;
	            if (rs >= 0)
	                op->f.srvargs = true ;

	        }

	        if (op->f.srvargs)
	            rs = vecstr_add(op->sap,cp,-1) ;

	    }

	} /* end if (setting 'argv[0]') */

/* we're out of here */

	if (rs >= 0)
	    goto retok ;

/* bad things */
bad7:
	freeit(&op->options) ;

bad6:
	freeit(&op->groupname) ;

bad5:
	freeit(&op->username) ;

bad4:
	if (op->f.srvargs) {
	    op->f.srvargs = false ;
	    vecstr_finish(op->sap) ;
	}

bad3:
	if (op->program) {
	    freeit(&op->program) ;
	}

bad2:
	if ((op->efname != nullptr) && (op->efname[0] != '\0')) {
	    uc_unlink(op->efname) ;
	}

bad1:
	if ((op->ofname != nullptr) && (op->ofname[0] != '\0')) {
	    uc_unlink(op->ofname) ;
	}

bad0:
retok:
	esap->interval = oldinterval ;
	esap->service = oldservice ;

		    rs1 = uc_free(obuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (svckey_load) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_expand) */
#else /* COMMENT */
int svcentry_expand(SE *op,ENT *sep,ARGS *esap) noex {
	svckey		sk ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl, cl ;
	int		opts ;
	cchar		*oldservice ;
	cchar		*oldinterval ;
	cchar		*argz ;
	cchar		*tmpdname ;
	cchar		*ccp ;
	cchar		*cp ;
	char		obuf[OUTBUFLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SVCENTRY_MAGIC) return SR_NOTOPEN ;

	oldservice = esap->service ;
	oldinterval = esap->interval ;

	svckey_load(&sk,sep) ;

	esap->service = sk.svc ;
	esap->interval = sk.interval ;

/* load the job ID if one was supplied */

	if (esap->jobid != nullptr)
	    strwcpy(op->jobid,esap->jobid,SVCENTRY_IDLEN) ;

/* did they supply a TMPDIR? */

	tmpdname = (esap->tmpdname) ? esap->tmpdname : deftmpdname ;

/* make some temporary files for program file input and output */

	rs = svcentry_mkfile(op,tmpdname,'o') ;
	if (rs < 0) goto bad0 ;

	rs = svcentry_mkfile(op,tmpdname,'e') ;
	if (rs < 0) goto bad1 ;

/* process them */

	if (sk.p != nullptr) {

	    rs = svcentry_proc(op,sk.p,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad2 ;

	    if ((cl = sfshrink(obuf,sl,&cp)) > 0) {
	        if (cchar *ap{} ; (rs = uc_mallocstrw(cp,cl,&ap)) >= 0) {
		    op->program = ap ;
		}
	    } else {
		rs = SR_INVALID ;
	    }
	    if (rs < 0) goto bad2 ;

	} /* end if (program path) */

	argz = nullptr ;
	if (sk.a != nullptr) {

	    rs = svcentry_proc(op,sk.a,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad3 ;

	    opts = VECSTR_OCOMPACT ;
	    rs = vecstr_start(op->sap,6,opts) ;
	    if (rs < 0)
	        goto bad3 ;

	    op->f.srvargs = true ;
	    if ((rs = vecstr_procargs(op->sap,obuf)) > 0) {

	        rs1 = vecstr_get(op->sap,0,&argz) ;
	        if (rs1 < 0)
	            argz = nullptr ;

	    } /* end if */

	} /* end if (program arguments) */

	if (rs < 0) goto bad4 ;

	if (sk.u != nullptr) {

	    rs = svcentry_proc(op,sk.u,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad4 ;

	    if ((rs = uc_mallocstrw(obuf,sl,&ccp)) >= 0) {
		op->username = ccp ;
	    }
	    if (rs < 0)
	        goto bad4 ;

	} /* end if (username field) */

	if (sk.g != nullptr) {

	    rs = svcentry_proc(op,sk.g,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad5 ;

	    if ((rs = uc_mallocstrw(obuf,sl,&ccp)) >= 0) {
		op->groupname = ccp ;
	    }
	    if (rs < 0)
	        goto bad5 ;

	}

	if (sk.opts != nullptr) {

	    rs = svcentry_proc(op,sk.opts,esap,obuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad6 ;

	    if ((rs = uc_mallocstrw(obuf,sl,&ccp)) >= 0) {
		op->options = ccp ;
	    }
	    if (rs < 0)
	        goto bad6 ;

	}

/* OK, perform some fixups */

	if ((op->program == nullptr) && (argz != nullptr)) {

	    cl = sfshrink(argz,-1,&cp) ;

	    if ((rs = uc_mallocstrw(cp,cl,&ccp)) >= 0) {
		op->program = ccp ;
	    }
	    if (rs < 0)
	        goto bad7 ;

	}

/* are we OK for a go? */

	if (op->program == nullptr)
	    goto bad7 ;

/* set at least one program argument if we have none so far */

	rs = SR_OK ;
	if (op->f.srvargs) {
	    rs = vecstr_count(op->sap) ;
	}

	if ((rs == 0) && (op->program != nullptr)) {

	    if (sfbasename(op->program,-1,&cp) > 0) {

	        if (! op->f.srvargs) {

	            rs = vecstr_start(op->sap,2,0) ;
	            if (rs >= 0)
	                op->f.srvargs = true ;

	        }

	        if (op->f.srvargs)
	            rs = vecstr_add(op->sap,cp,-1) ;

	    }

	} /* end if (setting 'argv[0]') */

/* we're out of here */

	if (rs >= 0)
	    goto retok ;

/* bad things */
bad7:
	freeit(&op->options) ;

bad6:
	freeit(&op->groupname) ;

bad5:
	freeit(&op->username) ;

bad4:
	if (op->f.srvargs) {
	    op->f.srvargs = false ;
	    vecstr_finish(op->sap) ;
	}

bad3:
	if (op->program != nullptr)
	    freeit(&op->program) ;

bad2:
	if ((op->efname != nullptr) && (op->efname[0] != '\0')) {
	    uc_unlink(op->efname) ;
	}

bad1:
	if ((op->ofname != nullptr) && (op->ofname[0] != '\0')) {
	    uc_unlink(op->ofname) ;
	}

bad0:
retok:
	esap->interval = oldinterval ;
	esap->service = oldservice ;

	return rs ;
}
/* end subroutine (svcentry_expand) */
#endif /* COMMENT */

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
	    if (char *obuf{} ; (rs = uc_malloc((olen + 1),&obuf)) >= 0) {
	        if (skp->acc) {
	            cchar	*sp = skp->acc ;
	            if ((rs = svcentry_proc(op,sp,esap,obuf,olen)) >= 0) {
			cchar	*cp{} ;
	                if ((rs = uc_mallocstrw(obuf,rs,&cp)) >= 0) {
		            op->access = cp ;
		        }
	            }
		} /* end if (field-access) */
	        /* process the interval field */
	        if ((rs >= 0) && skp->interval) {
	            cchar	*sp = skp->interval ;
	            if ((rs = svcentry_proc(op,sp,esap,obuf,olen)) >= 0) {
		        int	v{} ;
	                if ((rs = cfdecti(obuf,rs,&v)) >= 0) {
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
	    if (char *vbuf{} ; (rs = uc_malloc((vlen + 1),&vbuf)) >= 0) {
		int	ibl = 0 ;
	        cchar	*ibp = inbuf ;
		if (op->ssp) {
	    	    if ((rs = varsub_exp(op->ssp,vbuf,vlen,inbuf,-1)) >= 0) {
	                ibl = rs ;
	                ibp = vbuf ;
	    	    }
		} else {
	    	    ibl = strlen(ibp) ;
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
	if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkfile(tbuf,tmpdname,type)) >= 0) {
	        cint	tlen = rs ;
	        if (cchar *cp{} ; (rs = uc_mallocstrw(tbuf,tlen,&cp)) >= 0) {
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
	        if (sl < 0) sl = strlen(sp) ;
	        if (char *hbuf{} ; (rs = malloc_hn(&hbuf)) >= 0) {
		    cint	sch = '%' ;
		    cint	hlen = rs ;
		    int		bl = rlen ;
		    cchar	*tp ;
		    char	*bp = rbuf ;
		    while ((tp = strnchr(sp,sl,sch)) != np) {
			if ((rs = snwcpy(bp,bl,sp,(tp-sp))) >= 0) {
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
	                        cl = strlen(cp) ;
	                        break ;
	                    case 'R':
	                        cp = esap->programroot ;
	                        cl = strlen(cp) ;
	                        break ;
	                    case 'N':
	                        cp = esap->nodename ;
	                        cl = strlen(cp) ;
	                        break ;
	                    case 'D':
	                        cp = esap->domainname ;
	                        cl = strlen(cp) ;
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
	                        if (cl < 0) sl = strlen(cp) ;
	                        break ;
	                    case 'U':
	                        cp = esap->username ;
	                        cl = strlen(cp) ;
	                        break ;
	                    case 'G':
	                        cp = esap->groupname ;
	                        cl = strlen(cp) ;
	                        break ;
	                    case 's':
	                        cp = esap->service ;
	                        if (cp) cl = strlen(cp) ;
	                        break ;
	                    case 'i':
	                        if (esap->interval != nullptr) {
	                            cp = esap->interval ;
	                            if (cp) cl = strlen(cp) ;
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
		 	sl -= ((tp + 1) - sp) ;
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
		    rl = (bp - rbuf) ;
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
	int		i = 0 ;
	if (alp && abuf) {
	    cint	alen = strlen(abuf) ;
	    if (abuf[0]) {
	        cint	flen = alen ;
		if (char *fbuf{} ; (rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	            field	fsb ;
	            if ((rs = fsb.start(abuf,alen)) >= 0) {
			int	fl ;
	                while ((fl = fsb.sharg(pt.terms,fbuf,flen)) > 0) {
	                    i += 1 ;
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
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (processargs) */

/* make our little files for input and output of the server programs */
static int mkfile(char *obuf,cc *tmpdname,int type) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (obuf && tmpdname) {
	    if (char *pbuf{} ; (rs = malloc_mp(&pbuf)) >= 0) {
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
	sbuf		b ;
	int		rs ;
	int		rs1 ;
	if ((rs = b.start(pbuf,plen)) >= 0) {
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

static void freeit(cchar **pp) noex {
	if (*pp != nullptr) {
	    uc_free(*pp) ;
	    *pp = nullptr ;
	}
}
/* end subroutine (freeit) */


