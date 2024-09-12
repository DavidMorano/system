/* splitaddr HEADER */
/* lang=C++20 */

/* splitaddr mail address management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1889 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	splitaddr

	Description:
	These object manages a mail splitaddr.

	Note: Strategy with memory allocation:
	We just have a static storage area (in the object) that is
	large enough to hold any valid mail address.  We copy the
	address we are splitting into this storage and then split
	it in place from there.  There is already too much (and
	superfluous) memory allocation going on in the world today!
	:-)

	Matching:
	We do "prefix" matching.

	list		candidate	match
	----------------------------------------------------

	b.a		a		N
	b.a		b.a		Y
	b.a		c.b.a		Y
	c.b.a		b.a		N
	c.b.a		d.b.a		N
	c.b.a		c.b.a		Y
	d@c.b.a		c.b.a		N
	d@c.b.a		y@c.b.a		N
	d@c.b.a		d@c.b.a		Y
	e@c.b.a		e@d.c.b.a	Y

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<netdb.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<usystem.h>
#include	<vechand.h>
#include	<localmisc.h>

#include	"address.h"
#include	"splitaddr.h"


/* local defines */

#define	SPLITADDR_DEFENTS	4

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
#endif

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX(MAILADDRLEN,2048)
#endif


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int splitaddr_start(SPLITADDR *op,cchar *ap) noex {
	cint		nents = SPLITADDR_DEFENTS ;
	int		rs ;
	int		n = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (ap == NULL) return SR_FAULT ;

	memset(op,0,sizeof(SPLITADDR)) ;

	if ((rs = vechand_start(&op->coms,nents,0)) >= 0) {
	    int		al = strlen(ap) ;
	    cchar	*tp ;
	    char	*bp ;
	    while (al && (ap[al-1] == '.')) {
		al -= 1 ;
	    }
	    if ((rs = uc_malloc((al+1),&bp)) >= 0) {
	        int	bl = al ;
	        int	f = FALSE ;

	        op->mailaddr = (cchar *) bp ;
		strwcpy(bp,ap,al) ;

	        while ((tp = strnrpbrk(bp,bl,".@")) != NULL) {

	            f = (*tp == '@') ;
	            n += 1 ;
	            rs = vechand_add(&op->coms,(tp+1)) ;
	            bl = (tp - bp) ;
	            bp[bl] = '\0' ;

	            if (f) break ;
	            if (rs < 0) break ;
	        } /* end while */

	        if ((rs >= 0) && (bp[0] != '\0')) {

	            if (! f) {
	                n += 1 ;
	                rs = vechand_add(&op->coms,bp) ;
	            } else
	                op->local = bp ;

	        } /* end if */

	        if (rs >= 0) op->nd = n ;

	    } /* end if */
	    if (rs < 0) {
	        vechand_finish(&op->coms) ;
	    }
	} /* end if */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (splitaddr_start) */

int splitaddr_finish(SPLITADDR *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->mailaddr != NULL) {
	    rs1 = uc_free(op->mailaddr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mailaddr = NULL ;
	}

	rs1 = vechand_finish(&op->coms) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (splitaddr_finish) */

int splitaddr_count(SPLITADDR *op) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	rs = vechand_count(&op->coms) ;

	return rs ;
}
/* end subroutine (splitaddr_count) */

int splitaddr_prematch(SPLITADDR *op,SPLITADDR *tp) noex {
	int		rs = SR_OK ;
	int		rs1, rs2 ;
	int		i = 0 ;
	int		f_so = FALSE ;
	int		f = FALSE ;
	cchar		*cp1, *cp2 ;

	if (op == NULL) return SR_FAULT ;

	if (tp == NULL) return SR_INVALID ;

/* CONSTCOND */
	while (TRUE) {

	    rs1 = vechand_get(&op->coms,i,&cp1) ;

	    if (rs1 < 0) break ;

	    f_so = FALSE ;
	    rs2 = vechand_get(&tp->coms,i,&cp2) ;

	    if (rs2 < 0) break ;

	    f_so = TRUE ;
	    f = (strcasecmp(cp1,cp2) == 0) ;

	    if (! f)
	        break ;

	    i += 1 ;
	} /* end while */

/* handle non-matching related errors first */

	if ((rs1 < 0) && (rs1 != SR_NOTFOUND))
	    rs = rs1 ;

	if ((rs >= 0) && (rs2 < 0) && (rs2 != SR_NOTFOUND))
	    rs = rs2 ;

/* candidate entry must be as long or longer than the list entry */

	if ((rs >= 0) && f)
	    f = f_so ;

/* candidate entry must have local-name match if list entry has local */

	if ((rs >= 0) && f && (op->local != NULL)) {

	    f = FALSE ;
	    if (tp->local != NULL)
	        f = (strcmp(op->local,tp->local) == 0) ;

	} /* end if */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (splitaddr_prematch) */

int splitaddr_audit(SPLITADDR *op) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	rs = vechand_audit(&op->coms) ;

	return rs ;
}
/* end subroutine (splitaddr_audit) */


