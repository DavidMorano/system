/* usupport HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORT_INCLUDE
#define	USUPPORT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<time.h>		/* |time_t| */
#include	<stddef.h>
#include	<stdlib.h>
#include	<stdint.h>
#include	<string.h>		/* |memset(3c)| + |memcpy(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usys.h>		/* <- auxillary OS support */
#include	<utimeout.h>
#include	<ulogerror.h>
#include	<usysutility.hh>
#include	<timecount.hh>
#include	<filetypes.h>
#include	<aflag.hh>
#include	<stdintx.h>
#include	<intx.h>

#include	<vstrorders.h>

#include	<usupport_ctdec.hh>
#include	<usupport_cfdec.hh>
#include	<usupport_cfdect.hh>
#include	<usupport_cfhex.hh>
#include	<usupport_cfinet.hh>
#include	<usupport_getsign.hh>
#include	<usupport_hasnot.hh>
#include	<usupport_itimer.hh>
#include	<usupport_matkeystr.hh>
#include	<usupport_matostr.hh>
#include	<usupport_matstr.hh>
#include	<usupport_nleadkeystr.hh>
#include	<usupport_nleadstr.hh>
#include	<usupport_prime.h>
#include	<usupport_rmx.hh>
#include	<usupport_sichar.hh>
#include	<usupport_sispan.hh>
#include	<usupport_siwhtx.hh>
#include	<usupport_sixchr.hh>
#include	<usupport_sncpyx.hh>
#include	<usupport_snwcpy.hh>
#include	<usupport_strdcpyx.hh>
#include	<usupport_strdcpyxw.hh>
#include	<usupport_strkeycmp.hh>
#include	<usupport_strncmp.hh>
#include	<usupport_strncpyxc.hh>
#include	<usupport_strnkeycmp.hh>
#include	<usupport_strnchar.hh>
#include	<usupport_strnset.hh>
#include	<usupport_strnwht.hh>
#include	<usupport_strnxchr.hh>
#include	<usupport_strnxbrk.hh>
#include	<usupport_strwcmp.hh>
#include	<usupport_strwcpy.hh>
#include	<usupport_strxxxxcmp.hh>
#include	<usupport_toxc.hh>
#include	<usupport_ustrftime.hh>
#include	<usupport_vstrcmp.hh>
#include	<usupport_vstrkeycmp.hh>


#endif /* USUPPORT_INCLUDE */


