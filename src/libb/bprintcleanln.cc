/* bprintcleanln SUPPORT */
/* lang=C++20 */

/* print a clean (cleaned up) line of text */
/* version %I% last-modified %G% */

#define	CF_BADSUB	1		/* fill with NBSP */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine prints out a cleaned up line of text.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* external variables */


/* local structures */


/* forward references */

#if	CF_BADSUB
static int	clean_nbsp(char *,int) noex ;
#else
static int	clean_del(char *,int) noex ;
#endif

static bool	ischarok(int) noex ;
static bool	isend(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int bprintcleanln(bfile *ofp,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		oli = 0 ;
	int		f_needeol = FALSE ;

	if (ofp == NULL) return SR_FAULT ;
	if (lp == NULL) return SR_FAULT ;

	if (ofp->magic != BFILE_MAGIC) return SR_NOTOPEN ;

	if (ofp->f.nullfile) goto ret0 ;

/* continue */

	if (ll < 0)
	    ll = strlen(lp) ;

	while ((ll > 0) && isend(lp[ll - 1])) {
	    ll -= 1 ;
	}

#if	CF_BADSUB
	oli = clean_nbsp(lp,ll) ;
#else
	oli = clean_del(lp,ll) ;
#endif

	if ((oli == 0) || (lp[oli - 1] != '\n')) {
	    f_needeol = TRUE ;
	}

	if ((rs >= 0) && (oli > 0)) {
	    rs = bwrite(ofp,lp,oli) ;
	}

	if ((rs >= 0) && f_needeol) {
	    oli += 1 ;
	    rs = bputc(ofp,'\n') ;
	}

ret0:

	return (rs >= 0) ? oli : rs ;
}
/* end subroutine (bprintcleanln) */


/* local subroutines */


#if	CF_BADSUB

static int clean_nbsp(char *lp,int ll) noex {
	int		ili = 0 ;
	bool		f ;
	for (ili = 0 ; ili < ll ; ili += 1) {
	    cint	ch = mkchar(lp[ili]) ;
	    f = isprintlatin(ch) || ischarok(ch) ;
	    if (! f) {
	        lp[ili] = cchar('¿') ;
	    }
	} /* end for */
	return ili ;
}
/* end subroutine (clean_nbsp) */

#else /* CF_BADSUB */
static int clean_del(char *lp,int ll) noex {
	int		oli = 0 ;
	int		f_flipped = FALSE ;
	bool		f ;
	for (int ili = 0 ; ili < ll ; ili += 1) {
	    cint	ch = mkchar(lp[ili]) ;
	    f = isprintlatin(ch) || ischarok(ch) ;
	    if (f) {
	        if (f_flipped) {
	            lp[oli++] = lp[ili] ;
	        } else {
	            oli += 1 ;
		}
	    } else {
	        f_flipped = TRUE ;
	    }
	} /* end for */
	return oli ;
}
/* end subroutine (cleandel) */

#endif /* CF_BADSUB */

static bool ischarok(int ch) noex {
	return ((ch == '\t') || (ch == '\n')) ;
}
/* end subroutine (ischarok) */

static int isend(int ch) noex {
	return ((ch == '\n') || (ch == '\r')) ;
}
/* end subroutine (isend) */


