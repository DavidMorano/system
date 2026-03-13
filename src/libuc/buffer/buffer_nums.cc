/* buffer_nums SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string buffer object */
/* version %I% last-modified %G% */


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

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using buffer_ns::buffer_ext ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T>
int buffer_xxxx(buffer *op,int (*ctxxx)(char *,int,T),T v) noex {
	cint		dlen = DIGBUFLEN ;
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) ylikely {
	    if ((rs = buffer_ext(op,dlen)) >= 0) ylikely {
	        char	*bp = (op->dbuf + op->clen) ;
	        rs = ctxxx(bp,dlen,v) ;
	        op->clen += rs ;
		len = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine-template (buffer_xxxx) */

template<typename T>
int buffer_binx(buffer *op,T v) noex {
	return buffer_xxxx(op,ctbin,v) ;
} /* end subroutine-template (buffer_binx) */

template<typename T>
int buffer_octx(buffer *op,T v) noex {
	return buffer_xxxx(op,ctoct,v) ;
} /* end subroutine-template (buffer_octx) */

template<typename T>
int buffer_decx(buffer *op,T v) noex {
	return buffer_xxxx(op,ctdec,v) ;
}
/* end subroutine-template (buffer_decx) */

template<typename T>
int buffer_hexx(buffer *op,T v) noex {
	return buffer_xxxx(op,cthex,v) ;
}
/* end subroutine-template (buffer_hexx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int buffer_bini(buffer *op,int v) noex {
	return buffer_binx(op,v) ;
}
/* end subroutine (buffer_bini) */

int buffer_binl(buffer *op,long v) noex {
	return buffer_binx(op,v) ;
}
/* end subroutine (buffer_binl) */

int buffer_binll(buffer *op,longlong v) noex {
	return buffer_binx(op,v) ;
}
/* end subroutine (buffer_binll) */

int buffer_binui(buffer *op,uint v) noex {
	return buffer_binx(op,v) ;
}
/* end subroutine (buffer_binui) */

int buffer_binul(buffer *op,ulong v) noex {
	return buffer_binx(op,v) ;
}
/* end subroutine (buffer_binul) */

int buffer_binull(buffer *op,ulonglong v) noex {
	return buffer_binx(op,v) ;
}
/* end subroutine (buffer_binull) */

int buffer_octi(buffer *op,int v) noex {
	return buffer_octx(op,v) ;
}
/* end subroutine (buffer_octi) */

int buffer_octl(buffer *op,long v) noex {
	return buffer_octx(op,v) ;
}
/* end subroutine (buffer_octl) */

int buffer_octll(buffer *op,longlong v) noex {
	return buffer_octx(op,v) ;
}
/* end subroutine (buffer_octll) */

int buffer_octui(buffer *op,uint v) noex {
	return buffer_octx(op,v) ;
}
/* end subroutine (buffer_octui) */

int buffer_octul(buffer *op,ulong v) noex {
	return buffer_octx(op,v) ;
}
/* end subroutine (buffer_octul) */

int buffer_octull(buffer *op,ulonglong v) noex {
	return buffer_octx(op,v) ;
}
/* end subroutine (buffer_octull) */

int buffer_deci(buffer *op,int v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_deci) */

int buffer_decl(buffer *op,long v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decl) */

int buffer_decll(buffer *op,longlong v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decll) */

int buffer_decui(buffer *op,uint v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decui) */

int buffer_decul(buffer *op,ulong v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decul) */

int buffer_decull(buffer *op,ulonglong v) noex {
	return buffer_decx(op,v) ;
}
/* end subroutine (buffer_decull) */

int buffer_hexc(buffer *op,int v) noex {
	uint		uv = uint(v) ;
	return buffer_hexuc(op,uv) ;
}
/* end subroutine (buffer_hexc) */

int buffer_hexi(buffer *op,int v) noex {
	uint		uv = uint(v) ;
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexi) */

int buffer_hexl(buffer *op,long v) noex {
	ulong		uv = ulong(v) ;
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexl) */

int buffer_hexll(buffer *op,longlong v) noex {
	ulonglong	uv = ulonglong(v) ;
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexll) */

int buffer_hexuc(buffer *op,uint uv) noex {
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexc) */

int buffer_hexui(buffer *op,uint uv) noex {
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexui) */

int buffer_hexul(buffer *op,ulong uv) noex {
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexul) */

int buffer_hexull(buffer *op,ulonglong uv) noex {
	return buffer_hexx(op,uv) ;
}
/* end subroutine (buffer_hexull) */


