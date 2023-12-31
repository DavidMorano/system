/* transfer */

/* transfer data */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	1		/* switchable debug print-outs */
#define	CF_LOCALEOF	0		/* allow local EOF to exit */
#define	CF_SIGPIPE	1		/* ignore SIGPIPE */


/* revision history:

	= 1999-03-01, David A�D� Morano
	This program was originally written.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine transfer data among several file descriptors.


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>
#include	<unistd.h>
#include	<stropts.h>
#include	<stdlib.h>
#include	<string.h>
#include	<netdb.h>
#include	<time.h>

#include	<usystem.h>
#include	<logfile.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"proglog.h"
#include	"transfer.h"

#if	CF_DEBUGS || CF_DEBUG
#include	"debug.h"
#endif


/* local defines */

#define	TO_READ		30
#define	TO_PING		20

#undef	BUFLEN
#define	BUFLEN		((10 * 1024) + MAXHOSTNAMELEN)

#define	DENOM		(1000 * 1000)
#define	NFDS		6

#ifndef	SHUT_RD
#define	SHUT_RD		0
#define	SHUT_WR		1
#define	SHUT_RDWR	2
#endif

#ifndef	POLLINTMULT
#define	POLLINTMULT	1000
#endif


/* external subroutines */

extern int	isasocket(int) ;
extern int	inetping(const char *,int) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(const char *,...) ;
extern int	strlinelen(const char *,int,int) ;
#endif

extern char	*strwcpy(char *,const char *,int) ;
extern char	*timestr_log(time_t,char *) ;
extern char	*timestr_logz(time_t,char *) ;


/* external variables */


/* local structures */


/* local variables */


/* exported subroutines */


