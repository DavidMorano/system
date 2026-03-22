/* filetypes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* define file types according to the S_IFMT field of |mode_t| */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

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

constexpr cpcchar	filetypes_names[] =  {
	[filetype_unassigned0]	= "un0",
	[filetype_fifo]		= "fifo",
	[filetype_chr]		= "chr",
	[filetype_unassigned3]	= "un3",
	[filetype_dir]		= "dir",
	[filetype_name]		= "mame",
	[filetype_blk]		= "blk",
	[filetype_unassigned7]	= "un7",
	[filetype_reg]		= "reg",
	[filetype_unassigned9]	= "un9",
	[filetype_lnk]		= "lnk",
	[filetype_unassigned11]	= "un11",
	[filetype_sock]		= "sock",
	[filetype_door]		= "door",
	[filetype_wht]		= "wht",
	[filetype_unassigned15]	= "un15",
	[filetype_overlast]	= nullptr
} ; /* end enum (filetypes_names) */


/* exported subroutines */


/* local subroutines */


