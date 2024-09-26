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
	This little object is used to create a program entry and
	to populate aspects of it with different operations on the
	object.  This object is used in "server" types of programs.
	This object is usually created from elements taken from the
	parsing of a server file.

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
	was younger than at least twenty-five (25) years ago.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |pid_t| */
#include	<sys/param.h>
#include	<unistd.h>
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<varsub.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<fieldterminit.hh>
#include	<sbuf.h>
#include	<svcfile.h>
#include	<snx.h>
#include	<sfx.h>
#include	<mktmp.h>
#include	<strwcpy.h>
#include	<cfdec.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"svcentry.h"
#include	"svckey.h"


/* local defines */

#undef	OUTBUFLEN
#define	OUTBUFLEN	(10 * MAXPATHLEN)

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

static int	svcentry_process(SVCENTRY *,cchar *,ARGS *,char *,int) noex ;
static int	svcentry_mkfile(SVCENTRY *,cchar *,int) noex ;

static int	args_expand(ARGS *,char *,int,cchar *,int) noex ;
static int	vecstr_procargs(vecstr *,char *) noex ;
static int	mkfile(char *,cchar *,int) noex ;

static void	freeit(cchar **) noex ;


/* external variables */


/* local variables */

constexpr cchar		xes[] = "XXXXXXXXXXXXXX" ;

constexpr fieldterminit	pt(" \t") ;


/* exported variables */


/* exported subroutines */

