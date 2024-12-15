/* rfile SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* subroutines to write to remote files or read from them */
/* version %I% last-modified %G% */


/* revision history:

	- 1996-11-21, Dave morano
	This subroutine code was started by copying from some other
	program (one of the other PCS remote host access programs
	or subroutines).

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rfile

	Description:
	The subroutine either returns a FD for the remote file or
	it returns an error which is indicated by a negative valued
	return.  Depending on the arguments to the subroutine call,
	both the INET 'exec' or 'shell' services may be invoked to
	try and make a connection to the remote host.

	Synopsis:
	int rfile(cc *rhost,rex_au *auth,cc *rfn,int flags,mode_t om) noex
	
	Arguments:
	rhost		remote host
	auth		pointer to REX_AU object
	rfn		remote file-name
	flags		option flags
	om		open-mode

	Returns:
	>=0		OK (and ere is the socket to the file)
	<0		error (system-return)

	Notes:
	struct rex_auth {
		char	*restrict ;
		char	*username ;
		char	*password ;
		NETFILE_ENT	**machinev ;
	} *auth ;

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<sys/utsname.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<netfile.h>
#include	<rex.h>
#include	<localmisc.h>

#include	"rfile.h"
#include	"incfile_rfilewrite.h"


/* local defines */

#define	PROG_SHELL	"/bin/sh"

#define	BUFLEN		(8 * 1024)
#define	CMDBUFLEN	8192


/* external subroutines */

extern "C" {
    extern int	reade() noex ;
    extern char	*strbasename() noex ;
}


/* external variables */


/* local structures */


/* forward subroutines */

static int	hostequiv() noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int rfile(cc *rhost,REX_AUTH *auth,cc *rfilename,int flags,mode_t mode) noex {
	REX_AUTH	aa, *ap ;
	NETFILE_ENT	*mp ;
	cnullptr	np{} ;
	int		i, j ;
	int		srs, rs, len, l ;
	int		fd, fd2 ;
	cchar	*prog_shell = PROG_SHELL ;
	cchar	*args[4] = {} ;

	char	*cp, *cp1, *cp2 ;

	char	buf[BUFLEN + 1], *bp ;
	char	hostname[BUFLEN + 1] ;
	char	jobfname[MAXPATHLEN + 1], *jobid ;


	if ((rhost == nullptr) || (rhost[0] == '\0'))
	    goto badhost ;

	if ((rfilename == nullptr) || (rfilename[0] == '\0'))
	    goto badfile ;


/* make a job file name */

	if ((rs = mkjobfile("/tmp",0600,jobfname)) < 0)
	    return SR_PROTO ;

	jobid = strbasename(jobfname) ;

/* start by opening a connection to the remote machine */

	f.keepalive = false ;
	if (flags & O_KEEPALIVE) 
		f.keepalive = true ;

	bufprintf(buf,BUFLEN,"/bin/cat > %s",jobfname) ;

	args[0] = "sh" ;
	args[1] = "-c" ;
	args[2] = buf ;
	args[3] = nullptr ;
	fd = rex(rhost,auth,0,prog_shell,args,np,np,&mp) ;

	if (fd < 0) {

	    u_unlink(jobfname) ;

	    return fd ;
	}

/* shutdown further reception on the socket */

	u_shutdown(fd,0) ;

	srs = fd ;

/* we have a connection, let's try to make the best use of it ! */

	if ((flags & O_WRONLY) || (flags & O_APPEND)) {

	    bp = buf ;
	    len = 0 ;

/* send over a greeting ! */

	    len += sprintf(buf + len," # <-- force CSH to use Bourne\n\n") ;

	    len += sprintf(buf + len,"# RFILE job\n\n") ;

	    len += sprintf(buf + len,"JOBFNAME=%s\n",jobfname) ;

/* send the file name over */

	    len += sprintf(buf + len,"FILE=%s\n",rfilename) ;

/* send over the "open" flags */

	    len += sprintf(buf + len,"F_C=%s\n",
	        (flags & O_CREAT) ? "true" : "false") ;

	    len += sprintf(buf + len,"F_T=%s\n",
	        (flags & O_TRUNC) ? "true" : "false") ;

	    len += sprintf(buf + len,"F_A=%s\n",
	        (flags & O_APPEND) ? "true" : "false") ;

/* send the file creation mode over */

	    len += sprintf(buf + len,"MODE=%4o\n",
	        mode & 0777) ;

/* send it all over there */

	    l = 0 ;
	    while (((len - l) > 0) && ((rs = u_write(fd,buf + l,len - l)) > 0))
	        l += rs ;

	    if (rs < 0)
	        srs = rs ;

/* send over the "write" program */

	    if (srs >= 0) {

	        len = INCFILELEN_rfilewrite ;
	        bp = (char *) incfile_rfilewrite ;
	        l = 0 ;
	        while (((len - l) > 0) && 
	            ((rs = u_write(fd,bp + l,len - l)) > 0))
	            l += rs ;

	        if (rs < 0)
	            srs = rs ;

	    } /* end if */

	    close(fd) ;

	} else {

	    close(fd) ;

	    srs = SR_PROTO ;
	}

/* try to execute the worm */

	fd = -1 ;
	if (srs >= 0) {

/* can we arrange for a short-cut for the REX connection? */

	    ap = auth ;
	    if ((ap != nullptr) && (mp != nullptr)) {

	        memcpy(&aa,ap) ;

	        aa.res = "rcmd" ;
	        if (mp->login != nullptr)
	            aa.username = mp->login ;

	        if (mp->password != nullptr)
	            aa.password = mp->password ;

	        ap = &aa ;

	    } /* end if (changing the authorization) */

	    args[0] = "rfile" ;
	    args[1] = jobfname ;
	    args[2] = nullptr ;
	    fd = rex(rhost,ap,0,"/bin/sh",args,np,&fd2,nullptr) ;

	    if (fd < 0) {

	        u_unlink(jobfname) ;

	        return fd ;
	    }

	    srs = fd ;

/* check if we have the go ahead signal from the other end */

	    len = 0 ;
	    while (((l = reade(fd2,buf + len,1,FM_NONE,15)) > 0)
	        && (buf[len] != '\n'))
	        len += l ;

	    if (l < 0) {
	        srs = l ;
	    } else if ((len < 1) || (strncmp(buf,"OK",2) != 0)) {
	        srs = SR_ACCES ;
	    }

	} /* end if (we attempted to execute the worm) */

	u_close(fd2) ;

	if ((srs < 0) && (fd >= 0))  {
	    u_close(fd) ;
	}

	u_unlink(jobfname) ;

	return srs ;

/* bad returns come here */
badret:
	return srs ;

badhost:
	srs = SR_INVAL ;
	goto badret ;

badfile:
	srs = SR_INVAL ;
	goto badret ;

}
/* end subroutine (rfile) */