int transfer(pip,hostname,rfd,r2fd,ifd,ofd,efd,mxu)
PROGINFO	*pip ;
const char	hostname[] ;
int		rfd, r2fd ;
int		ifd, ofd, efd ;
int		mxu ;
{
	struct pollfd	fds[NFDS] ;
	struct fpstat	fp[NFDS] ;
	struct ustat	sb ;
	time_t		ti_pollsanity ;
	time_t		ti_sanity ;
	const int	pollinput = (POLLIN|POLLRDNORM|POLLRDBAND|POLLPRI) ;
	const int	polloutput = (POLLWRNORM | POLLWRBAND) ;
	int		rs = SR_OK ;
	int		pollint = (10 * POLLINTMULT) ;
	int		i ;
	int		nfds, len ;
	int		sanityfailures = 0 ;
	int		fdi = 0 ;
	int		loopcount = 0 ;
	int		c_already = 0 ;
	int		f_exit ;
	int		f_daytime = FALSE ;
	int		f_issock = isasocket(rfd) ;
	int		f_dgram = FALSE ;
	int		f ;
	char		buf[BUFLEN + 1] ;

#if	CF_DEBUGS || CF_DEBUG
	char		timebuf[TIMEBUFLEN + 1] ;
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("transfer: rfd=%d r2fd=%d\n",rfd,r2fd) ;
#endif

#if	CF_SIGPIPE
	uc_sigignore(SIGPIPE) ;
#endif /* CF_SIGPIPE */

	if (f_issock) {
	    if ((rs = uc_getsocktype(rfd)) >= 0) {
		f_dgram = (rs == SOCK_DGRAM) ;
	    }
	}

/* zero out stuff */

	for (i = 0 ; i < 6 ; i += 1) {
	    fds[i].fd = -1 ;
	    fds[i].events = 0 ;
	    fds[i].revents = 0 ;
	    memset((fp+i),0,sizeof(struct fpstat)) ;
	}

/* standard input */

	fds[0].fd = -1 ;
	if (! pip->f.ni) {
	    fds[0].fd = ifd ;
	    fds[0].events = (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI) ;
	} else {
	    fp[0].eof = TRUE ;
	}

/* standard output */

	if (rs >= 0) {
	    fds[1].fd = -1 ;
	    if ((rs = u_fstat(ofd,&sb)) >= 0) {
	        fds[1].fd = ofd ;
	        fds[1].events = (POLLWRNORM | POLLWRBAND) ;
	    }
	}

/* standard error */

	fds[2].fd = -1 ;
	if ((r2fd >= 0) && (u_fstat(efd,&sb) >= 0)) {

	    fds[2].fd = efd ;
	    fds[2].events = (POLLWRNORM | POLLWRBAND) ;

#ifdef	COMMENT /* no input on STDERR! */
	    fds[2].events |= (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI) ;
#endif /* COMMENT */

	}

/* remote socket */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("transfer: remote socket ?\n") ;
#endif

	fds[3].fd = rfd ;
	fds[3].events = (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI) ;

	if (pip->f.ni) {

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("transfer: no-input mode\n") ;
#endif

	    if (f_issock) {
	        rs = u_shutdown(rfd,SHUT_WR) ;
	    } else {
	        u_write(rfd,buf,0) ;
	    }

	} else {
	    fds[3].events |= (POLLWRNORM | POLLWRBAND) ;
	}

/* secondary connection */

	fdi = 4 ;
	if (r2fd >= 0) {
	    fds[fdi].fd = r2fd ;
	    fds[fdi].events = (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI) ;
	    fds[fdi].events |= (POLLWRNORM | POLLWRBAND) ;
	    fdi += 1 ;
	}

/* what about sanity checking */

	if (pip->f.sanity) {
	    ti_pollsanity = 0 ;
	    ti_sanity = 1 ;
	    sanityfailures = 0 ;
	}

/* do the copy data function */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("transfer: about to loop\n") ;
#endif

	f_exit = FALSE ;
	while ((rs >= 0) && (! f_exit)) {
	    f_daytime = FALSE ;
	    if ((rs = u_poll(fds,fdi,pollint)) > 0) {
	        nfds = rs ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(4)) {
		struct timeval	tv ;
	        debugprintf("transfer: back from POLL w/ rs=%d\n",rs) ;
		uc_gettimeofday(&tv,NULL) ;
	        debugprintf("transfer: %s.%ld\n",
			timestr_log(((time_t) tv.tv_sec),timebuf),
			(tv.tv_usec/1000)) ;
	}
#endif /* CF_DEBUG */

#if	CF_DEBUG
	    if (pip->debuglevel > 2) {
	        for (i = 0 ; i < NFDS ; i += 1) {
	            debugprintf("transfer: fds%d %s\n",i,
	                d_reventstr(fds[i].revents,buf,BUFLEN)) ;
	        }
	    }
#endif /* CF_DEBUG */

/* check the actual low level events */

	    if (fds[0].revents != 0) {

	        if (fds[0].revents & pollinput) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: IN0\n") ;
#endif

	            fp[0].in = TRUE ;
	            fds[0].events &= (~ pollinput) ;

	        }

	        if (fds[0].revents & POLLHUP) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: HUP0\n") ;
#endif

	            fp[0].hup = TRUE ;
	            fp[0].out = FALSE ;
	            fds[0].events = 0 ;
		    fds[0].fd = -1 ;
	        }

	    } /* end if */

	    if ((! fp[0].in) && fp[0].hup) {
			fp[0].in = TRUE ;
			fp[0].hup = FALSE ;
			fp[0].final = TRUE ;
	    }

	    if (fds[1].revents != 0) {

	        if (fds[1].revents & polloutput) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: OUT1\n") ;
#endif

	            fp[1].out = TRUE ;
	            fds[1].events &= (~ polloutput) ;

	        }

	        if (fds[1].revents & POLLHUP) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: HUP1\n") ;
#endif

	            fp[1].hup = TRUE ;
	            fp[1].out = FALSE ;
	            fds[1].events &= (~ polloutput) ;

	        }

	    } /* end if */

	    if ((! fp[1].in) && fp[1].hup) {
			fp[1].in = TRUE ;
			fp[1].hup = FALSE ;
			fp[1].final = TRUE ;
	    }

	    if (fds[2].revents != 0) {

	        if (fds[2].revents & pollinput) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: IN2\n") ;
#endif

	            fp[2].in = TRUE ;
	            fds[2].events &= (~ pollinput) ;

	        }

	        if (fds[2].revents & polloutput) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: OUT2\n") ;
#endif

	            fp[2].out = TRUE ;
	            fds[2].events &= (~ polloutput) ;

	        }

	        if (fds[2].revents & POLLHUP) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: HUP2\n") ;
#endif

	            fp[2].hup = TRUE ;
	            fp[2].out = FALSE ;
	            fds[2].events &= (~ polloutput) ;

	        }

	    } /* end if */

	    if ((! fp[2].in) && fp[2].hup) {
			fp[2].in = TRUE ;
			fp[2].hup = FALSE ;
			fp[2].final = TRUE ;
	    }

/* the remote promary connection */

	    if (fds[3].revents != 0) {

	        if (fds[3].revents & pollinput) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: IN3\n") ;
#endif

	            fp[3].in = TRUE ;
	            fds[3].events &= (~ pollinput) ;

	        }

	        if (fds[3].revents & polloutput) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: OUT3\n") ;
#endif

	            fp[3].out = TRUE ;
	            fds[3].events &= (~ polloutput) ;

	        }

	        if (fds[3].revents & POLLHUP) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: HUP3\n") ;
