/* getcanonical SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get a canonical host name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getcanonical

	Synopsis:
	int getcanonical() noex

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<localmisc.h>

#include	"getcanonical.h"


/* local defines */

#ifndef	HOSTBUFLEN
#define	HOSTBUFLEN		MAXPATHLEN
#endif

#undef	GETCANONICAL_SYSV
#define	GETCANONICAL_SYSV	SYSHAS_GETHOSTXXXR


/* external subroutines */


/* external variables */


/* forward references */


/* external variables */


#if	(! GETCANONICAL_SYSV)
extern int	h_errno ;
#endif


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getcanonical(cchar *name,char *rhost,char *rnode,char *rdomain) noex {
	HOSTENT		*hep ;
	UTSNAME		uts ;
	int	i ;

#if	GETCANONICAL_SYSV
	int	h_errno_local ;
#endif

	char	*cp, *cp1 ;

/* are we "doing" ourselves? */
	if (name == nullptr) {
	    if (uname(&uts) >= 0) {
	        name = uts.nodename ;
	    } else {
	        name = "localhost" ;
	    }
	}

#if	GETCANONICAL_SYSV
	h_errno_local = 0 ;
	do {
	    hep = gethostbyname_r(name,
	        &he,hostbuf,HOSTBUFLEN,&h_errno_local) ;
	    if ((hep == nullptr) && (h_errno_local == TRY_AGAIN)) sleep(1) ;
	} while ((hep == nullptr) && (h_errno_local == TRY_AGAIN)) ;
#else
	do {
	    hep = gethostbyname(name) ;
	    if ((hep == nullptr) && (h_errno == TRY_AGAIN)) sleep(1) ;
	} while ((hep == nullptr) && (h_errno == TRY_AGAIN)) ;
#endif /* GETCANONICAL_SYSV */

	if (hep != nullptr) {

	    if ((cp = strchr(hep->h_name,'.')) != nullptr) {

	        strcpy(rhost,hep->h_name) ;

	        *cp++ = '\0' ;
	        if (rnode != nullptr) {
	            strcpy(rnode,hep->h_name) ;
		}
	        if (rdomain != nullptr) {
	            strcpy(rdomain,cp) ;
		}

	        return OK ;

	    } /* end if */

	    for (i = 0 ; hep->h_aliases[i] != nullptr ; i += 1) {
	        if ((cp = strchr(hep->h_aliases[i],'.')) != nullptr) {
	            strcpy(rhost,hep->h_aliases[i]) ;

	            *cp++ = '\0' ;
	            if (rnode != nullptr) {
	                strcpy(rnode,hep->h_aliases[i]) ;
		    }
	            if (rdomain != nullptr) {
	                strcpy(rdomain,cp) ;
		    }
	            return OK ;

	        } /* end if */
	    } /* end for */
	} /* end if */

/* assume that any dots in the original name means that it is fully qualled */

	if ((cp = strchr(name,'.')) != nullptr) {
	    strcpy(rhost,name) ;
	    if (rnode != nullptr) {
		cint tl = intconv(cp -name) ;
	        strwcpy(rnode,name,tl) ;
	    }
	    if (rdomain != nullptr) {
	        strcpy(rdomain,cp + 1) ;
	    }
	    return OK ;

	} /* end if */

/* try to attach our local domain to it */

	if ((cp = getenv("LOCALDOMAIN")) != nullptr) {
		char	*bp ;
		while (CHAR_ISWHITE(*cp)) {
			cp += 1 ;
		}
		cp1 = cp ;
		while (*cp1 && (! CHAR_ISWHITE(*cp1))) {
			cp1 += 1 ;
		}
		bp = strwcpy(rhost,name,-1) ;

		*bp++ = '.' ;
		cint tl = intconv(cp1 - cp) ;
		strwcpy(bp,cp,tl) ;

	    if (rnode != nullptr) {
	        strcpy(rnode,name) ;
	    }
	    if (rdomain != nullptr) {
	        strcpy(rdomain,cp) ;
	    }

	    return OK ;
	}

	return BAD ;
}
/* end subroutine (getcanonical) */


