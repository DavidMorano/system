/* dialticotsord SUPPORT */
/* lang=C++20 */

/* subroutine to dial over to a UNIX® domaiun socket */
/* version %I% last-modified %G% */

#define	CF_PUSHMOD	0		/* push TIRDWR */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialticotsord 

	Origin:
	-Dial
	-Transport
	-Interface
	-Connection
	-Oriented
	-Transport
	-Service
	-Orderly
	-Release

	Description:
	This subroutine will dial out to the TICOTSORD transport.

	Synopsis:
	int dialticotsord(cchar *addr,int alen,int to,int opts) noex

	Arguments:
	addr		XTI address
	alen		address of XTI address
	to		to ('>=0' mean use one, '-1' means don't)
	opts		any dial options

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<xti.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<strn.h>
#include	<sfx.h>
#include	<cfhex.h>
#include	<localmisc.h>


/* local defines */

#define	ADDRBUFLEN	MAXPATHLEN
#define	CONNTIMEOUT	120		/* seconds */
#define	LINGERTIME	(3 * 6)		/* seconds */
#define	TPIDEV		"/dev/ticotsord"

#define	HEXBUFLEN	((2 * MAXPATHLEN) + 2)

#define	SUBINFO		struct subinfo

#ifndef	CF_PUSHMOD
#define	CF_PUSHMOD	0		/* push TIRDWR */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	snxtilook(char *,int,int) noex ;
}


/* external variables */


/* local structures */

struct subinfo {
	int		f ;
} ;


/* forward references */

static int	makeconn(SUBINFO *,cchar *,int,int) noex ;

#if	CF_PUSHMOD
static int	pushmod(int,cchar *) noex ;
#endif /* CF_PUSHMOD */


/* local variables */

constexpr bool	f_pushmod = CF_PUSHMOD ;


/* exported variables */


/* exported subroutines */

int dialticotsord(cchar *abuf,int alen,int to,int opts) noex {
	SUBINFO		g ;
	int		rs = SR_OK ;
	int		fd = -1 ;
	char		addrbuf[ADDRBUFLEN + 1] ;
	(void) opts ;
	if (abuf == NULL) return SR_FAULT ;

	if (alen < 0) {
	    if (strncmp(abuf,"\\x",2) == 0) {
	        abuf += 2 ;
	        alen = strlen(abuf) ;
	        if ((alen >> 1) <= ADDRBUFLEN) {
	            rs = cfhexs(abuf,alen,addrbuf) ;
	            abuf = addrbuf ;
		    alen = rs ;
	        } else {
	            rs = SR_TOOBIG ;
		}
	    } else {
	        alen = strlen(abuf) ;
	    }
	} /* end if */

/* try to connect to the remote machine */

	if (rs >= 0) {
	    rs = makeconn(&g,abuf,alen,to) ;
	    fd = rs ;
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialticotsord) */


/* local subroutines */

static int makeconn(SUBINFO *gp,cchar addr[],int alen,int to) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (gp) {
	    struct t_info	info{} ;
	    if ((rs = ut_open(TPIDEV,O_RDWR,&info)) >= 0) {
	        fd = rs ;
	        if ((rs = ut_bind(fd,NULL,NULL)) >= 0) {
	            struct t_call	*sndcall ;
	            if ((rs = ut_alloc(fd,T_CALL,0,(void **) &sndcall)) >= 0) {
	                sndcall->addr.maxlen = alen ;
	                sndcall->addr.buf = (char *) addr ;
	                sndcall->addr.len = alen ;
		        {
	                    rs = ut_connect(fd,sndcall,NULL) ;
		        }
	                sndcall->addr.maxlen = 0 ;
	                sndcall->addr.buf = NULL ;
	                sndcall->addr.len = 0 ;
	                ut_free(sndcall,T_CALL) ;
	            } /* end if (alloc) */
    /* was this "busy" at all, requiring a TLOOK operation? */
	            if ((rs == SR_BUSY) || (rs == SR_LOOK)) {
	                rs = ut_look(fd) ;
	            }
	            if_constexpr (f_pushmod) {
	                if (rs >= 0) {
	                    rs = pushmod(fd,"tirdwr") ;
	                }
		    } /* end if_constexpr */
	        } /* end if (bind) */
	        if ((rs < 0) && (fd >= 0)) {
	            u_close(fd) ;
	        }
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (makeconn) */

#if	CF_PUSHMOD
static int pushmod(int fd,cchar *mods) noex {
	int		rs = SR_OK ;
	cchar		*timod = "timod" ;

	if (mods == NULL) return SR_FAULT ;
	if (fd < 0) return SR_NOTOPEN ;

	if (strcmp(mods,timod) == 0) {

	    if ((rs = u_ioctl(fd,I_LOOK,timod)) == SR_INVALID) {
	        rs = u_ioctl(fd,I_PUSH,timod) ;
	    }

	} else { /* pop 'timod' if it is on the stack */
	    char	mbuf[MAXNAMELEN + 1] ;

	    if ((rs = u_ioctl(fd,I_LOOK,mbuf)) >= 0) {
	        if (strcmp(mbuf,timod) == 0) {
	            rs = u_ioctl(fd,I_POP,0) ;
	        }
	    } else if (rs == SR_INVALID) {
	        rs = SR_OK ;
	    }
	    if (rs >= 0) {
	        int	cl ;
	        cchar	*sp = mods ;
	        cchar	*cp{} ;
	        cchar	*tp ;
	        while ((tp = strchr(sp,',')) != NULL) {
	            if ((cl = sfshrink(sp,(tp-sp),&cp)) > 0) {
	                strnwcpy(mbuf,MAXNAMELEN,cp,cl) ;
	                rs = u_ioctl(fd,I_PUSH,mbuf) ;
	            }
	            sp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end while */

	        if ((rs >= 0) && sp[0]) {
	            if ((cl = sfshrink(sp,-1,&cp)) > 0) {
	                strnwcpy(mbuf,MAXNAMELEN,cp,cl) ;
	                rs = u_ioctl(fd,I_PUSH,mbuf) ;
	            }
	        } /* end if */

	    } /* end if (pushing) */

	} /* end if (easy or more complex) */

	return rs ;
}
/* end subroutine (pushmod) */
#endif /* CF_PUSHMOD */

#if	COMMENT
static int shownetbuf(struct netbuf *p,cchar *s) noex {
	debugprintf("shownetbuf: id=%s\n",s) ;
	debugprintf("shownetbuf: maxlen=%d\n",p->maxlen) ;
	debugprintf("shownetbuf: len=%d\n",p->len) ;
	debugprinthex("shownetbuf",80,p->buf,p->len) ;
	return 0 ;
}
/* end suboroutine (shownetbuf) */
#endif /* COMMENT */


