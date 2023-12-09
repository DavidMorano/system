/* strtabfind */
/* lang=C20 */

/* find a string in tables created by a STRTAB object */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine finds a specified string in tables that
	were (assumed) created by a STRTAB object.

	Synopsis:
	int strtabfind(tab,it,itlen,nskip,sp,sl)
	const char	tab[] ;
	int		(*it)[3] ;
	int		itlen ;
	const char	*sp ;
	int		sl ;

	Arguments:
	tab		the string table
	it		the index table
	itlen		the length (number of entries) of index table
	nskip		the "skip" factor
	sp		pointer to string to lookup
	sl		length of string to lookup

	Returns:
	>=0		collisions
	<0		error code (NOTFOUND)


	Note: 
	This subroutine uses -> hash-linking <- to track down key
	matches in the index table.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>
#include	<hash.h>


/* local defines */

#define	MODP2(v,n)	((v) & ((n) - 1))


/* external subroutines */

extern int	nleadstr(const char *,const char *,int) noex ;


/* external variables */


/* exported variables */


/* local structures */


/* forward references */

static int	hashindex(uint,int) noex ;
static int	ismatkey(const char *,const char *,int) noex ;


/* local variables */


/* exported subroutines */

int strtabfind(cchar tab[],int (*it)[3],int itlen,int nskip,cchar *sp,int sl)
{
	uint		khash, nhash ;
	uint		chash ;
	int		nhi, hi ;
	int		nmax ;
	int		j ;
	int		si = -1 ;
	int		sc = 0 ; /* Skip-Count */
	int		f_mathash = FALSE ;
	int		f_mat = FALSE ;
	const char	*cp ;

	nmax = itlen + nskip ;
	khash = hash_elf(sp,sl) ;

	chash = (khash & INT_MAX) ;
	nhash = khash ;
	hi = hashindex(nhash,itlen) ;

	    f_mathash = ((it[hi][1] & INT_MAX) == chash) ;
	    if (f_mathash) break ;

	    if ((it[hi][1] & (~ INT_MAX)) == 0) {
		break ;
	    }

	    nhash = hashagain(nhash,j,nskip) ;

	    hi = hashindex(nhash,itlen) ;

	} /* end for */

	sc += j ;

	if ((si > 0) && f_mathash) {

	    while ((si = it[hi][0]) > 0) {

	        cp = (tab + si) ;
	        f_mat = (cp[0] == sp[0]) && ismatkey(cp,sp,sl) ;
	        if (f_mat)
	            break ;

		nhi = it[hi][2] ;
		if (nhi == 0)
		    break ;

		hi = nhi ;
		sc += 1 ;

	    } /* end while */

	} /* end if */

	return (f_mat) ? sc : SR_NOTFOUND ;
}
/* end subroutine (strtabfind) */


/* local subroutines */

static int hashindex(uint i,int n) noex {
	int	hi = MODP2(i,n) ;
	if (hi == 0) hi = 1 ;
	return hi ;
}
/* end subroutine (hashindex) */

static int ismatkey(cchar key[],cchar kp[],int kl) noex {
	int	f = (key[0] == kp[0]) ;
	if (f) {
	    int	m = nleadstr(key,kp,kl) ;
	    f = (m == kl) && (key[m] == '\0') ;
	}
	return f ;
}
/* end subroutine (ismatkey) */


