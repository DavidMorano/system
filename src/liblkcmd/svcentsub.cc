/* svcentsub SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* build up a server entry piece-meal as it were */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	svcentsub

	Description:
        This little object takes a regular server entry and substitutes
	values for keys possibly embedded in the string values inside the
	server entry.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"svcentsub.h"
#include	"mfslocinfo.h"
#include	"svcfile.h"


/* local defines */

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(4 * MAXPATHLEN)
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cpcchar	svckeys[] = { /* KEEP IN SYNC W/ HEADER */
	"file",
	"passfile",
	"so",
	"program",
	"args",
	"username",
	"groupname",
	"interval",
	"access",
	"opts",
	"failcont",
	"include",
	NULL
} ;


/* external variables */


/* exported subroutines */

int svcentsub_start(SVCENTSUB *op,LOCINFO *lip,SVCFILE_ENT *sep) noex {
	strpack		*spp = &op->strs ;
	cint		csize = SVCENTSUB_CSIZE ;
	int		rs ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;
	if (lip == NULL) return SR_FAULT ;
	if (sep == NULL) return SR_FAULT ;

	memclear(op) ;
	if ((rs = strpack_start(spp,csize)) >= 0) {
	    cint	elen = EBUFLEN ;
	    char	*ebuf ;
	    if ((rs = uc_malloc((elen+1),&ebuf)) >= 0) {
	        int	i ;
	        for (i = 0 ; sep->keyvals[i][0] != NULL ; i += 1) {
	            int		ki ;
	            cchar	*kp = sep->keyvals[i][0] ;
	            cchar	*vp = sep->keyvals[i][1] ;
	            if ((ki = matostr(svckeys,1,kp,-1)) >= 0) {
	                if ((rs = locinfo_varsub(lip,ebuf,elen,vp,-1)) >= 0) {
	                    cchar	*ep ;
	                    if ((rs = strpack_store(spp,ebuf,rs,&ep)) >= 0) {
	                        op->var[ki] = ep ;
	                    }
	                }
	            } /* end if (matostr) */
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = uc_free(ebuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    if (rs < 0) {
	        strpack_finish(&op->strs) ;
	    }
	} /* end if (strpack_start) */

	return rs ;
}
/* end subroutine (svcentsub_start) */

int svcentsub_finish(SVCENTSUB *op) noex {
	cint		vsz = (szof(cchar *) * svckey_overlast) ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	rs1 = strpack_finish(&op->strs) ;
	if (rs >= 0) rs = rs1 ;

	memset(op->var,0,vsz) ;

	return rs ;
}
/* end subroutine (svcentsub_finish) */


