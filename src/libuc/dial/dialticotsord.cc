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
#include	<sys/param.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uxti.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucsigset.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<cfhexstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"dialticotsord.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	TPIDEV		"/dev/ticotsord"

#define	SUB		subinfo

#ifndef	CF_PUSHMOD
#define	CF_PUSHMOD	0		/* push TIRDWR */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	snxtilook(char *,int,int) noex ;
} /* end */


/* external variables */


/* local structures */

struct subinfo {
	int		f ;
} ; /* end struct */


/* forward references */

local int	makeconn(SUB *,cchar *,int,int) noex ;
local int	pushmod(int,cchar *) noex ;


/* local variables */

constexpr bool		f_pushmod = CF_PUSHMOD ;


/* exported variables */


/* exported subroutines */

int dialticotsord(cchar *abuf,int alen,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	(void) opts ;
	if (abuf) {
	    rs = SR_NOSYS ;
	    if (syshas.xti) {
	        if (char *addrbuf ; (rs = lm_mp(&addrbuf)) >= 0) {
		    cint	addrlen = rs ;
	            if (alen < 0) {
	                if (strncmp(abuf,"\\x",2) == 0) {
	                    abuf += 2 ;
	                    alen = lenstr(abuf) ;
	                    if ((alen >> 1) <= addrlen) {
	                        rs = cfhexstr(abuf,alen,addrbuf) ;
	                        abuf = addrbuf ;
		                alen = rs ;
	                    } else {
	                        rs = SR_TOOBIG ;
		            }
	                } else {
	                    alen = lenstr(abuf) ;
	                }
	            } /* end if */
	            /* try to connect to the remote machine */
	            if (rs >= 0) {
	                if (SUB g ; (rs = makeconn(&g,abuf,alen,to)) >= 0) {
	                    fd = rs ;
	                }
	            } /* end if (ok) */
	    	    rs1 = lm_free(addrbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (syshas.xti) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (dialticotsord) */


/* local subroutines */

local int makeconn(SUB *gp,cchar addr[],int alen,int to) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	(void) to ;
	if (gp) {
	    t_info	info{} ;
	    if ((rs = ut_open(TPIDEV,O_RDWR,&info)) >= 0) {
	        fd = rs ;
	        if ((rs = ut_bind(fd,nullptr,nullptr)) >= 0) {
	            t_call	*sndcall{} ;
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
	        if ((rs < 0) && (fd >= 0)) nlikely {
	            u_close(fd) ;
	        } /* end if (error) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (makeconn) */

local int pushmod(int fd,cchar *mods) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mods) {
	    rs = SR_NOTOPEN ;
	    if (fd >= 0) {
	        cchar	timod[] = "timod" ;
	        if (strcmp(mods,timod) == 0) {
	            if ((rs = u_ioctl(fd,I_LOOK,timod)) == SR_INVALID) {
	                rs = u_ioctl(fd,I_PUSH,timod) ;
	            }
	        } else { /* pop 'timod' if it is on the stack */
	            if (char *mbuf{} ; (rs = lm_mp(&mbuf)) >= 0) {
		        cint	mlen = rs ;
	                if ((rs = u_ioctl(fd,I_LOOK,mbuf)) >= 0) {
	                    if (strcmp(mbuf,timod) == 0) {
	                        rs = u_ioctl(fd,I_POP,0) ;
	                    }
	                } else if (rs == SR_INVALID) {
	                    rs = SR_OK ;
	                }
	                if (rs >= 0) {
	                    int		cl ; /* used-multiple */
	                    cchar	*sp = mods ;
	                    cchar	*cp{} ;
	                    for (cc *tp ; (tp = strchr(sp,',')) != np ; ) {
				cint tl = intconv(tp - sp) ;
	                        if ((cl = sfshrink(sp,tl,&cp)) > 0) {
	                            strnwcpy(mbuf,mlen,cp,cl) ;
	                            rs = u_ioctl(fd,I_PUSH,mbuf) ;
	                        } /* end if */
	                        sp = (tp + 1) ;
	                        if (rs < 0) break ;
	                    } /* end for */
	                    if ((rs >= 0) && sp[0]) {
	                        if ((cl = sfshrink(sp,-1,&cp)) > 0) {
	                            strnwcpy(mbuf,mlen,cp,cl) ;
	                            rs = u_ioctl(fd,I_PUSH,mbuf) ;
	                        } /* end if */
	                    } /* end if */
	                } /* end if (ok) */
	                rs1 = lm_free(mbuf) ;
			if (rs >= 0) rs = rs1 ;
	    	    } /* end if (m-a-f) */
		} /* end if (easy or more complex) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pushmod) */

#if	COMMENT
local int shownetbuf(netbuf *p,cchar *s) noex {
	debugprintf("shownetbuf: id=%s\n",s) ;
	debugprintf("shownetbuf: maxlen=%d\n",p->maxlen) ;
	debugprintf("shownetbuf: len=%d\n",p->len) ;
	debugprinthex("shownetbuf",80,p->buf,p->len) ;
	return 0 ;
} /* end suboroutine (shownetbuf) */
#endif /* COMMENT */


