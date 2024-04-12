/* strpack_envstore */
/* lang=C20 */

/* vector-packed-string object (add environment variable) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
        This object module was morphed from some previous one. I do not remember
        what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strpack_envstore

	Description:
	This is a little add-on to the STRPACK object to add an
	environment variable in the form of a key-value pair.

	int strpack_envstore(op,kp,vl,vl,rpp)
	STRPACK		*op ;
	const char	*kp ;
	const char	*vp ;
	int		vl ;
	const char	**rpp ;

	Arguments:
	op		object pointer
	kp		key pointer
	vp		value pointer
	kl		value length
	rpp		result pointer

	Returns:
	>=0		the total length of the filled up strpack so far!
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"strpack.h"


/* local defines */


/* external subroutines */

extern char	*strwcpy(char *,cchar *,int) noex ;


/* local structures */


/* forward references */

int strpack_envstorer(STRPACK *,cchar *,int,cchar *,int,cchar **) noex ;


/* local variables */


/* exported subroutines */

int strpack_envstore(op,kp,vp,vl,rpp)
STRPACK		*op ;
const char	*kp ;
const char	*vp ;
int		vl ;
const char	**rpp ;
{
	return strpack_envstorer(op,kp,-1,vp,vl,rpp) ;
}
/* end subroutine (strpack_envstore) */


int strpack_envstorer(op,kp,kl,vp,vl,rpp)
STRPACK		*op ;
const char	*kp ;
int		kl ;
const char	*vp ;
int		vl ;
const char	**rpp ;
{
	int		rs = SR_OK ;
	int		size = 1 ;
	char		*ep ;

	if (op == NULL) return SR_FAULT ;
	if (kp == NULL) return SR_FAULT ;

	if (kl < 0) kl = strlen(kp) ;

	size += (kl+1) ;
	if (vp != NULL) {
	    if (vl < 0) vl = strlen(vp) ;
	    size += vl ;
	}

	if ((rs = uc_malloc(size,&ep)) >= 0) {
	    int		el ;
	    char	*bp = ep ;
	    bp = strwcpy(bp,kp,kl) ;
	    *bp++ = '=' ;
	    bp = strwcpy(bp,vp,vl) ;
	    el = (bp-ep) ;
	    rs = strpack_store(op,ep,el,rpp) ;
	    uc_free(ep) ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (strpack_envstorer) */


