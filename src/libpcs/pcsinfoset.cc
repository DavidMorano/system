/* pcsinfoset SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* PCS set-information */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsinfoset

	Description:
	This subroutine sets the PCS names of a user.  Two different
	types of PCS names can be set, along with some other user
	information.  These are:

	0. regular name
	1. full name
	2. project-information
	3. organization

	Synopsis:
	int pcsinfoset(pr,nbuf,nlen,un,type)
	cchar	*pr ;
	cchar	*nbuf ;
	int		nlen ;
	cchar	*un ;
	int		type ;

	Arguments:
	pr		program root
	nbuf		caller-supplied name buffer
	nlen		caller-supplied name buffer length
	un		username
	type		type of name to set: 0=regular, 1=full

	Returns:
	>=0		OK
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	rmnames(cchar *,cchar *) noex ;
local int	setnames(cchar *,cchar *,cchar *,int) noex ;


/* local variables */

constexpr cpcchar	nfnames[] = {
	".name",
	".fullname",
	".project",
	".organization",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int pcsinfoset(cchar *pr,cchar *nbuf,int nlen,cchar *un,int nt) noex {
	cint	nnt = (nelem(nfnames)-1) ;
	int		rs = SR_OK ;
	int		f_set ;
	char		uh[MAXPATHLEN+1] ;
	char		nfname[MAXPATHLEN + 1] ;

	if (pr == nullptr) return SR_FAULT ;
	if (un == nullptr) return SR_FAULT ;

	if (un[0] == '\0') return SR_INVALID ;
	if ((nt < 0) || (nt > nnt)) return SR_DOM ;

	rs = getuserhome(uh,MAXPATHLEN,un) ;
	if (rs >= 0)
	    rs = mkpath2(nfname,uh,nfnames[nt]) ;

	if (rs >= 0) {

	    f_set = ((nbuf != nullptr) && (nbuf[0] != '*')) ;
	    if (f_set) {
	        rs = setnames(pr,nfname,nbuf,nlen) ;
	    } else
	        rs = rmnames(pr,nfname) ;

	} /* end if (name-file-name) */

	return rs ;
}
/* end subroutine (pcsinfoset) */


/* local subroutines */


local int rmnames(cchar *pr,cchar *nfname)
{
	ustat	sb ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (pr == nullptr) return SR_FAULT ;

	rs1 = u_stat(nfname,&sb) ;

#ifdef	COMMENT
	if ((rs1 >= 0) && (! S_ISDIR(sb.st_mode)))
	    rs = u_unlink(nfname) ;
#else /* COMMENT */
	if (rs1 >= 0) 
	    rs = removes(nfname) ;
#endif /* COMMENT */

	return rs ;
}
/* end subroutine (rmnames) */

local int setnames(cchar *pr,cchar *nfname,cchar *nbuf,int nlen) noex {
	cmode	om = 0664 ;
	cint	of = (O_CREAT|O_TRUNC|O_WRONLY) ;
	cint	to = -1 ;
	int		rs ;
	int		size ;
	char		*p ;

	if (pr == nullptr) return SR_FAULT ;

	if (nlen < 0) nlen = strlen(nbuf) ;

	size = (nlen+2) ;
	if ((rs = uc_malloc(size,&p)) >= 0) {
	    cchar	*np = (cchar *) p ;
	    char	*bp = (char *) p ;

	    if ((rs = sncpy2(bp,(size-1),nbuf,"\n")) >= 0) {
	        int	nl = rs ;
	        if ((rs = uc_opene(nfname,of,om,to)) >= 0) {
	            cint	fd = rs ;
	            if ((rs = u_write(fd,np,nl)) >= 0) {
	                rs = uc_fminmod(fd,0644) ;
		    }
	            u_close(fd) ;
	        } /* end if (opened) */
	    } /* end if */

	    uc_free(np) ;
	} /* end if (memory-allocations) */

	return rs ;
}
/* end subroutine (setnames) */


