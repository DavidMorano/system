/* pcsuucp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get a FD to a file from another host using UUCP */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */
#define	CF_BDUMP	1		/* dump extraneous output */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uucp

	Description:
	This subroutine opens a file on a remote machine using UUCP.
	If then return an FD to the open file to caller.

	Synopsis:
	int uucp(cchar *rhost,cchar *filename,int *fd2p) noex

	Arguments:
	rhost		nodename of UUCP machine to contact
	filename	filename on remote machine to write
	fd2p		pointer to an integer to receive an FD to
			standard error from queuing process

	Returns:
	>=0		FD to remote file
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<sys/wait.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<strings.h>		/* BSD |strncasecmp(3c)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>		/* LIBB */

#include	"pcsuucp.h"


/* local defines */

#define	PROG_UUNAME	"/usr/bin/uuname"
#define	PROG_UUCP	"/usr/bin/uucp"

#define	CACHEDIR	"/var/tmp/cache"
#define	DUMPLEN		50
#define	DSTLEN		(2 * MAXPATHLEN)


/* external subroutines */


/* external variables */


/* forward reference */

local int	testuucp(cchar *) ;

#if	CF_BDUMP
static void	bdump(bfile *,bfile *) ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsuucp(cchar *rhost,cchar *filename,int *fd2p) noex {
	cmode	omode = (0600 | S_IFIFO) ;
	int		rs = SR_OK ;
	int		i ;
	int		pipes[3][2] ;
	int		pfd ;
	cchar	*cmd_uucp = PROG_UUCP ;
	char		pfname[MAXPATHLEN + 1] ;
	char		dst[DSTLEN + 1] ;
	DEBUGPRINTF("ent\n") ;
	/* check for bad input */
	if ((rhost == NULL) || (rhost[0] == '\0')) return SR_INVAL ;

	if (u_access(cmd_uucp,X_OK) < 0) 
	    return SR_PROTO ;

	DEBUGPRINTF("got in\n") ;

/* test the remote host for accessibility */

	rs = testuucp(rhost) ;
	if (rs < 0)
	    return SR_HOSTUNREACH ;

	DEBUGPRINTF("continuing\n") ;
	pfname[0] = '\0' ;

/* open up the necessary pipes */

	if ((fd2p != NULL) && ((rs = u_pipe(pipes[2])) < 0))
	    goto badpipes ;

	DEBUGPRINTF("about to make pipe file\n") ;

	rs = mktmpfile(pfname,omode,"/tmp/uufileXXXXXXXX") ;
	if (rs < 0) {
	    rs = SR_PROTO ;
	    goto badpipemk ;
	}

	DEBUGPRINTF("got pipes \n") ;
/* form the necessary UUCP command */

	rs = sncpy3(dst,DSTLEN,rhost,"!",filename) ;
	if (rs < 0) goto baddst ;

/* we fork the command */

	DEBUGPRINTF("about to fork\n") ;
	if ((rs = uc_fork()) == 0) { /* child */
	    int		fd ;
	    DEBUGPRINTF("inside fork\n") ;
	    for (i = 0 ; i < 3 ; i += 1)
	        u_close(i) ;

	    fd = u_open("/dev/null",O_RDWR,0600) ;

	    u_dup(fd) ;

	    if (fd2p != NULL) {

	        u_close(pipes[2][0]) ;

	        u_dup(pipes[2][1]) ;

	        u_close(pipes[2][1]) ;

	    } else {

	        u_dup(fd) ;

	    }

/* do the exec */

	    execl(cmd_uucp,"uucp","-C",pfname,dst,NULL) ;

	    uc_exit(EX_NOEXEC) ;

	} else if (rs < 0)
		goto badfork ;

	DEBUGPRINTF("main line continue\n") ;

/* close some pipe ends */

	if (fd2p != NULL) {
	    u_close(pipes[2][1]) ;
	    pipes[2][1] = -1 ;
	}

/* open the pipe file */

	DEBUGPRINTF("about to open the pipe\n") ;

	rs = u_open(pfname,O_WRONLY,0600) ;
	pfd = rs ;
	if (rs < 0)
	    goto badopen ;

#if	CF_DEBUG
	DEBUGPRINTF("opened pipe, about to read answer\n") ;
	sleep(10) ;
	system("ps -f") ;
#endif

/* read out the answer */

#ifdef	COMMENT
	{
	int f_bad = false ;
	while ((rs = reade(pipes[2][0],pfname,1,0,10)) > 0) {

	    DEBUGPRINTF("'reade' goto one\n") ;

	    f_bad = true ;
	}

	DEBUGPRINTF("read answer, f_bad=%d\n",f_bad) ;
	}
#endif /* COMMENT */

/* we are out of here */

#ifdef	COMMENT
	u_close(pipes[2][0]) ;
#endif

	u_unlink(pfname) ;

ret0:

	DEBUGPRINTF("ret rs=%d fd=%d\n", rs,pfd) ;
	return (rs >= 0) ? pfd : rs ;

/* handle the bad cases */
badopen:

badfork:
baddst:
	if (pfname[0] != '\0')
		u_unlink(pfname) ;

badpipemk:
	if (fd2p != NULL) {
	    u_close(pipes[2][0]) ;
	    u_close(pipes[2][1]) ;
	}

badpipes:
	goto ret0 ;
} /* end subroutine (uucp) */


