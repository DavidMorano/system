/* ns_getcname SUPPORT */
/* lang=C++20 */

/* subroutine to get a canonical hostname */
/* version %I% last-modified %G% */

#define	CF_LOG		1

/* revision history:

	= 1995-11-01, David A­D­ Morano
	This program was originally written.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine is used to get a canonical INET hostname
	for a supplied name. Note carefully that the returned
	hostname, if any, may NOT be a name which can be traslated
	into a good INET address. In other words, this subroutine
	defines its own definition of a "canonical" name and that
	definition does NOT necessarily include the fact that the
	resulting name can be translated into a good INET address.
	If you want a name which is guaranteed to be translatable
	into a valid INET address, then you want to investigate the
	subroutine GETEHOSTNAME (Get Entry HostName).

	Synopsis:
	int ns_getcname(cchar *name,char *hostname) noex

	Arguments:
	- name		name to lookup
	- hostname	if not NULL, a buffer to hold the resulting hostname

	Returns:
	>=0		<name> had a valid INET address
	<0		<name> did not have a valid address (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<sys/utsname.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<signal.h>
#include	<time.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<pwd.h>
#include	<grp.h>

#include	<usystem.h>
#include	<baops.h>
#include	<bfile.h>

#if	CF_LOG
#include	<logfile.h>
#endif

#include	<localmisc.h>


/* local defines */

#define	HOSTBUFLEN	(4 * 1024)

#ifndef	LOGIDLEN
#define	LOGIDLEN	80
#endif

#define	SERIALFILE1	"/tmp/serial"
#define	SERIALFILE2	"/tmp/ns_getcname.serial"
#define	LOGFNAME	"/tmp/ns_getcname.log"
#define	DEFLOGSIZE	(10 * MAXHOSTNAMELEN)


/* external subroutines */

extern int	ns_getheaddr(cvoid *,HOSTENT *,char *,int) ;


/* forward references */

static int	nsi_getthing2() ;


/* external variables */


/* global variables */


/* local structures */


/* local variables */


/* exported subroutines */

