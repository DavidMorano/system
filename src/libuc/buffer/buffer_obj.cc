/* buffer_obj SUPPORT */
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
#include	<stdintx.h>
#include	<format.h>
#include	<strwcpy.h>
#include	<ctbin.h>
#include	<ctoct.h>
#include	<ctdec.h>
#include	<cthex.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"buffer.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* private subroutines */

int buffer::strcompact(cchar *sp,int sl) noex {
	return buffer_strcompact(this,sp,sl) ;
}

int buffer::stropaque(cchar *sp,int sl) noex {
	return buffer_stropaque(this,sp,sl) ;
}

int buffer::strquote(cchar *sp,int sl) noex {
	return buffer_strquote(this,sp,sl) ;
}

int buffer::chrs(int ch,int n) noex {
	return buffer_chrs(this,ch,n) ;
}

int buffer::blanks(int n) noex {
	return buffer_blanks(this,n) ;
}

int buffer::vprintf(cchar *fmt,va_list ap) noex {
    	return buffer_vprintf(this,fmt,ap) ;
}

int buffer::printf(cchar *fmt,...) noex {
    	va_list		ap ;
	int		rs = SR_FAULT ;
	if (fmt) ylikely {
	    va_begin(ap,fmt) ;
	    rs = buffer_vprintf(this,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
} /* end method (buffer::printf) */

buffer::operator int () noex {
	return len ;
} /* end method (buffer::operator) */


