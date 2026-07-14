/* filetypes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* define file types according to the S_IFMT field of |mode_t| */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"filetypes.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

consteval void filetyper::mkname() noex {
	name[filetype_unassigned0]	= "un0" ;
	name[filetype_fifo]		= "fifo" ;
	name[filetype_chr]		= "chr" ;
	name[filetype_unassigned3]	= "un3" ;
	name[filetype_dir]		= "dir" ;
	name[filetype_name]		= "mame" ;
	name[filetype_blk]		= "blk" ;
	name[filetype_unassigned7]	= "un7" ;
	name[filetype_reg]		= "reg" ;
	name[filetype_unassigned9]	= "un9" ;
	name[filetype_lnk]		= "lnk" ;
	name[filetype_unassigned11]	= "un11" ;
	name[filetype_sock]		= "sock" ;
	name[filetype_door]		= "door" ;
	name[filetype_wht]		= "wht" ;
	name[filetype_unassigned15]	= "un15" ;
	name[filetype_overlast]		= nullptr ;
} /* end method (filetyper:mkname) */

consteval filetyper::filetyper() noex {
    	mkname() ;
} /* end ctor (filetyper) */

constexpr filetyper	filetype_data ;


/* exported subroutines */


/* local subroutines */