int ns_getcname(cchar *name,char *hostname) noex {

#if	CF_LOG
	bfile	sf, *sfp = &sf ;
#endif

	HOSTENT	he, *hep ;

	struct utsname	uts ;

#if	CF_LOG
	logfile	lh ;
#endif

	unsigned long	addr ;

	int	rs = SR_OK ;
	int	i, l ;

#if	CF_LOG
	int	slen, brs, pid, serial ;
#endif

	char	localnode[NODENAMELEN + 1] ;
	char	localdomain[MAXHOSTNAMELEN + 1] ;
	char	newname[MAXHOSTNAMELEN + 1] ;
	char	altname[MAXHOSTNAMELEN + 1] ;
	char	hostbuf[HOSTBUFLEN + 1] ;
	char	*np, *cp ;

#if	CF_LOG
	char	logid[LOGIDLEN + 1] ;
#endif


#if	CF_LOG
	pid = getpid() ;

	if ((serial = ns_getserial(SERIALFILE1)) < 0)
		serial = ns_getserial(SERIALFILE2) ;

	sprintf(logid,"%d.%d",pid,serial) ;

	logfile_open(&lh,LOGFNAME,0,0666,logid) ;

	logfile_printf(&lh,"name=%s\n",(name == NULL) ? STDFNNULL : name) ;

#endif /* CF_LOG */


/* are we "doing" ourselves? or is the name OK? */

	if (name == NULL) {

	    if (uname(&uts) < 0)
	        name = "localhost" ;

	    else
	        name = uts.nodename ;

	}

/* remove any trailing dots from the name */

	    l = strnlen(name,MAXHOSTNAMELEN) ;

	    if (name[l - 1] == '.') {

	        while (name[l - 1] == '.') l -= 1 ;

	        strwcpy(altname,name,l) ;

	        name = altname ;

	    } /* end if */


/* continue */

	if (hostname != NULL)
	    hostname[0] = '\0' ;

	hep = &he ;
	if (nsi_getthing2(name,hostname,&he,hostbuf,HOSTBUFLEN) < 0)
	    hep = NULL ;

	if (hep != NULL) {

	    rs = OK ;
	    if (hostname == NULL)
	        goto good ;

	    if ((cp = strchr(hostname,'.')) != NULL)
	        goto good ;

	    getnodedomain(localnode,localdomain) ;

	    strcat(hostname,".") ;

	    strcat(hostname,localdomain) ;

		strcpy(newname,hostname) ;

	    rs = nsi_getthing2(newname,hostname,&he,hostbuf,HOSTBUFLEN) ;

	    goto ret ;

	} /* end if */

/* no entry as it was, does it have a domain part already? */

	if ((addr = inet_addr(name)) != (~ 0))
		goto bad ;

	getnodedomain(localnode,localdomain) ;

	if ((cp = strchr(name,'.')) != NULL) {

/* it has a domain, is it the same as our local domain? */

	    if (strcmp(cp + 1,localdomain) != 0)
	        goto bad ;

/* it has our domain, check it without the domain qualification */

	    strwcpy(newname,name,cp - name) ;

	    hep = &he ;
	    if (nsi_getthing2(newname,hostname,
	        hep,hostbuf,HOSTBUFLEN) >= 0) {

	        if (hostname != NULL) {

	            if ((cp = strchr(hostname,'.')) != NULL)
	                goto bad ;

	            strcat(hostname,".") ;

	            strcat(hostname,localdomain) ;

	        }

	        goto bad ;

	    } /* end if (we had an entry) */

	    goto bad ;

	} /* end if (it has a domain name) */

/* do the best that we can with what is left over */

	strcpy(newname,name) ;

	strcat(newname,".") ;

	strcat(newname,localdomain) ;

	hep = &he ;
	rs = nsi_getthing2(newname,hostname,hep,hostbuf,HOSTBUFLEN) ;

	if ((rs >= 0) && (hostname != NULL) && (strchr(hostname,'.') == NULL))
	    strcpy(hostname,newname) ;

/* let's get out of here */
good:

/* everybody returns here */
ret:

#if	CF_LOG
	cp = (rs == 0) ? "*succeeded*" : "*failed*" ;
	logfile_printf(&lh,"rs=%d result=%s\n",
	    rs,(hostname == NULL) ? cp : hostname) ;
#endif

#if	CF_LOG
	logfile_checksize(&lh,DEFLOGSIZE) ;

	logfile_close(&lh) ;

#endif /* CF_LOG */

	return rs ;

/* return a failure (we could not find a translation) */
bad:
	rs = BAD ;
	goto ret ;

}
/* end subroutine (ns_getcname) */


/* subroutine to search for a canonical hostname */

static int nsi_getthing2(name,hostname,hep,buf,buflen)
char		name[] ;
char		hostname[] ;
HOSTENT	*hep ;
char		buf[] ;
int		buflen ;
{
	HOSTENT	*lp ;
	unsigned long	addr ;
	int		rs ;
	int		i ;
	char		*cp ;

	if (hostname != NULL)
	    hostname[0] = '\0' ;

	if ((addr = inet_addr(name)) != (~ 0)) {
		rs = ns_getheaddr(&addr,hep,buf,buflen) ;
	} else {
		rs = ns_gethename(hep,buf,buflen,name) ;
	}

	if (rs < 0) {

	    return rs ;
	}

	if (hostname == NULL) return OK ;

/* can we get a domain name out of this */

	if ((cp = strchr(hep->h_name,'.')) != NULL) {

	    strcpy(hostname,hep->h_name) ;

	    return OK ;

	} /* end if */

	for (i = 0 ; hep->h_aliases[i] != NULL ; i += 1) {

	    if ((cp = strchr(hep->h_aliases[i],'.')) != NULL) {

	        strcpy(hostname,hep->h_aliases[i]) ;

	        return OK ;

	    } /* end if */

	} /* end for */

	if (strchr(name,'.') != NULL) {
	    strcpy(hostname,name) ;
	} else {
	    strcpy(hostname,hep->h_name) ;
	}

	return OK ;
}
/* end subroutine (nsi_getthing2) */