/* local subroutines */

/* check for UUCP availability */
local int testuucp(cchar *queue_machine) {
	bfile		file0, file2 ;
	bfile		procfile, *pfp = &procfile ;
	bfile		*fpa[3] ;
	pid_t		pid ;
	int		rs = SR_OK ;
	int		child_stat ;
	int		i, j, l ;
	cchar	*cmd_uuname = PROG_UUNAME ;
	char		buf[NODENAMELEN + 1] ;
	char		*cp ;
	DEBUGPRINTF("ent\n") ;
	if ((queue_machine == NULL) || (queue_machine[0] == '\0'))
	    return BAD ;

	DEBUGPRINTF("got in\n") ;
	if (u_access(cmd_uuname,X_OK) < 0) 
		return BAD ;

	fpa[0] = &file0 ;
	fpa[1] = pfp ;		/* capture the standard output ! */
	fpa[2] = &file2 ;

	DEBUGPRINTF("about to open command - FPA[1]=%08X\n",fpa[1]) ;
	DEBUGPRINTF("FPA[0]=%08X\n",fpa[0]) ;
	if ((rs = bopencmd(fpa,cmd_uuname)) >= 0) {
	    DEBUGPRINTF("opened command OK\n") ;
	    pid = rs ;
	    bclose(fpa[0]) ;

/* find the part of the machine name that we like */

	    i = strlen(queue_machine) ;

	    if ((cp = strchr(queue_machine,'.')) != NULL)
	        i = (cp - queue_machine) ;

	    rs = BAD ;
	    while ((l = breadln(pfp,buf,NODENAMELEN)) > 0) {

#if	CF_DEBUG && 0
	        DEBUGPRINTF("got a line\n") ;
#endif

	        buf[l] = '\0' ;
	        cp = strshrink(buf) ;

	        j = strlen(cp) ;

	        if ((i == j) && (strncasecmp(cp,queue_machine,i) == 0)) {
	            DEBUGPRINTF("got a machine match\n") ;

	            rs = OK ;
	            break ;
	        }
	    }

#if	CF_BDUMP
	    bdump(fpa[0],fpa[2]) ;
#endif

	    bclose(pfp) ;

	    bclose(fpa[2]) ;

	    u_waitpid(pid,&child_stat,0) ;

	} /* end if (program spawned) */
	DEBUGPRINTF("passed the command OK\n") ;
	return rs ;
} /* end subroutine (testuucp) */

#if	CF_BDUMP

static void bdump(bfile *f1p,bfile *f2p) noex {
	int	f_done1 = false ;
	int	f_done2 = false ;
	char	buf[DUMPLEN + 1] ;
	while ((! f_done1) || (! f_done2)) {
	    if (! f_done1) {
	        if (bread(f1p,buf,DUMPLEN) <= 0) {
	            f_done1 = true ;
		}
	    }
	    if (! f_done2) {
	        if (bread(f2p,buf,DUMPLEN) <= 0) {
	            f_done2 = true ;
		}
	    }
	} /* end while */
} /* end subroutine (bdump) */

#endif /* CF_BDUMP */