int svcentry_start(svcentry *op,varsub *ssp,ENT *sep,ARGS * esap) noex {
	svckey		sk ;
	cint		outlen = OUTBUFLEN ;
	int		rs = SR_OK ;
	int		sl ;
	char		outbuf[OUTBUFLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (sep == nullptr) return SR_FAULT ;
	op->name[0] = '\0' ;
	op->jobid[0] = '\0' ;

/* store what we will need later! */

	op->ssp = ssp ;
	op->atime = esap->daytime ;	/* job arrival time */

/* process the service-entry in a way that makes it how we want it */

	svckey_load(&sk,sep) ;

/* load the service name */

	strwcpy(op->name,sk.svc,MAXNAMELEN) ;

/* process the access field */

	if ((rs >= 0) && (sk.acc != nullptr)) {

	    rs = svcentry_process(op,sk.acc,esap,outbuf,outlen) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad0 ;

	    rs = uc_mallocstrw(outbuf,sl,&op->access) ;
	    if (rs < 0)
	        goto bad0 ;

	} /* end if (access field) */

/* process the interval field */

	if ((rs >= 0) && (sk.interval != nullptr)) {

	    rs = svcentry_process(op,sk.interval,esap,outbuf,outlen) ;
	    if (rs < 0)
	        goto bad1 ;

/* convert the interval string to an integer */

	    rs = cfdecti(outbuf,rs,&op->interval) ;

	    op->interval = (rs >= 0) ? op->interval : -1 ;
	    rs = SR_OK ;

	} else {
	    op->interval = -1 ;
	}

	if (rs >= 0)
	    op->magic = SVCENTRY_MAGIC ;

ret0:
	return rs ;

/* bad stuff happened */
bad1:
	if (op->access != nullptr) {
	    uc_free(op->access) ;
	    op->access = nullptr ;
	}

bad0:
	goto ret0 ;
}
/* end subroutine (svcentry_start) */

int svcentry_getargs(svcentry *op,mainv avp) noex {
	int		rs ;
	if ((rs = svcentry_magic(op,avp)) >= 0) {
	    rs = vecstr_getvec(op->sap,&avp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getargs) */

int svcentry_finish(svcentry *op) noex {
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

int svcentry_getaccess(svcentry *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = svcentry_ctor(op)) >= 0) {
	    if (rpp) *rpp = op->access ;
	    rs = (op->access) ? SR_OK : SR_EMPTY ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getaccess) */

/* retrieve for caller the execution interval for this entry */
int svcentry_getinterval(svcentry *op,int *rp) noex {
	int		rs ;
	if ((rs = svcentry_magic(op)) >= 0) {
	    if (rp) {
		*rp = op->interval ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getinterval) */

int svcentry_getargs(svcentry *op,mainv *avp) noex {
	int		rs ;
	if ((rs = svcentry_magic(op,avp)) >= 0) {
	    rs = vecstr_getvec(op->sap,avp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getargs) */

int svcentry_expand(svcentry *op,ENT *sep,ARGS *esap) noex {
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
	char		outbuf[OUTBUFLEN + 1] ;

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

	tmpdname = (esap->tmpdname) ? esap->tmpdname : SVCENTRY_TMPDIR ;

/* make some temporary files for program file input and output */

	rs = svcentry_mkfile(op,tmpdname,'o') ;
	if (rs < 0) goto bad0 ;

	rs = svcentry_mkfile(op,tmpdname,'e') ;
	if (rs < 0) goto bad1 ;

/* process them */

	if (sk.p != nullptr) {

	    rs = svcentry_process(op,sk.p,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad2 ;

	    if ((cl = sfshrink(outbuf,sl,&cp)) > 0) {
	        if ((rs = uc_mallocstrw(cp,cl,&ccp)) >= 0) {
		    op->program = ccp ;
		}
	    } else {
		rs = SR_INVALID ;
	    }
	    if (rs < 0) goto bad2 ;

	} /* end if (program path) */

	argz = nullptr ;
	if (sk.a != nullptr) {

	    rs = svcentry_process(op,sk.a,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad3 ;

	    opts = VECSTR_OCOMPACT ;
	    rs = vecstr_start(op->sap,6,opts) ;
	    if (rs < 0)
	        goto bad3 ;

	    op->f.srvargs = true ;
	    if ((rs = vecstr_procargs(op->sap,outbuf)) > 0) {

	        rs1 = vecstr_get(op->sap,0,&argz) ;
	        if (rs1 < 0)
	            argz = nullptr ;

	    } /* end if */

	} /* end if (program arguments) */

	if (rs < 0) goto bad4 ;

	if (sk.u != nullptr) {

	    rs = svcentry_process(op,sk.u,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad4 ;

	    if ((rs = uc_mallocstrw(outbuf,sl,&ccp)) >= 0) {
		op->username = ccp ;
	    }
	    if (rs < 0)
	        goto bad4 ;

	} /* end if (username field) */

	if (sk.g != nullptr) {

	    rs = svcentry_process(op,sk.g,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad5 ;

	    if ((rs = uc_mallocstrw(outbuf,sl,&ccp)) >= 0) {
		op->groupname = ccp ;
	    }
	    if (rs < 0)
	        goto bad5 ;

	}

	if (sk.opts != nullptr) {

	    rs = svcentry_process(op,sk.opts,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad6 ;

	    if ((rs = uc_mallocstrw(outbuf,sl,&ccp)) >= 0) {
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

int svcentry_arrival(svcentry *op,time_t *tp) noex {
	int		rs ;
	if ((rs = svcentry_ctor(op)) >= 0) {
	    if (tp) *tp = op->atime ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_arrival) */

int svcentry_stime(svcentry *op,time_t daytime) noex {
	int		rs ;
	if ((rs = svcentry_ctor(op)) >= 0) {
	    op->stime = daytime ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_stime) */


/* private subroutines */

static int svcentry_process(svcentry *op,cc *inbuf,ARGS *esap,
		char *outbuf,int outlen) noex {
	int		rs = SR_OK ;
	int		vlen ;
	int		elen = 0 ;
	cchar		*ibp ;
	char		vbuf[OUTBUFLEN + 1] ;

	if (inbuf == nullptr) return SR_FAULT ;

	ibp = inbuf ;
	if (op->ssp != nullptr) {

	    rs = varsub_exp(op->ssp,vbuf,OUTBUFLEN,inbuf,-1) ;
	    vlen = rs ;
	    if (rs >= 0)
	        ibp = vbuf ;

	} else {
	    vlen = strlen(ibp) ;
	}

	if (rs >= 0) {
	    if ((rs = args_expand(esap,outbuf,outlen,ibp,vlen)) >= 0) {
	        elen = rs ;
	    } else {
	        rs = SR_TOOBIG ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? elen : rs ;
}
/* end subroutine (svcentry_process) */

static int svcentry_mkfile(SVCENTRY *op,cchar *tmpdname,int type) noex {
	int		rs ;
	int		rs1 ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkfile(tbuf,tmpdname,type)) >= 0) {
	        cint	fl = rs ;
	        cchar	*cp ;
	        if ((rs = uc_mallocstrw(tbuf,fl,&cp)) >= 0) {
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
	int		elen = 0 ;
	if (esap && rbuf && sp) {
	    rbuf[0] = '\0' ;
	    if ((sl == 0) || sp[0]) {
	        char	*hbuf{} ;
	        if (sl < 0) sl = strlen(sp) ;
	        if ((rs = malloc_hn(&hbuf)) >= 0) {
		    cint	hlen = rs ;

	int		cl ;
	cchar		*cp ;
	char		*rbp = rbuf ;
	while ((sl > 0) && (elen < rlen)) {
	    int		ch = mkchar(*sp) ;
	    switch (ch) {

	    case '%':
	        sp += 1 ;
	        sl -= 1 ;
	        if (sl == 0)
	            return elen ;

	        cl = 0 ;
		ch = mkchar(*sp) ;
	        switch (ch) {
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
	                cl = snsds(hbuf,hlen,nn,dn) ;
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
	            cp = sp ;
	            cl = 1 ;
		    break ;
	        } /* end switch */
	        sp += 1 ;
	        sl -= 1 ;
	        if ((elen + sl) > rlen)
	            return BAD ;

	        strncpy(rbp,cp,cl) ;
	        rbp += cl ;
	        elen += cl ;
	        break ;

	    default:
	        *rbp++ = *sp++ ;
	        elen += 1 ;
	        sl -= 1 ;
		break ;

	    } /* end switch */

	} /* end while */

	rbuf[elen] = '\0' ;

		    rs1 = uc_free(hbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (non-empty source c-string) */
	} /* end if (non-null) */
	return (rs >= 0) ? elen : rs ;
}
/* end subroutine (args_expand) */

static void freeit(cchar **pp) noex {
	if (*pp != nullptr) {
	    uc_free(*pp) ;
	    *pp = nullptr ;
	}
}
/* end subroutine (freeit) */

/* process an argument list */
static int vecstr_procargs(vecstr *alp,char *abuf) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		i = 0 ;
	if (alp && abuf) {
	    cint	alen = strlen(abuf) ;
	    if (abuf[0]) {
	        cint	flen = alen ;
	        char	*fbuf ;
		if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
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
static int mkfile(char *outbuf,cc *tmpdname,int type) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (outbuf && tmpdname) {
	    char	*pbuf{} ;
	    if ((rs = malloc_mp(&pbuf)) >= 0) {
		sbuf	b ;
		cint	plen = rs ;
		if ((rs = b.start(pbuf,plen)) >= 0) {
	            b.strw(tmpdname,-1) ;
	    	    b.chr('/') ;
	            {
		        int	nu = 0 ;
	                nu += b.strw("pcspoll",-1) ;
	                nu += b.chr(type) ;
	                b.strw(xes,(14 - nu)) ;
	            }
	            rs1 = b.finish ;
	    	    if (rs >= 0) rs = rs1 ;
		} /* end if (sbuf) */
	        if (rs >= 0) {
		    rs = mktmpfile(outbuf,pbuf,0600) ;
		}
		rs1 = uc_free(pbuf) ;
		if (rs >= 0) rs = rs1 ;
	    }  /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkfile) */


