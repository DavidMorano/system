/* dialticotsord SUPPORT */
/* charset=ISO8859-1 */
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
	to		to ('>=0' mean use one, '-1' means do not)
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<uxti.h>
#include	<mallocxx.h>
#include	<strn.h>
#include	<sfx.h>
#include	<cfhexstr.h>
#include	<localmisc.h>


/* local defines */

#define	TPIDEV		"/dev/ticotsord"

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

static int	makeconn(subinfo *,cchar *,int,int) noex ;

#if	CF_PUSHMOD
static int	pushmod(int,cchar *) noex ;
#endif /* CF_PUSHMOD */


/* local variables */

constexpr bool		f_pushmod = CF_PUSHMOD ;


/* exported variables */


/* exported subroutines */

int dialticotsord(cchar *abuf,int alen,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	(void) opts ;
	if (abuf) {
	    if (char *addrbuf{} ; (rs = malloc_mp(&addrbuf)) >= 0) {
		cint	addlen = rs ;
	        if (alen < 0) {
	            if (strncmp(abuf,"\\x",2) == 0) {
	                abuf += 2 ;
	                alen = strlen(abuf) ;
	                if ((alen >> 1) <= addrlen) {
	                    rs = cfhexstr(abuf,alen,addrbuf) ;
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
	            if (subinfo g ; (rs = makeconn(&g,abuf,alen,to)) >= 0) {
	                fd = rs ;
	            }
	        }
	    	rs = rsfree(rs,addrbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialticotsord) */


/* local subroutines */

static int makeconn(subinfo *gp,cchar addr[],int alen,int to) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (gp) {
	    struct t_info	info{} ;
	    if ((rs = ut_open(TPIDEV,O_RDWR,&info)) >= 0) {
	        fd = rs ;
	        if ((rs = ut_bind(fd,nullptr,nullptr)) >= 0) {
	            struct t_call	*sndcall ;
	            if ((rs = ut_alloc(fd,T_CALL,0,(void **) &sndcall)) >= 0) {
	                sndcall->addr.maxlen = alen ;
	                sndcall->addr.buf = (char *) addr ;
	                sndcall->addr.len = alen ;
		        {
	                    rs = ut_connect(fd,sndcall,nullptr) ;
		        }
	                sndcall->addr.maxlen = 0 ;
	                sndcall->addr.buf = nullptr ;
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
	int		rs = SR_FAULT ;
	if (mods) {
	    rs = SR_NOTOPEN ;
	    if (fd >= 0) {
	        cchar	timod[] = "timod" ;
	        if (strcmp(mods,timod) == 0) {
	            if ((rs = u_ioctl(fd,I_LOOK,timod)) == SR_INVALID) {
	                rs = u_ioctl(fd,I_PUSH,timod) ;
	            }
	        } else { /* pop 'timod' if it is on the stack */
	            if (char *mbuf{} ; (rs = malloc_mp(&mbuf)) >= 0) {
		        cint	mlen = rs ;
	                if ((rs = u_ioctl(fd,I_LOOK,mbuf)) >= 0) {
	                    if (strcmp(mbuf,timod) == 0) {
	                        rs = u_ioctl(fd,I_POP,0) ;
	                    }
	                } else if (rs == SR_INVALID) {
	                    rs = SR_OK ;
	                }
	                if (rs >= 0) {
	                    int		cl ;
	                    cchar	*sp = mods ;
	                    cchar	*cp{} ;
	                    cchar	*tp ;
	                    while ((tp = strchr(sp,',')) != nullptr) {
	                        if ((cl = sfshrink(sp,(tp-sp),&cp)) > 0) {
	                            strnwcpy(mbuf,mlen,cp,cl) ;
	                            rs = u_ioctl(fd,I_PUSH,mbuf) ;
	                        }
	                        sp = (tp + 1) ;
	                        if (rs < 0) break ;
	                    } /* end while */
	                    if ((rs >= 0) && sp[0]) {
	                        if ((cl = sfshrink(sp,-1,&cp)) > 0) {
	                            strnwcpy(mbuf,mlen,cp,cl) ;
	                            rs = u_ioctl(fd,I_PUSH,mbuf) ;
	                        }
	                    } /* end if */
	                } /* end if (pushing) */
	                rs = rsfree(rs,mbuf) ;
	    	    } /* end if (m-a-f) */
		} /* end if (easy or more complex) */
	    } /* end if (valid) */
	} /* end if (non-null) */
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


