/* getnodeinfo SUPPORT */
/* lang=C20 */

/* get a cluster name given a nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This subroutine was originally written.

	= 1996-05-22, David A­D­ Morano
	This subroutine was enhanced to get the local node-name if
	one if not supplied, using 'getnodename(3dam)'.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getnodeinfo

	Description:
	Get a cluster name given a nodename.

	Synopsis:
	int getnodeinfo(cc *pr,char *cbuf,char *sbuf,vecstr *kp,cc *nn) noex

	Arguments:
	pr		program root
	cbuf		buffer to receive the requested cluster name
	sbuf		buffer to receive the requested system name
	kp		pointer to VECSTR to hold resulting key-value pairs
	nn		nodename used to find associated cluster

	Returns:
	>=0		string length of cluster name
	SR_OK		if OK
	SR_NOTFOUND	if could not get something needed for correct operation
	SR_ISDIR	database file was a directory (admin error)
	<0		some other error

	Design note:
	If there is no entry in the NODE DB file for the given
	nodename, then we proceed on to lookup the nodename in the
	CLUSTER DB.  Since we are using a nodedb object to read the
	CLUSTER DB file, results (key-value pairs) are returned in
	a random order.  If the idea was to return the => first <=
	cluster with the given node as a member, this will not
	always give predictable results.  This is just something
	to keep in mind, and another reason to have an entry for
	the given node in the NODE DB if deterministic results need
	to be returned for a cluster name lookup by nodename.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<nodedb.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<getnodename.h>
#include	<localmisc.h>

#include	"getnodeinfo.h"


/* local defines */

#define	NODEFNAME	"etc/node"
#define	CLUSTERFNAME1	"etc/clusters"
#define	CLUSTERFNAME2	"etc/cluster"

#ifndef	LOCALHOSTNAME
#define	LOCALHOSTNAME	"localhost"
#endif

#ifndef	VARNODE
#define	VARNODE		"NODE"
#endif


/* local namespaces */

using std::nullptr_t ;


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	vecstr_envadd(vecstr *,cchar *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int getnodeinfo(cc *pr,char *cbuf,char *sbuf,vecstr *klp,cc *nn) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = -1 ;
	char		*nbuf{} ;
	if (pr == nullptr) pr = "/" ;
	if ((rs = malloc_nn(&nbuf)) >= 0) {
	    cint	nlen = rs ;		/* "node" length */
	    cint	clen = rs ;		/* "cluster" length */
	    if ((rs = getnodename(nbuf,nlen)) >= 0) {
		char	*tbuf{} ;
		if ((rs = malloc_mp(&tbuf)) >= 0) {
	            if ((rs = mkpath2(tbuf,pr,NODEFNAME)) >= 0) {
	                nodedb		st ;
	                nodedb_ent	ste{} ;
	                cint		elen = NODEDB_ENTLEN ;
	                char		ebuf[NODEDB_ENTLEN + 1] ;
	                if ((rs = nodedb_open(&st,tbuf)) >= 0) {
			    auto	nf = nodedb_fetch ;
			    nullptr_t	np{} ;
	                    if ((rs = nf(&st,nn,np,&ste,ebuf,elen)) >= 0) {
	                        if (cbuf != nullptr) {
	                            rs = sncpy1(cbuf,clen,ste.clu) ;
			            len = rs ;
	                        } else {
	                            len = strlen(ste.clu) ;
			        }
	                        if ((rs >= 0) && (sbuf != nullptr)) {
	                            rs = sncpy1(sbuf,nlen,ste.sys) ;
			        }
	                        if ((rs >= 0) && (klp != nullptr)) {
	                            cchar	*kp, *vp ;
	                            for (int i = 0 ; ste.keys[i][0] ; i += 1) {
	                                kp = ste.keys[i][0] ;
	                                vp = ste.keys[i][1] ;
				        {
	                                    rs = vecstr_envadd(klp,kp,vp,-1) ;
				        }
	                                if (rs < 0) break ;
	                            } /* end for */
	                        } /* end if (keys) */
	                    } /* end if (fetched result found) */
	                    rs1 = nodedb_close(&st) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (DB opened) */
	            } /* end if (mkpath2) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (getnodename) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getnodeinfo) */