#endif

	            fp[3].hup = TRUE ;
	            fp[3].out = FALSE ;
	            fds[3].events &= (~ polloutput) ;

	        }

	        if ((fds[3].revents & POLLERR) ||
	            (fds[3].revents & POLLNVAL)) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2) {
	                if (fds[3].revents & POLLERR)
	                    debugprintf("transfer: ERR3\n") ;
	                if (fds[3].revents & POLLNVAL)
	                    debugprintf("transfer: NVAL3\n") ;
	            }
#endif /* CF_DEBUG */

	            fp[3].in = FALSE ;
	            fp[3].out = FALSE ;
	            fp[3].final = TRUE ;
	            fds[3].fd = -1 ;
	            fds[3].events = 0 ;

	        }

	    } /* end if */

	    if ((! fp[3].in) && fp[3].hup) {
			fp[3].in = TRUE ;
			fp[3].hup = FALSE ;
			fp[3].final = TRUE ;
	    }

/* the secondard connection */

	    if (fdi > 4) {

	        if (fds[4].revents != 0) {

	        if (fds[4].revents & pollinput) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: IN4\n") ;
#endif

	            fp[4].in = TRUE ;
	            fds[4].events &= (~ pollinput) ;

	        }

	        if (fds[4].revents & polloutput) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: OUT4\n") ;
#endif

	            fp[4].out = TRUE ;
	            fds[4].events &= (~ polloutput) ;

	        }

	        if (fds[4].revents & POLLHUP) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: HUP4\n") ;
#endif

	            fp[4].hup = TRUE ;
	            fp[4].out = FALSE ;
	            fds[4].events &= (~ polloutput) ;

	        }

	        if ((fds[4].revents & POLLERR) ||
	            (fds[4].revents & POLLNVAL)) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2) {
	                if (fds[4].revents & POLLERR)
	                    debugprintf("transfer: ERR4\n") ;
	                if (fds[4].revents & POLLNVAL)
	                    debugprintf("transfer: NVAL4\n") ;
	            }
#endif /* CF_DEBUG */

	            fp[4].in = FALSE ;
	            fp[4].out = FALSE ;
	            fp[4].final = TRUE ;
	            fds[4].fd = -1 ;
	            fds[4].events = 0 ;

	        }

		}

	    if ((! fp[4].in) && fp[4].hup) {
			fp[4].in = TRUE ;
			fp[4].hup = FALSE ;
			fp[4].final = TRUE ;
	    }

	    } /* end if (secondary connection) */

/* now we are ready to check for the events that we really want */

/* output from remote connection to our standard output */

	    if (fp[3].in && fp[1].out) {

	        len = u_read(rfd,buf,mxu) ;

#if	CF_DEBUG
	        if (pip->debuglevel > 2)
	            debugprintf("transfer: IN3 -> OUT1 len=%d\n",
	                len) ;
#endif

	        if (len <= 0) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: IN3 EOF\n") ;
#endif

	            fp[3].eof = TRUE ;
	            fp[3].in = FALSE ;
	            fds[3].events &= (~ pollinput) ;

	        } else
	            uc_writen(fds[1].fd,buf,len) ;

	        if ((! fp[3].eof) && (! (fp[3].hup || fp[3].final))) {

	            fp[3].in = FALSE ;
	            fds[3].events |= pollinput ;

	        } else if (fds[3].events == 0) {
	            fds[3].fd = -1 ;
		}

	        fp[1].out = FALSE ;
	        fds[1].events |= polloutput ;

	    } /* end if (remote connection to standard output) */

/* input from our standard input out to the remote connection */

	    if (fp[0].in && fp[3].out) {

	        len = u_read(ifd,buf,mxu) ;

#if	CF_DEBUG
	        if (pip->debuglevel > 2)
	            debugprintf("transfer: IN0 -> OUT3 len=%d\n",len) ;
#endif

	        if (len <= 0) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: IN0 EOF\n") ;
#endif

	            fp[0].eof = TRUE ;
	            fp[0].in = FALSE ;
	            fds[0].fd = -1 ;

#if	CF_DEBUG
	            if (pip->debuglevel > 2) {
	                rs = u_fstat(rfd,&sb) ;
	                debugprintf("transfer: RFD stat rs=%d mode=%08X\n",
	                    rs,sb.st_mode) ;
	            }
#endif /* CF_DEBUG */

	            if (f_issock) {

			if (f_dgram) {

	                rs = u_send(rfd,buf,0,0) ;

#if	CF_DEBUG
	                if (pip->debuglevel > 2) {
			    debugprintf("transfer: u_send() rs=%d\n",rs) ;
		        }
#endif

			}

	                u_shutdown(rfd,SHUT_WR) ;

	            } else
	                u_write(rfd,buf,0) ;

	        } else
	            uc_writen(fds[3].fd,buf,len) ;

	        if ((! fp[0].eof) && (! (fp[0].hup || fp[0].final))) {
	            fp[0].in = FALSE ;
	            fds[0].events |= pollinput ;
	        }

	        fp[3].out = FALSE ;
	        fds[3].events |= polloutput ;

	    } /* end if (standard input to primary socket) */

