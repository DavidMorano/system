/* mkcmds SUPPORT */
/* lang=C20 ( <-MUST be "C-language") */

/* names string storage: purpose? */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"mkcmds.h"


const char	*mkcmds[] = {
	[mkcmd_minwordlen] = "minwordlen",
	[mkcmd_maxwordlen] = "maxwordlen",
	[mkcmd_eigenwords] = "eigenwords",
	[mkcmd_nkeys] = "nkeys",
	[mkcmd_tablen] = "tablen",
	[mkcmd_sdn] = "sdn",
	[mkcmd_sfn] = "sfn",
	[mkcmd_lang] = "lang",
	nullptr
} ;


