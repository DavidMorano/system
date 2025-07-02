/* srventry SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* build up a server entry piece-meal as it were */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-09-01, David Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	srventry

	Description:
	This little object is used to create a server entry and to
	populate aspects of it with different operations on the
	object.  This object is used in "server" types of programs.
	This object is usually created from elements taken from the
	parsing of a server file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<netdb.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<varsub.h>
#include	<mallocstuff.h>
#include	<sfx.h>
#include	<snx.h>
#include	<localmisc.h>

#include	"srvtab.h"
#include	"srventry.h"


/* local defines */

#define	EBUFLEN		(10 * MAXPATHLEN)

#define	SE		srventry
#define	SE_ARGS		srventry_args


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	process(varsub *,cchar *,SE_ARGS *,cchar **) noex ;
static int	expand(cchar *,int,SE_ARGS *,char *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int srventry_start(SE *sep) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    rs = memclear(sep) ;
	    sep->program = nullptr ;
	    sep->srvargs = nullptr ;
	    sep->username = nullptr ;
	    sep->groupname = nullptr ;
	    sep->options = nullptr ;
	    sep->access = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (srventry_init) */

int srventry_finish(SE *sep) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    rs = SR_OK ;
	    if (sep->program != nullptr) {
	        uc_free(sep->program) ;
	    }
	    if (sep->srvargs != nullptr) {
	        uc_free(sep->srvargs) ;
	    }
	    if (sep->username != nullptr) {
	        uc_free(sep->username) ;
	    }
	    if (sep->groupname != nullptr) {
	        uc_free(sep->groupname) ;
	    }
	    if (sep->options != nullptr) {
	        uc_free(sep->options) ;
	    }
	    if (sep->access != nullptr) {
	        uc_free(sep->access) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (srventry_uc_free) */

int srventry_process(SE *sep,varsub *ssp,mainv envv,
		srvtab_ent *step,SE_ARGS *esap) noex {
	int		rs = SR_FAULT ;
	if (sep && sep && envv && step && esap) {
	    rs = SR_OK ;
	    if (step->program != nullptr) {
	        process(ssp,step->program,esap,&sep->program) ;
	    }
	    if (step->args != nullptr) {
	        process(ssp,step->args,esap,&sep->srvargs) ;
	    }
	    if (step->username != nullptr) {
	        process(ssp,step->username,esap,&sep->username) ;
	    }
	    if (step->groupname != nullptr) {
	        process(ssp,step->groupname,esap,&sep->groupname) ;
	    }
	    if (step->options != nullptr) {
	        process(ssp,step->options,esap,&sep->options) ;
	    }
	    if (step->access != nullptr) {
	        process(ssp,step->access,esap,&sep->access) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (srventry_process) */

int srventry_addprogram(SE *sep,cchar *program) noex {
	int		rs = SR_FAULT ;
	if (sep && program) {
	    sep->program = mallocstr(program) ;
	    rs = ((sep->program) ? SR_OK : SR_NOMEM) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (srventry_addprogram) */

int srventry_addsrvargs(SE *sep,cchar *srvargs) noex {
	int		rs = SR_FAULT ;
	if (sep && srvargs) {
	    sep->srvargs = mallocstr(srvargs) ;
	    rs = ((sep->srvargs) ? SR_OK : SR_NOMEM) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (srventry_addsrvargs) */

int srventry_addusername(SE *sep,cchar *username) noex {
	int		rs = SR_FAULT ;
	if (sep && username) {
	    sep->username = mallocstr(username) ;
	    rs = ((sep->username) ? SR_OK : SR_NOMEM) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (srventry_addusername) */

int srventry_addgroupname(SE *sep,cchar *groupname) noex {
	int		rs = SR_FAULT ;
	if (sep && groupname) {
	    sep->groupname = mallocstr(groupname) ;
	    rs = ((sep->groupname) ? SR_OK : SR_NOMEM) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (srventry_addgroupname) */

int srventry_addoptions(SE *sep,cchar *options) noex {
	int		rs = SR_FAULT ;
	if (sep && options) {
	    sep->options = mallocstr(options) ;
	    rs = ((sep->options) ? SR_OK : SR_NOMEM) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (srventry_addoptions) */


/* local subroutines */

static int process(varsub *vsp,cchar *inbuf,SE_ARGS *esap,cc **opp) noex {
	int		rs ;
	int		fl = 0 ;
	char		vbuf[EBUFLEN + 1] ;
	char		ebuf[EBUFLEN + 1] ;

	*opp = nullptr ;
	if ((rs = varsub_exp(vsp,vbuf,EBUFLEN,inbuf,-1)) >= 0) {
	    cint	vlen = rs ;
	    if ((rs = expand(vbuf,vlen,esap,ebuf,EBUFLEN)) >= 0) {
	        cint	elen = rs ;
		cchar	*fp{} ;
	        fl = sfshrink(ebuf,elen,&fp) ;
	        *opp = mallocstrw(fp,fl) ;
	    } /* end if */
	} /* end if (varsub_exp) */

	return (fl >= 0) ? rs : fl ;
}
/* end subroutine (process) */

/* expand out a program argument with the substitution parameters */

/*
#	The following substitutions are made on command strings:

#		%V	Directory Watcher version string
#		%S	search name
#		%R	program root
#		%N	machine nodename
#		%D	machine DNS domain
#		%H	machine hostname
#		%U	server username

#		%P	socket peername (if available)
#		%I	ident or network name (if available)

#		%s	service
#		%a	service arguments (if available)
#		%h	originating host (if known)
#		%u	network username (if known)
#		%p	network password (if known)
#
*/

static int expand(cchar *buf,int len,SE_ARGS *esap,
		char *rbuf,int rlen) noex {
	int		elen = 0 ;
	int		sl ;

	char	hostbuf[MAXHOSTNAMELEN + 1] ;
	char	*rbp = rbuf ;
	cchar	*bp = buf ;
	cchar	*cp ;

	rbuf[0] = '\0' ;
	if (len == 0)
	    return 0 ;

	if (len < 0)
	    len = lenstr(buf) ;

	rlen -= 1 ;			/* reserve for zero terminator */
	while ((len > 0) && (elen < rlen)) {

	    switch ((int) *bp) {
	    case '%':
	        bp += 1 ;
	        len -= 1 ;
	        if (len == 0) 
			return elen ;

	        sl = 0 ;
	        switch ((int) *bp) {
	        case 'V':
	            cp = esap->version ;
	            sl = lenstr(cp) ;
	            break ;
	        case 'S':
	            cp = esap->searchname ;
	            sl = lenstr(cp) ;
	            break ;
	        case 'R':
	            cp = esap->programroot ;
	            sl = lenstr(cp) ;
	            break ;
	        case 'N':
	            cp = esap->nodename ;
	            sl = lenstr(cp) ;
	            break ;
	        case 'D':
	            cp = esap->domainname ;
	            sl = lenstr(cp) ;
	            break ;
	        case 'H':
	            sl = -1 ;
	            if (esap->hostname == nullptr) {
			cint	hlen = MAXHOSTNAMELEN ;
			cchar	*s1 = esap->nodename ;
			cchar	*s2 = esap->domainname ;
	                cp = hostbuf ;
			sl = snsds(hostbuf,hlen,s1,s2) ;
	            } else {
	                cp = esap->hostname ;
		    }
	            if (sl < 0) {
	                sl = lenstr(cp) ;
		    }
	            break ;

		case 'U':
	            cp = esap->username ;
	            if (cp != nullptr)
	                sl = lenstr(cp) ;

		    break ;

	        case 'P':
	            cp = esap->peername ;
	            if (cp != nullptr)
	                sl = lenstr(cp) ;

	            break ;

	        case 's':
	            cp = esap->service ;
	            if (cp != nullptr)
	                sl = lenstr(cp) ;

	            break ;

	        case 'a':

	            cp = esap->svcargs ;
	            if (cp != nullptr)
	                sl = lenstr(cp) ;

	            break ;

	        case 'h':
	            cp = esap->nethost ;
	            if (cp != nullptr)
	                sl = lenstr(cp) ;

	            break ;

	        case 'u':
	            cp = esap->netuser ;
	            if (cp != nullptr)
	                sl = lenstr(cp) ;

	            break ;

	        case 'p':
	            cp = esap->netpass ;
	            if (cp != nullptr)
	                sl = lenstr(cp) ;

	            break ;

	        default:
	            cp = bp ;
	            sl = 1 ;

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

	    } /* end switch */

	} /* end while */

	rbuf[elen] = '\0' ;
	return elen ;
}
/* end subroutine (expand) */


