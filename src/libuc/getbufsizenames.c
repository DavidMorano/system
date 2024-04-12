/* getbufsizenames SUPPORT */
/* lang=C23 (MUST-BE C-language) */

/* enumerate the various system buffer-size names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the enumerations for the various system
	buffer-size names.

	This code module MUST-BE written in the C-language.
	The reason is because C++ does NOT support the language feature 
	used below.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	"getbufsizenames.h"


cchar *getbufsizenames[] = {
	[getbufsize_ma] = "MA",		/* max-args */
	[getbufsize_mn] = "MN",		/* max-name */
	[getbufsize_mp] = "MP",		/* max-path */
	[getbufsize_ml] = "ML",		/* max-line */
	[getbufsize_nn] = "NN",		/* node-name */
	[getbufsize_hn] = "HN",		/* host-name */
	[getbufsize_un] = "UN",		/* user-name */
	[getbufsize_gn] = "GN",		/* group-name */
	[getbufsize_pn] = "PN",		/* project-name */
	[getbufsize_pw] = "PW",		/* passwd */
	[getbufsize_sp] = "SP",		/* shadow */
	[getbufsize_ua] = "UA",		/* user-attr */
	[getbufsize_gr] = "GR",		/* group */
	[getbufsize_pj] = "PJ",		/* project */
	[getbufsize_pr] = "PR",		/* protocol-ent */
	[getbufsize_nw] = "NW",		/* network-ent */
	[getbufsize_ho] = "HO",		/* host-ent */
	[getbufsize_sv] = "SV",		/* service-ent */
	[getbufsize_zn] = "ZN",		/* zone-name (from Zone-Info) */
	nullptr
} ; /* end array (getbufsizenames) */


