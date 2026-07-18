/* rcmdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get connection to remote command (unpriviledged) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_TESTRCMD	0		/* run the access-test code? */
#define	CF_PIPES	0		/* use System V pipes */
#define	CF_CMDPATH	1		/* try PATH for finding cmd_rsh */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	rcmdr

	Description:
	This is a dialer to use the underlying RSH program to make
	a "SHELL" remote login connection to another machine.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<sys/wait.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucproc.h>		/* LIBUC */
#include	<getehostname.h>	/* LIBUC */
#include	<exitcodes.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUGU */

#include	"rcmdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#if	defined(IRIX) && (! defined(TYPEDEF_INADDRT))
#define	TYPEDEF_INADDRT	1
typedef unsigned int	in_addr_t ;
#endif

#ifndef	NOADDR
#define	NOADDR		((in_addr_t) (~ 0))
#endif

#ifndef	NOFILE
#define	NOFILE		20
#endif

#define	CMDLEN		(64 * 1024)
#define	NTRIES		3
#define	NWAITINT	1
#define	TO_READ		5

#define	TESTRCMDU_LOOKLEN	64

#define	DUMPLEN			50


/* external subroutines */

extern int	findfilepath(cchar *,char *,cchar *,int) noex ;


/* external variables */


/* forward references */

#if	CF_TESTRCMD
local int	testrcmdu(cchar *,cchar *,cchar *,int) ;
local void	dump() ;
#endif /* CF_TESTRCMD */

#if	CF_DEBUGS
local int showdev(int) ;
#endif


/* local variables */

