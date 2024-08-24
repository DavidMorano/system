/* sfreadline */
/* lang=C20 */

/* Safe-Fast Read Line */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_SFPOLL	0		/* use 'sfpoll(3ast)' */
#define	CF_SFGETR	1		/* use 'sfgetr(3ast)' */
#define	CF_PEEK		0		/* use 'sfpeek(3ast)' */

/* revision history:

	= 2004-03-01, David A­D­ Morano
	This subroutine was originally written.  

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfreadlinetimed

	Description:
	This subroutine reads a line (while timed) using the SFIO
	package.

	Synopsis:
	int sfreadlinetimed(Sdio_t fp,char *rbuf,int rlen,int to) noex

	Arguments:
	fp		SFIO file-pointer
	rbuf		supplied buffer to receive result
	rlen		lrngth of supplied buffer
	to		time-out

	Returns:
	<0		error
	>=0		number of bytes read

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ast.h>			/* configures other stuff also */
#include	<sys/types.h>
#include	<time.h>
#include	<string.h>
#include	<sfio.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern int	snwcpy(char *,int,cchar *,int) ;
extern int	msleep(int) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern char	*strnchr(cchar *,int,int) ;


/* exported subroutines */

#if	CF_SFPOLL

#if	CF_PEEK

int sfreadlinetimed(Sfio_t *fp,char *rbuf,int rlen,int to) noex {
	int		rl = -1 ;
	if (fp && (rbuf) {
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
	        Sfio_t		*streams[2] ;
	        time_t		daytime = time(NULL) ;
	        time_t		stime ;
	        cchar	*tp ;
	        rl = 0 ;
	        stime = daytime ;
	        streams[0] = fp ;
	        streams[1] = NULL ;
	forever {
	    int	r = sfpoll(streams,1,1000) ;
	    if (r > 0) {
		int	v = sfvalue(fp) ;
		if (v & SF_READ) {
		    int		rbl = (rlen-rl) ;
		    int		pl ;
		    int		len ;
		    char	*rbp = (rbuf+rl) ;
	            pl = sfpeek(fp,rbp,rbl) ;

		    if (pl > 0) {
		        if ((tp = strnchr(rbp,pl,'\n')) != NULL) {
			    len = sfread(fp,rbp,(tp+1-rbp)) ;
			    if (len >= 0) {
			        rl += len ;
			    } else {
			        rl = -1 ;
			    }
			    break ;
		        } else {
			    len = sfread(fp,rbp,rbl) ;
			    if (len < 0) {
				rl = -1 ;
				break ;
			    } else if (len > 0) {
				rl += len ;
			    } else
				break ;
			}
		    } else if (pl == 0) {
			len = sfread(fp,rbp,rbl) ;
			if (len < 0) rl = -1 ;
			break ;
		    }
		} else {
		    msleep(10) ;
		}
	    } /* end if (positive) */
	    if (r < 0) {
		rl = -1 ;
		break ;
	    }
	    daytime = time(NULL) ;
	    if ((r == 0) && (to >= 0) && ((daytime - stime) >= to)) {
		if (rl == 0) rl = -1 ;
		break ;
	    }
	} /* end forever */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rl ;
}
/* end subroutine (sfreadlinetimed) */

#else /* CF_PEEK */

int sfreadlinetimed(Sfio_t *fp,char *rbuf,int rlen,int to) noex {
	int		rl = -1 ;
	if (fp && rbuf) {
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
	        Sfio_t	*streams[2] ;
	        time_t	ti_start ;
	        time_t	ti_now = time(NULL) ;
		rl = 0 ;
	        ti_start = ti_now ;
	        streams[0] = fp ;
	        streams[1] = NULL ;

/* CONSTCOND */
	forever {
	    int		r = sfpoll(streams,1,1000) ;
	    if (r > 0) {
		int	v = sfvalue(fp) ;
		if (v & SF_READ) {
		    int		rbl = (rlen-rl) ;
		    char	*rbp = (rbuf+rl) ;
		    cchar	*rp = sfgetr(fp,'\n',0) ;
		    if (rp != NULL) {
			v = sfvalue(fp) ;
			if (v >= 0) {
#ifdef	COMMENT
			    if ((v == 1) && (rp[0] == 4)) {
				rl = 0 ;
				break ;
			    }
#endif /* COMMENT */
			    rl = snwcpy(rbp,rbl,rp,v) ;
			    break ;
			} else {
			    rl = -1 ;
			}
		    } else {
		        rp = sfgetr(fp,'\n',SF_LASTR) ;
		        if (rp) {
			    v = sfvalue(fp) ;
			    if (v >= 0) {
			        rl = snwcpy(rbp,rbl,rp,v) ;
			    } else {
				rl = -1 ;
				break ;
			    }
			} else {
			    rl = 0 ;
			    break ;
			}
		    }
		} else {
		    msleep(10) ;
		}
	    } /* end if (poll says we have some data) */
	    if (r < 0) {
		rl = -1 ;
		break ;
	    }
	    if ((r == 0) && (to >= 0)) {
	        ti_now = time(NULL) ;
	        if ((ti_now - ti_start) >= to) {
		    if (rl == 0) rl = -1 ;
		}
	    }
	    if (rl < 0) break ;
	} /* end forever */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rl ;
}
/* end subroutine (sfreadlinetimed) */

#endif /* CF_PEEK */

#else /* CF_SFPOLL */

int sfreadlinetimed(Sfio_t *fp,char *rbuf,int rlen,int to) noex {
	int		rl = -1 ;
	if (fp && rbuf) {
	    rbuf[0] = '\0' ;
	    if (rlen > 0) {
		rl = 0 ;

#if     CF_SFGETR
        {
            cchar       *rp = sfgetr(fp,'\n',0) ;
	    if (rp != NULL) {
	        rl = sfvalue(fp) ;
	    } else {
	        rp = sfgetr(fp,'\n',SF_LASTR) ;
	        if (rp != NULL) {
	            rl = sfvalue(fp) ;
#if	CF_DEBUGS
		    debugprintf("sfreadlinetimed: sfvalue() rl=%d\n",rl) ;
#endif
		} else {
		    rl = 0 ;
		}
	    } /* end if */
	    if ((rl > 0) && (snwcpy(rbuf,rlen,rp,rl) < 0)) {
		rl = -1 ;
	    }
	}
#else /* CF_SFGETR */
	{
	    int ch = 0 ;
	    while ((rl < rlen) && ((ch = sfgetc(fp)) >= 0)) {
	        rbuf[rl++] = ch ;
	        if (ch == '\n') break ;
	    }
	    if (ch < 0) rl = -1 ;
	}
#endif /* CF_SFGETR */

	    } /* end if (valid) */
	} /* end if (non-null) */
	return rl ;
}
/* end subroutine (sfreadlinetimed) */

#endif /* CF_SFPOLL */

int sfreadline(Sfio_t *fp,char *rbuf,int rlen) noex {
	return sfreadlinetimed(fp,rbuf,rlen,-1) ;
}
/* end subroutine (sfreadline) */


