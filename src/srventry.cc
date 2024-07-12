/* srventry SUPPORT */
/* lang=C++20 */

/* build up a server entry piece-meal as it were */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-09-01, David Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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
#include	<localmisc.h>

#include	"srvtab.h"
#include	"srventry.h"


/* local object defines */

#undef	BUFLEN
#define	BUFLEN		(10 * MAXPATHLEN)



/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	process(varsub *,char *,SRVENTRY_ARGS *,char **) noex ;
static int	expand() noex ;

static void	freeit(char **) noex ;


/* external variables */


/* exported variables */


/* exported subroutines */

int srventry_start(SRVENTRY *sep) noex {
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

int srventry_finish(SRVENTRY *sep) noex {
	if (sep->program != nullptr)
	    free(sep->program) ;

	if (sep->srvargs != nullptr)
	    free(sep->srvargs) ;

	if (sep->username != nullptr)
	    free(sep->username) ;

	if (sep->groupname != nullptr)
	    free(sep->groupname) ;

	if (sep->options != nullptr)
	    free(sep->options) ;

	if (sep->access != nullptr)
	    free(sep->access) ;

	return SR_OK ;
}
/* end subroutine (srventry_free) */

int srventry_process(sep,ssp,envv,step,esap)
SRVENTRY	*sep ;
varsub		*ssp ;
char		**envv ;
SRVTAB_ENTRY	*step ;
SRVENTRY_ARGS	*esap ;
{


	if (step->program != nullptr)
	    process(ssp,step->program,esap,&sep->program) ;

	if (step->args != nullptr) {

	    process(ssp,step->args,esap,&sep->srvargs) ;

	}

	if (step->username != nullptr)
	    process(ssp,step->username,esap,&sep->username) ;

	if (step->groupname != nullptr)
	    process(ssp,step->groupname,esap,&sep->groupname) ;

	if (step->options != nullptr)
	    process(ssp,step->options,esap,&sep->options) ;

	if (step->access != nullptr)
	    process(ssp,step->access,esap,&sep->access) ;

	return SR_OK ;
}
/* end subroutine (srventry_process) */


int srventry_addprogram(sep,program)
SRVENTRY	*sep ;
char		program[] ;
{


	sep->program = mallocstr(program) ;

	return ((sep->program != nullptr) ? SR_OK : SR_NOMEM) ;
}


int srventry_addsrvargs(sep,srvargs)
SRVENTRY	*sep ;
char		srvargs[] ;
{


	sep->srvargs = mallocstr(srvargs) ;

	return ((sep->srvargs != nullptr) ? SR_OK : SR_NOMEM) ;
}


int srventry_addusername(sep,username)
SRVENTRY	*sep ;
char		username[] ;
{


	sep->username = mallocstr(username) ;

	return ((sep->username != nullptr) ? SR_OK : SR_NOMEM) ;
}


int srventry_addgroupname(sep,groupname)
SRVENTRY	*sep ;
char		groupname[] ;
{


	sep->groupname = mallocstr(groupname) ;

	return ((sep->groupname != nullptr) ? SR_OK : SR_NOMEM) ;
}


int srventry_addoptions(sep,options)
SRVENTRY	*sep ;
char		options[] ;
{


	sep->options = mallocstr(options) ;

	return ((sep->options != nullptr) ? SR_OK : SR_NOMEM) ;
}
/* end subroutine (srventry_addoptions) */


/* local subroutines */


static int process(vsp,inbuf,esap,opp)
varsub		*vsp ;
char		inbuf[] ;
SRVENTRY_ARGS	*esap ;
char		**opp ;
{
	int	rs ;

	int	vlen, elen ;
	int	fl ;

	char	vbuf[BUFLEN + 1] ;
	char	ebuf[BUFLEN + 1] ;
	char	*fp ;

	*opp = nullptr ;
	rs = varsub_expand(vsp, vbuf,BUFLEN, inbuf,-1) ;
	vlen = rs ;
	if (rs < 0)
	    goto ret0 ;

	elen = expand(vbuf,vlen,esap,ebuf,BUFLEN) ;
	if (elen < 0)
	    rs = SR_OVERFLOW ;

	if (rs >= 0) {

	    fl = sfshrink(ebuf,elen,&fp) ;

	    *opp = mallocstrw(fp,fl) ;

	} /* end if */

ret0:

	return (fl >= 0) ? rs : fl ;
}
/* end subroutine (process) */


/* expand out a program argument with the substitution parameters */

/*
#	The following substitutions are made on command strings :

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

static int expand(buf,len,esap,rbuf,rlen)
char		rbuf[], buf[] ;
int		rlen, len ;
SRVENTRY_ARGS	*esap ;
{
	int	elen = 0, sl ;

	char	hostbuf[MAXHOSTNAMELEN + 1] ;
	char	*rbp = rbuf ;
	char	*bp = buf ;
	char	*cp ;

	rbuf[0] = '\0' ;
	if (len == 0)
	    return 0 ;

	if (len < 0)
	    len = strlen(buf) ;

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
	            sl = strlen(cp) ;

	            break ;

	        case 'S':
	            cp = esap->searchname ;
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

	                cp = hostbuf ;
	                sl = bufprintf(hostbuf,MAXHOSTNAMELEN,"%s.%s",
	                    esap->nodename,esap->domainname) ;

	            } else
	                cp = esap->hostname ;

	            if (sl < 0)
	                sl = strlen(cp) ;

	            break ;

		case 'U':
	            cp = esap->username ;
	            if (cp != nullptr)
	                sl = strlen(cp) ;

		    break ;

	        case 'P':
	            cp = esap->peername ;
	            if (cp != nullptr)
	                sl = strlen(cp) ;

	            break ;

	        case 's':
	            cp = esap->service ;
	            if (cp != nullptr)
	                sl = strlen(cp) ;

	            break ;

	        case 'a':

	            cp = esap->svcargs ;
	            if (cp != nullptr)
	                sl = strlen(cp) ;

	            break ;

	        case 'h':
	            cp = esap->nethost ;
	            if (cp != nullptr)
	                sl = strlen(cp) ;

	            break ;

	        case 'u':
	            cp = esap->netuser ;
	            if (cp != nullptr)
	                sl = strlen(cp) ;

	            break ;

	        case 'p':
	            cp = esap->netpass ;
	            if (cp != nullptr)
	                sl = strlen(cp) ;

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

static void freeit(char **pp) noex {
	if (*pp != nullptr) {
	    free(*pp) ;
	    *pp = nullptr ;
	}
}
/* end subroutine (freeit) */


