/* svcentry SUPPORT */
/* lang=C++20 */

/* build up a program entry piece-meal as it were */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little object is used to create a program entry and
	to populate aspects of it with different operations on the
	object.  This object is used in "server" types of programs.
	This object is usually created from elements taken from the
	parsing of a server file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<varsub.h>
#include	<field.h>
#include	<fieldterms.h>
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

#define	STEBUFLEN	(2 * MAXPATHLEN)

#undef	OUTBUFLEN
#define	OUTBUFLEN	(10 * MAXPATHLEN)

#define	FBUFLEN		(2 * MAXPATHLEN)

#define	ARGS		svcentry_args
#define	ENT		svcfile_ent


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int svcentry_ctor(svcentry *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    SVCENTRY	*hep = (SVCENTRY *) op ;
	    cnullptr	np{} ;
	    memclear(hep) ;
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

static int	svcentry_process(SVCENTRY *,cchar *,ARGS *,
			char *,int) noex ;
static int	svcentry_mkfile(SVCENTRY *,cchar *,int) noex ;

static int	expand(ARGS *,cchar *,int,char *,int) noex ;
static int	vecstr_procargs(vecstr *,char *) noex ;
static int	mkfile(char *,cchar *,int) noex ;

static void	freeit(cchar **) noex ;


/* external variables */


/* local variables */

constexpr cchar		xes[] = "XXXXXXXXXXXXXX" ;


/* exported variables */


/* exported subroutines */

int svcentry_start(svcentry *pep,varsub *ssp,ENT *sep,ARGS * esap) noex {
	svckey		sk ;
	cint		outlen = OUTBUFLEN ;
	int		rs = SR_OK ;
	int		sl ;
	char		outbuf[OUTBUFLEN + 1] ;

	if (pep == nullptr) return SR_FAULT ;
	if (sep == nullptr) return SR_FAULT ;
	pep->magic = 0 ;
	pep->pid = 0 ;
	pep->interval = 0 ;

	pep->program = nullptr ;
	pep->username = nullptr ;
	pep->groupname = nullptr ;
	pep->options = nullptr ;
	pep->access = nullptr ;
	pep->efname = nullptr ;
	pep->ofname = nullptr ;

	pep->f = {} ;
	pep->name[0] = '\0' ;
	pep->jobid[0] = '\0' ;

/* store what we will need later! */

	pep->ssp = ssp ;
	pep->atime = esap->daytime ;	/* job arrival time */

/* process the service-entry in a way that makes it how we want it */

	svckey_load(&sk,sep) ;

/* load the service name */

	strwcpy(pep->name,sk.svc,MAXNAMELEN) ;

/* process the access field */

	if ((rs >= 0) && (sk.acc != nullptr)) {

	    rs = svcentry_process(pep,sk.acc,esap,outbuf,outlen) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad0 ;

	    rs = uc_mallocstrw(outbuf,sl,&pep->access) ;
	    if (rs < 0)
	        goto bad0 ;

	} /* end if (access field) */

/* process the interval field */

	if ((rs >= 0) && (sk.interval != nullptr)) {

	    rs = svcentry_process(pep,sk.interval,esap,outbuf,outlen) ;
	    if (rs < 0)
	        goto bad1 ;

/* convert the interval string to an integer */

	    rs = cfdecti(outbuf,rs,&pep->interval) ;

	    pep->interval = (rs >= 0) ? pep->interval : -1 ;
	    rs = SR_OK ;

	} else {
	    pep->interval = -1 ;
	}

	if (rs >= 0)
	    pep->magic = SVCENTRY_MAGIC ;

ret0:
	return rs ;

/* bad stuff happened */
bad1:
	if (pep->access != nullptr) {
	    uc_free(pep->access) ;
	    pep->access = nullptr ;
	}

bad0:
	goto ret0 ;
}
/* end subroutine (svcentry_start) */

int svcentry_getaccess(svcentry *pep,cchar **rpp) noex {
	int		rs ;
	if ((rs = svcentry_ctor(pep)) >= 0) {
	    if (rpp) *rpp = pep->access ;
	    rs = (pep->access) ? SR_OK : SR_EMPTY ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getaccess) */

/* retrieve for caller the execution interval for this entry */
int svcentry_getinterval(svcentry *pep,int *rp) noex {
	int		rs ;
	if ((rs = svcentry_ctor(pep)) >= 0) {
	    if (rp) *rp = pep->interval ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getinterval) */

int svcentry_getargs(svcentry *pep,mainv *avp) noex {
	int		rs ;
	if ((rs = svcentry_ctor(pep,avp)) >= 0) {
	    rs = vecstr_getvec(pep->sap,avp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_getargs) */

int svcentry_finish(svcentry *pep) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = svcentry_ctor(pep)) >= 0) {
	    if (pep->ofname != nullptr) {
	        if (pep->ofname[0] != '\0') u_unlink(pep->ofname) ;
	        rs1 = uc_free(pep->ofname) ;
	        if (rs >= 0) rs = rs1 ;
	        pep->ofname = nullptr ;
	    }
	    if (pep->efname != nullptr) {
	        if (pep->efname[0] != '\0') u_unlink(pep->efname) ;
	        rs1 = uc_free(pep->efname) ;
	        if (rs >= 0) rs = rs1 ;
	        pep->efname = nullptr ;
	    }
	    if (pep->f.srvargs) {
	        pep->f.srvargs = false ;
	        rs1 = vecstr_finish(pep->sap) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pep->program != nullptr) {
	        rs1 = uc_free(pep->program) ;
	        if (rs >= 0) rs = rs1 ;
	        pep->program = nullptr ;
	    }
	    if (pep->username != nullptr) {
	        rs1 = uc_free(pep->username) ;
	        if (rs >= 0) rs = rs1 ;
	        pep->username = nullptr ;
	    }
	    if (pep->groupname != nullptr) {
	        rs1 = uc_free(pep->groupname) ;
	        if (rs >= 0) rs = rs1 ;
	        pep->groupname = nullptr ;
	    }
	    if (pep->options != nullptr) {
	        rs1 = uc_free(pep->options) ;
	        if (rs >= 0) rs = rs1 ;
	        pep->options = nullptr ;
	    }
	    if (pep->access != nullptr) {
	        rs1 = uc_free(pep->access) ;
	        if (rs >= 0) rs = rs1 ;
	        pep->access = nullptr ;
	    }
	    pep->program = nullptr ;
	    pep->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_finish) */

int svcentry_expand(svcentry *pep,ENT *sep,ARGS *esap) noex {
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

	if (pep == nullptr) return SR_FAULT ;

	if (pep->magic != SVCENTRY_MAGIC) return SR_NOTOPEN ;

	oldservice = esap->service ;
	oldinterval = esap->interval ;

	svckey_load(&sk,sep) ;

	esap->service = sk.svc ;
	esap->interval = sk.interval ;

/* load the job ID if one was supplied */

	if (esap->jobid != nullptr)
	    strwcpy(pep->jobid,esap->jobid,SVCENTRY_IDLEN) ;

/* did they supply a TMPDIR? */

	tmpdname = (esap->tmpdname) ? esap->tmpdname : SVCENTRY_TMPDIR ;

/* make some temporary files for program file input and output */

	rs = svcentry_mkfile(pep,tmpdname,'o') ;
	if (rs < 0) goto bad0 ;

	rs = svcentry_mkfile(pep,tmpdname,'e') ;
	if (rs < 0) goto bad1 ;

/* process them */

	if (sk.p != nullptr) {

	    rs = svcentry_process(pep,sk.p,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad2 ;

	    if ((cl = sfshrink(outbuf,sl,&cp)) > 0) {
	        if ((rs = uc_mallocstrw(cp,cl,&ccp)) >= 0) {
		    pep->program = ccp ;
		}
	    } else {
		rs = SR_INVALID ;
	    }
	    if (rs < 0) goto bad2 ;

	} /* end if (program path) */

	argz = nullptr ;
	if (sk.a != nullptr) {

	    rs = svcentry_process(pep,sk.a,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad3 ;

	    opts = VECSTR_OCOMPACT ;
	    rs = vecstr_start(pep->sap,6,opts) ;
	    if (rs < 0)
	        goto bad3 ;

	    pep->f.srvargs = true ;
	    if ((rs = vecstr_procargs(pep->sap,outbuf)) > 0) {

	        rs1 = vecstr_get(pep->sap,0,&argz) ;
	        if (rs1 < 0)
	            argz = nullptr ;

	    } /* end if */

	} /* end if (program arguments) */

	if (rs < 0) goto bad4 ;

	if (sk.u != nullptr) {

	    rs = svcentry_process(pep,sk.u,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad4 ;

	    if ((rs = uc_mallocstrw(outbuf,sl,&ccp)) >= 0) {
		pep->username = ccp ;
	    }
	    if (rs < 0)
	        goto bad4 ;

	} /* end if (username field) */

	if (sk.g != nullptr) {

	    rs = svcentry_process(pep,sk.g,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad5 ;

	    if ((rs = uc_mallocstrw(outbuf,sl,&ccp)) >= 0) {
		pep->groupname = ccp ;
	    }
	    if (rs < 0)
	        goto bad5 ;

	}

	if (sk.opts != nullptr) {

	    rs = svcentry_process(pep,sk.opts,esap,outbuf,OUTBUFLEN) ;
	    sl = rs ;
	    if (rs < 0)
	        goto bad6 ;

	    if ((rs = uc_mallocstrw(outbuf,sl,&ccp)) >= 0) {
		pep->options = ccp ;
	    }
	    if (rs < 0)
	        goto bad6 ;

	}

/* OK, perform some fixups */

	if ((pep->program == nullptr) && (argz != nullptr)) {

	    cl = sfshrink(argz,-1,&cp) ;

	    if ((rs = uc_mallocstrw(cp,cl,&ccp)) >= 0) {
		pep->program = ccp ;
	    }
	    if (rs < 0)
	        goto bad7 ;

	}

/* are we OK for a go? */

	if (pep->program == nullptr)
	    goto bad7 ;

/* set at least one program argument if we have none so far */

	rs = SR_OK ;
	if (pep->f.srvargs) {
	    rs = vecstr_count(pep->sap) ;
	}

	if ((rs == 0) && (pep->program != nullptr)) {

	    if (sfbasename(pep->program,-1,&cp) > 0) {

	        if (! pep->f.srvargs) {

	            rs = vecstr_start(pep->sap,2,0) ;
	            if (rs >= 0)
	                pep->f.srvargs = true ;

	        }

	        if (pep->f.srvargs)
	            rs = vecstr_add(pep->sap,cp,-1) ;

	    }

	} /* end if (setting 'argv[0]') */

/* we're out of here */

	if (rs >= 0)
	    goto retok ;

/* bad things */
bad7:
	freeit(&pep->options) ;

bad6:
	freeit(&pep->groupname) ;

bad5:
	freeit(&pep->username) ;

bad4:
	if (pep->f.srvargs) {
	    pep->f.srvargs = false ;
	    vecstr_finish(pep->sap) ;
	}

bad3:
	if (pep->program != nullptr)
	    freeit(&pep->program) ;

bad2:
	if ((pep->efname != nullptr) && (pep->efname[0] != '\0')) {
	    u_unlink(pep->efname) ;
	}

bad1:
	if ((pep->ofname != nullptr) && (pep->ofname[0] != '\0')) {
	    u_unlink(pep->ofname) ;
	}

bad0:
retok:
	esap->interval = oldinterval ;
	esap->service = oldservice ;

	return rs ;
}
/* end subroutine (svcentry_expand) */

int svcentry_arrival(svcentry *pep,time_t *tp) noex {
	int		rs ;
	if ((rs = svcentry_ctor(pep)) >= 0) {
	    if (tp) *tp = pep->atime ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_arrival) */

int svcentry_stime(svcentry *pep,time_t daytime) noex {
	int		rs ;
	if ((rs = svcentry_ctor(pep)) >= 0) {
	    pep->stime = daytime ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcentry_stime) */


/* private subroutines */

static int svcentry_process(svcentry *pep,cc *inbuf,ARGS *esap,
		char *outbuf,int outlen) noex {
	int		rs = SR_OK ;
	int		vlen ;
	int		elen = 0 ;
	cchar		*ibp ;
	char		vbuf[OUTBUFLEN + 1] ;

	if (inbuf == nullptr) return SR_FAULT ;

	ibp = inbuf ;
	if (pep->ssp != nullptr) {

	    rs = varsub_exp(pep->ssp, vbuf,OUTBUFLEN, inbuf,-1) ;
	    vlen = rs ;
	    if (rs >= 0)
	        ibp = vbuf ;

	} else {
	    vlen = strlen(ibp) ;
	}

	if (rs >= 0) {
	    elen = expand(esap,ibp,vlen,outbuf,outlen) ;
	    if (elen < 0)
	        rs = SR_TOOBIG ;
	}

	return (rs >= 0) ? elen : rs ;
}
/* end subroutine (svcentry_process) */

static int svcentry_mkfile(SVCENTRY *pep,cchar *tmpdname,int type) noex {
	int		rs ;
	char		tfname[MAXPATHLEN+1] ;

	if ((rs = mkfile(tfname,tmpdname,type)) >= 0) {
	    cint	fl = rs ;
	    cchar	*cp ;
	    if ((rs = uc_mallocstrw(tfname,fl,&cp)) >= 0) {
		switch (type) {
		case 'o':
		    pep->ofname = cp ;
		    break ;
		case 'e':
		    pep->efname = cp ;
		    break ;
		} /* end switch */
	    } /* end if (malloc-str) */
	} /* end if (mkfile) */

	return rs ;
}
/* end subroutine (svcentry_mkfile) */

/* expand out a program argument with the substitution parameters */

/*
#	The following substitutions are made on command strings:

#		%V	Directory Watcher version string
#		%R	program root
#		%N	machine nodename
#		%D	machine DNS domain
#		%H	machine hostname
#		%U	invoking username
#		%G	invoking groupname

#		%s	service
#		%i	interval (decimal seconds)
#
*/

static int expand(ARGS *esap,cc *buf,int len,char *rbuf,int rlen) noex {
	int		elen, sl ;
	int		ch ;
	cchar		*bp = buf ;
	cchar		*cp ;
	char		hostbuf[MAXHOSTNAMELEN + 1] ;
	char		*rbp = rbuf ;

	rbuf[0] = '\0' ;
	if (len == 0)
	    return 0 ;

	if (len < 0)
	    len = strlen(buf) ;

	elen = 0 ;
	rlen -= 1 ;			/* reserve for zero terminator */
	while ((len > 0) && (elen < rlen)) {

	    ch = MKCHAR(*bp) ;
	    switch (ch) {

	    case '%':
	        bp += 1 ;
	        len -= 1 ;
	        if (len == 0)
	            return elen ;

	        sl = 0 ;
		ch = (*bp & 0xff) ;
	        switch (ch) {
	        case 'V':
	            cp = esap->version ;
	            sl = strlen(cp) ;
	            break ;
	        case 'R':
	            cp = esap->programroot ;
	            sl = strlen(cp) ;
	            break ;
	        case 'N':
	            cp = esap->nodename ;
	            sl = strlen(cp) ;
	            break ;
	        case 'D':
	            cp = esap->domainname ;
	            sl = strlen(cp) ;
	            break ;
	        case 'H':
	            sl = -1 ;
	            if (esap->hostname == nullptr) {
			cint	hlen = MAXHOSTNAMELEN ;
			cchar		*nn = esap->nodename ;
			cchar		*dn = esap->domainname ;
	                cp = hostbuf ;
	                sl = snsds(hostbuf,hlen,nn,dn) ;
	            } else {
	                cp = esap->hostname ;
		    }
	            if (sl < 0) sl = strlen(cp) ;
	            break ;
	        case 'U':
	            cp = esap->username ;
	            sl = strlen(cp) ;
	            break ;
	        case 'G':
	            cp = esap->groupname ;
	            sl = strlen(cp) ;
	            break ;
	        case 's':
	            cp = esap->service ;
	            if (cp != nullptr)
	                sl = strlen(cp) ;
	            break ;
	        case 'i':
	            if (esap->interval != nullptr) {
	                cp = esap->interval ;
	                if (cp != nullptr)
	                    sl = strlen(cp) ;
	            } else {
	                cp = "1" ;
	                sl = 1 ;
	            }
	            break ;
	        default:
	            cp = bp ;
	            sl = 1 ;
		    break ;
	        } /* end switch */
	        bp += 1 ;
	        len -= 1 ;
	        if ((elen + sl) > rlen)
	            return BAD ;

	        strncpy(rbp,cp,sl) ;
	        rbp += sl ;
	        elen += sl ;
	        break ;

	    default:
	        *rbp++ = *bp++ ;
	        elen += 1 ;
	        len -= 1 ;
		break ;

	    } /* end switch */

	} /* end while */

	rbuf[elen] = '\0' ;
	return elen ;
}
/* end subroutine (expand) */

static void freeit(cchar **pp) noex {
	if (*pp != nullptr) {
	    uc_free(*pp) ;
	    *pp = nullptr ;
	}
}
/* end subroutine (freeit) */

/* process an argument list */
static int vecstr_procargs(vecstr *alp,char *abuf) noex {
	int		rs = SR_OK ;
	int		i = 0 ;

	if (alp == nullptr) return SR_FAULT ;

	if ((abuf != nullptr) && (abuf[0] != '\0')) {
	    field	fsb ;
	    cint	alen = strlen(abuf) ;
	    char	terms[32] ;

	    fieldterms(terms,false," \t") ;

	    if ((rs = field_start(&fsb,abuf,alen)) >= 0) {
	        cint	flen = alen ;
	        char	*fbuf ;
		if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
	            int		fl ;
	            while ((fl = field_sharg(&fsb,terms,fbuf,flen)) > 0) {
	                i += 1 ;
	                rs = vecstr_add(alp,fbuf,fl) ;
		        if (fsb.term == '#') break ;
	                if (rs < 0) break ;
	            } /* end while */
		    uc_free(fbuf) ;
		} /* end if (m-a) */
	        field_finish(&fsb) ;
	    } /* end if (field) */

	} /* end if (non-empty arguments) */

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (processargs) */

/* make our little files for input and output of the server programs */
static int mkfile(char *outbuf,cc *tmpdname,int type) noex {
	sbuf		b ;
	int		rs ;
	int		rs1 ;
	char		plat[MAXPATHLEN + 2] ;

	if ((rs = sbuf_start(&b,plat,MAXPATHLEN)) >= 0) {

	    sbuf_strw(&b,tmpdname,-1) ;

	    sbuf_chr(&b,'/') ;

	    {
		int	nu = 0 ;
	        nu += sbuf_strw(&b,"pcspoll",-1) ;
	        nu += sbuf_chr(&b,type) ;
	        sbuf_strw(&b,xes,(14-nu)) ;
	    }

	    rs1 = sbuf_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sbuf) */

	if (rs >= 0) {
	    rs = mktmpfile(outbuf,plat,0600) ;
	}

	return rs ;
}
/* end subroutine (mkfile) */


