/* buffer_ext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string buffer object */
/* version %I% last-modified %G% */

#define	CF_FASTGROW	1		/* grow (extend) faster */

/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	buffer

	Description:
	This module can be used to construct strings or messages
	in buffers WITHOUT using the 'sprint' subroutine.  This
	module is useful when the user does NOT supply a buffer to
	be used as the working store.  Instead, a dynamically grown
	and managed buffer is maintained within the object.  This
	module uses an object, that must be initialized and eventually
	freed, to track the state of the dynamically used internal
	buffer.  An exponential growth is used for increasing the
	buffer size as needed.

	Arguments:
	- bop		pointer to the buffer object
	- <others>

	Returns:
	>=0		the total length of the filled up buffer so far!
	<0		error (system-return)

	Note:
	This module is not completely protected against calling
	methods when the object has not been initialized already.
	This is a little hit for "performance reasons" but the
	benefits are really questionable given how cheap it is to
	check for an uninitialized object!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<stdintx.h>
#include	<fmtstr.h>
#include	<strwcpy.h>
#include	<ctbin.h>
#include	<ctoct.h>
#include	<ctdec.h>
#include	<cthex.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"buffer.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ulibvals ;			/* |maxlinelen(3u)| */

/* local defines */

#ifndef	CF_FASTGROW
#define	CF_FASTGROW	0
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_fastgrow = CF_FASTGROW ;


/* exported variables */


/* exported subroutines */

namespace buffer_ns {
    int buffer_ext(buffer *op,int req) noex {
	int		rs = SR_OK ;
	int		need ; /* used-below */
	if (req < 0) req = op->startlen ;
	need = ((op->clen + (req + 1)) - op->dlen) ;
	if (need > 0) {
	    int		nlen ; /* used-multiple */
	    char	*nbuf{} ; /* used-multiple */
	    if (op->dbuf) {
	        nlen = max(op->startlen,need) ;
	        if ((rs = libmem.mall((nlen + 1),&nbuf)) >= 0) {
	            op->dbuf = nbuf ;
		    op->dlen = nlen ;
	        } else {
	            op->clen = rs ;
	        }
	    } else {
		nlen = op->dlen ;
	        while ((op->clen + (req + 1)) > nlen) {
		    if_constexpr (f_fastgrow) {
	                nlen = ((nlen + 1) * 2) ;
		    } else {
	                nlen = (nlen + BUFFER_STARTLEN) ;
		    }
	        } /* end while */
	        if ((rs = libmem.rall(op->dbuf,nlen,&nbuf)) >= 0) {
	            op->dbuf = nbuf ;
		    op->dlen = nlen ;
	        } else {
	            op->clen = rs ;
		}
	    } /* end if */
	} /* end if (extension needed) */
	return rs ;
    } /* end subroutine (buffer_ext) */
} /* end namespace (buffer_ns) */