constexpr cpcchar	rshs[] = {
	"/usr/bin/rsh"
	"/usr/bin/remsh",
	"/usr/bin/ssh",
	"/usr/bin/ssh2",
	"/usr/bin/ssh1",
	"/usr/ucb/rsh",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int rcmdr(cchar *rhost,cchar *ruser,cchar *cmd,int *fd2p) noex {
	int	rs = SR_OK ;
	int	pipes[3][2] ;
	cchar	*cmd_rsh = nullptr ;
	char	cmdfname[MAXPATHLEN + 1] ;
	char	ehostname[MAXHOSTNAMELEN + 1] ;


#if	CF_DEBUGS
	debugprintf("rcmdr: rhost=%s\n",rhost) ;
	debugprintf("rcmdr: ruser=%s\n",ruser) ;
	debugprintf("rcmdr: cmd=>%r<\n",
	    cmd,strlinelen(cmd,-1,50)) ;
#endif

/* check for bad input */

	if ((rhost == nullptr) || (rhost[0] == '\0'))
	    return SR_INVALID ;

	if ((ruser != nullptr) && (lenstr(ruser) > USERNAMELEN))
	    return SR_TOOBIG ;

	if ((cmd == nullptr) || (cmd[0] == '\0'))
	    return SR_INVALID ;

	if ((lenstr(cmd) + 6) > CMDLEN)
	    return SR_2BIG ;

#if	CF_DEBUGS
	debugprintf("rcmdr: got in\n") ;
#endif

/* find the RSH program */

#if	CF_CMDPATH
	cmd_rsh = cmdfname ;
	rs = findfilepath(nullptr,cmdfname,"rsh",X_OK) ;
#else
	rs = SR_NOENT ;
#endif

	if (rs == SR_NOENT) {
	    int i ; /* used-afterwards */
	    for (i = 0 ; rshs[i] != nullptr ; i += 1) {
	        cmd_rsh = rshs[i] ;
	        rs = u_access(cmd_rsh,X_OK) ;
	        if (rs >= 0) break ;
	    } /* end for */
	    if (rshs[i] == nullptr) rs = SR_NOPKG ;
	} /* end if (trying alternatives) */

#if	CF_DEBUGS
	debugprintf("rcmdr: rs=%d cmd_rsh=%s\n",rs,cmd_rsh) ;
#endif

/* test the host name for addressability */

	if ((rs >= 0) && (inet_addr(rhost) == NOADDR)) {
	    if (getehostname(ehostname,rhost) < 0) {
	        rs = SR_HOSTUNREACH ;
	    }
	    if ((rs >= 0) && (strcmp(rhost,ehostname) != 0)) {
	        rhost = ehostname ;
	    }
	} /* end if (needed name service) */
	if (rs < 0) goto ret0 ;

#if	CF_TESTRCMD

/* test the remote host for accessibility */

	rs = SR_AGAIN ;
	for (int i = 0 ; i < NTRIES ; i += 1) {
	    if (i > 0) sleep(NWAITINT) ;
	    rs = testrcmdu(cmd_rsh,rhost,ruser) ;
	    if (rs != SR_AGAIN) break ;
	} /* end for */
	if (rs < 0) goto ret0 ;

#endif /* CF_TESTRCMD */

/* open up the necessary pipes */

#if	CF_PIPES
	rs = u_pipe(pipes[1]) ;
#else
	rs = u_socketpair(PF_UNIX,SOCK_STREAM,0,pipes[1]) ;
#endif

	if (rs < 0)
	    goto badpipe ;

	if (fd2p != nullptr) {

#if	CF_PIPES
	    rs = u_pipe(pipes[2]) ;
#else
	    rs = u_socketpair(PF_UNIX,SOCK_STREAM,0,pipes[2]) ;
#endif

	    if (rs < 0) {
	        for (int j = 0 ; j < 2 ; j += 1) {
	            u_close(pipes[1][j]) ;
		}
	        goto badpipe ;
	    }

	} /* end if */

#if	CF_DEBUGS
	debugprintf("rcmdr: got pipes \n") ;
#endif

/* we fork RSH */

#if	CF_DEBUGS
	debugprintf("rcmdr: about to fork\n") ;
#endif

	rs = uc_fork() ;
	if (rs < 0)
	    goto badfork ;

	if (rs == 0) {

#if	CF_DEBUGS
	    debugprintf("rcmdr: inside fork\n") ;
	    showdev(pipes[1][1]) ;
	    if (fd2p != nullptr)
	        showdev(pipes[2][1]) ;
#endif

	    for (int i = 0 ; i < 3 ; i += 1) {
	        u_close(i) ;
	    }

	    u_dup(pipes[1][1]) ;
	    u_dup(pipes[1][1]) ;
	    u_close(pipes[1][0]) ;
	    if (fd2p != nullptr) {
	        u_dup(pipes[2][1]) ;
	        for (int j = 0 ; j < 2 ; j += 1) {
	            u_close(pipes[2][j]) ;
		}
	    } else {
	        u_dup(pipes[1][1]) ;
	    }

	    u_close(pipes[1][1]) ;

#if	CF_DEBUGS
	    showdev(0) ;
	    showdev(1) ;
	    if (fd2p != nullptr)
	        showdev(2) ;
#endif

/* just check some other stuff a little bit */

#if	(! CF_DEBUGS)
	    for (int i = 3 ; i < NOFILE ; i += 1) {
	        u_close(i) ;
	    }
#endif

/* do the exec */

#if	CF_DEBUGS
	    debugprintf("rcmdr: RSH rhost=%s cmd=>%s<\n",rhost,cmd) ;
#endif

	    if (ruser != nullptr) {
	        execlp(cmd_rsh,"rsh","-l",ruser,rhost,cmd,nullptr) ;
	    } else {
	        execlp(cmd_rsh,"rsh",rhost,cmd,nullptr) ;
	    }

#if	CF_DEBUGS
	    debugprintf("rcmdr: exec() failed\n") ;
#endif

	    uc_exit(EX_NOEXEC) ;
	} /* end if (child) */

#if	CF_DEBUGS
	debugprintf("rcmdr: parent continues here\n") ;
#endif

/* close some pipe ends */

	u_close(pipes[1][1]) ;

	if (fd2p != nullptr) {
	    *fd2p = pipes[2][0] ;
	    u_close(pipes[2][1]) ;
	}

	rs = pipes[1][0] ;

ret0:

#if	CF_DEBUGS
	debugprintf("rcmdr: ret rs=%d\n",rs) ;
#endif

	return rs ;

/* bad stuff */
badfork:
	u_close(pipes[1][0]) ;
	u_close(pipes[1][1]) ;
	if (fd2p != nullptr) {
	    for (int j = 0 ; j < 2 ; j += 1) {
	        u_close(pipes[2][j]) ;
	    }
	}

badpipe:
	goto ret0 ;
}
/* end subroutine (rmcdr) */


/* local subroutines */

#if	CF_TESTRCMD

local int testrcmdu(cc *cmd_rsh,cc *rhost,cc *ruser) noex {
	pid_t	pid_child ;
	int	rs = SR_OK ;
	int	rs1 ;
	int	pipes[3][2] ;
	int	child_stat ;
	int	len1, len2 ;
	int	f_done1 ;
	int	f_done2 ;
	char	buf1[TESTRCMDU_LOOKLEN + 1] ;
	char	buf2[TESTRCMDU_LOOKLEN + 1] ;

	rs = u_socketpair(PF_UNIX,SOCK_STREAM,0,pipes[1]) ;
	if (rs < 0)
	    goto badpipe ;

	rs = u_socketpair(PF_UNIX,SOCK_STREAM,0,pipes[2]) ;
	if (rs < 0) {
	    u_close(pipes[1][0]) ;
	    u_close(pipes[1][1]) ;
	    goto badpipe ;
	}

#if	CF_DEBUGS
	debugprintf("rcmdr: got pipes \n") ;
#endif

/* we fork RSH */

#if	CF_DEBUGS
	debugprintf("rcmdr: fork()\n") ;
#endif

	rs = uc_fork() ;
	pid_child = rs ;
	if (rs < 0)
	    goto ret0 ;

	if (rs == 0) {

#if	CF_DEBUGS
	    debugprintf("rcmdr: child\n") ;
#endif

	    for (int i = 0 ; i < 3 ; i += 1) {
	        u_close(i) ;
	    }

	    u_dup(pipes[1][1]) ;
	    u_dup(pipes[1][1]) ;
	    u_close(pipes[1][0]) ;
	    u_close(pipes[1][1]) ;
	    u_dup(pipes[2][1]) ;
	    u_close(pipes[2][0]) ;
	    u_close(pipes[2][1]) ;

/* do the exec */

	    if (ruser != nullptr) {
	        execlp(cmd_rsh,"rsh","-n","-l",ruser,rhost,
	            "/bin/echo","YES",nullptr) ;
	    } else {
	        execlp(cmd_rsh,"rsh","-n",rhost,
	            "/bin/echo","YES",nullptr) ;
	    }

	    uc_exit(EX_NOEXEC) ;
	} /* end if */

/* parent continues here */

#if	CF_DEBUGS
	debugprintf("rcmdr: parent continues\n") ;
#endif

/* close some pipe ends */

	u_close(pipes[1][1]) ;
	u_close(pipes[2][1]) ;

/* the command is not reading the other end of this pipe anyway */

#if	(! CF_PIPES)
	u_shutdown(pipes[1][0],SHUT_WR) ;
#endif

/* read both the standard output and the error output for possible data */

	buf1[0] = buf2[0] = '\0' ;
	f_done1 = f_done2 = false ;
	len1 = len2 = 0 ;
	while ((! f_done1) && (! f_done2)) {

	    if (! f_done1) {

	        rs = u_read(pipes[1][0],(buf1 + len1),
	            (TESTRCMDU_LOOKLEN - len1)) ;

	        if (rs <= 0) {
	            f_done1 = true ;
	        } else {
	            len1 += rs ;
		}

	        if (len1 >= TESTRCMDU_LOOKLEN) {
	            f_done1 = true ;
		}

	    }

	    if (! f_done2) {

	        rs = u_read(pipes[2][0],buf2 + len2,
	            TESTRCMDU_LOOKLEN - len2) ;

	        if (rs <= 0) {
	            f_done2 = true ;
		} else {
	            len2 += rs ;
		}

	        if (len2 >= TESTRCMDU_LOOKLEN) {
	            f_done2 = true ;
		}

	    }

	} /* end while */

/* compare who has what */

	if ((len1 == 0) && (len2 == 0))
	    rs = SR_HOSTDOWN ;

	if (strncmp(buf1,"YES",3) == 0) {
	    rs = SR_OK ;

	} else if (strncmp(buf2,"permi",5) == 0) {
	    rs = SR_ACCES ;

	} else if (strncmp(buf2,"Login",5) == 0) {
	    rs = SR_ACCES ;

	} else if (strncmp(buf2,"comma",5) == 0) {
	    rs = SR_2BIG ;

	} else if (strncmp(buf2,"Hostn",5) == 0) {
	    rs = SR_NOTSUP ;

	} else if (strncmp(buf2,"Try a",5) == 0) {
	    rs = SR_AGAIN ;

	} else if (strncmp(buf2,"Can't",5) == 0) {
	    rs = SR_AGAIN ;

	} else {

#if	CF_DEBUGS
	    debugprintf("rcmdr: far_side> %s\n",buf2) ;
#endif

	    rs = SR_PROTO ;

	}

/* we are out of here! */

	dump(pipes[1][0],pipes[2][0]) ;
	u_close(pipes[1][0]) ;
	u_close(pipes[2][0]) ;
	u_waitpid(pid_child,&child_stat,0) ;

ret0:
	return rs ;

/* bad returns here */
badfork:
	u_close(pipes[1][0]) ;

	u_close(pipes[1][1]) ;

	u_close(pipes[2][0]) ;

	u_close(pipes[2][1]) ;

badpipe:
	goto ret0 ;
} /* end subroutine (testrmcdu) */

local void dump(int fd1,int fd2) noex {
	int		f_done1 = false ;
	int		f_done2 = false ;
	char		buf[DUMPLEN + 1] ;
	while ((! f_done1) || (! f_done2)) {
	    if (! f_done1) {
	        if (u_read(fd1,buf,DUMPLEN) <= 0) {
	            f_done1 = true ;
		}
	    }
	    if (! f_done2) {
	        if (u_read(fd2,buf,DUMPLEN) <= 0) {
	            f_done2 = true ;
		}
	    }
	} /* end while */
} /* end subroutine (dump) */

#endif /* CF_TESTRCMD */

#if	CF_DEBUGS
local int showdev(int fd) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    debugprintf("rcmdr: fd=%d rs=%d ino=%u dev=%08x\n",
	        fd,rs,sb.st_ino,sb.st_dev) ;
	}
	return rs ;
} /* end subroutine (showdev) */
#endif /* CF_DEBUGS */