/* standard error to secondary socket */

	    if ((r2fd >= 0) && (fp[2].in && fp[4].out)) {

	        len = u_read(efd,buf,mxu) ;

#if	CF_DEBUG
	        if (pip->debuglevel > 2)
	            debugprintf("transfer: IN2 -> OUT4 len=%d\n",len) ;
#endif

	        if (len <= 0) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: IN2 EOF\n") ;
#endif

	            fp[2].eof = TRUE ;
	            fp[2].in = FALSE ;
	            fds[2].events &= (~ pollinput) ;

	            if (f_issock) {
	                u_shutdown(r2fd,SHUT_WR) ;
	            } else {
	                u_write(r2fd,buf,0) ;
		    }

	        } else
	            uc_writen(fds[4].fd,buf,len) ;

	        if ((! fp[2].eof) && (! (fp[2].hup || fp[2].final))) {
	            fp[2].in = FALSE ;
	            fds[2].events |= pollinput ;
	        } else if (fds[2].events == 0) {
	            fds[2].fd = -1 ;
		}

	        fp[4].out = FALSE ;
	        fds[4].events |= polloutput ;

	    } /* end if (standard error to secondary connection) */

/* secondary socket to standard error */

	    if ((r2fd >= 0) && (fp[4].in && fp[2].out)) {

	        len = u_read(r2fd,buf,mxu) ;

#if	CF_DEBUG
	        if (pip->debuglevel > 2)
	            debugprintf("transfer: IN4 -> OUT2 len=%d\n",
	                len) ;
#endif

	        if (len <= 0) {

#if	CF_DEBUG
	            if (pip->debuglevel > 2)
	                debugprintf("transfer: IN4 EOF\n") ;
#endif

	            fp[4].eof = TRUE ;
	            fp[4].in = FALSE ;
	            fds[4].events &= (~ pollinput) ;

	        } else {
	            uc_writen(fds[2].fd,buf,len) ;
		}

	        if ((! fp[4].eof) && (! (fp[4].hup || fp[4].final))) {
	            fp[4].in = FALSE ;
	            fds[4].events |= pollinput ;
	        } else if (fds[4].events == 0) {
	            fds[4].fd = -1 ;
		}

	        fp[2].out = FALSE ;
	        fds[2].events |= polloutput ;

	    } /* end if (secondary connection to standard error) */

	    } else if (rs == SR_INTR) {
		rs = SR_OK ;
	    }

/* should we break out ? */

	    if ((c_already > 0) && (nfds == 0)) {
	        if (c_already > 1) break ;
	        c_already += 1 ;
	    }

#if	CF_LOCALEOF
	    f = fp[0].eof || fp[3].eof ;
#else
	    f = fp[3].eof ;
#endif

	    if (f && (c_already == 0)) {
	        pollint = (POLLINTMULT / 4) ;
	        c_already = 1 ;
	    }

/* miscellaneous functions */

	    if (pip->f.sanity) {

		if (! f_daytime) {
		f_daytime = TRUE ;
	        pip->daytime = time(NULL) ;
		}

	        if ((pip->daytime - ti_pollsanity) > 
	            (pip->intkeep / SANITYFAILURES)) {

	            ti_pollsanity = pip->daytime ;
	            if (inetping(hostname,TO_PING) >= 0) {
	                sanityfailures = 0 ;
	                ti_sanity = pip->daytime ;
	            } else {
	                sanityfailures += 1 ;
		    }

	            if (((pip->daytime - ti_sanity) > pip->intkeep) &&
	                (sanityfailures >= SANITYFAILURES) &&
	                ((rs = inetping(hostname,TO_PING)) < 0))
	                break ;

	        } /* end if (sanity poll) */

	    } /* end if (sanity check) */

	    if (pip->open.logprog && ((loopcount % 3) == 0)) {
		if (! f_daytime) {
		    f_daytime = TRUE ;
	            pip->daytime = time(NULL) ;
		}
		proglog_check(pip) ;
	    } /* end if */

	    loopcount += 1 ;
	} /* end while (transferring data) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("transfer: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (transfer) */


