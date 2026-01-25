/* bufsizenames SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++23 (MUST-BE C-language; conformance reviewed to C23) */

/* enumerate the various system buffer-size names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bufsizenames

	Description:
	This file contains the enumerations for the various system
	buffer-size names.  This code module MUST-BE written in the
	C-language.  The reason is because C++ does NOT (officially)
	support the language feature (according to its ISO standard)
	used below.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

#include	"bufsizenames.h"


cchar *bufsizs[] = {
	[bufsize_ps]		= "PS",		/* page-size */
	[bufsize_mi]		= "MI",		/* max-pid */
	[bufsize_ma]		= "MA",		/* max-args */
	[bufsize_mn]		= "MN",		/* max-name */
	[bufsize_mp]		= "MP",		/* max-path */
	[bufsize_ml]		= "ML",		/* max-line */
	[bufsize_mm]		= "MM",		/* max-message */
	[bufsize_nn]		= "NN",		/* node-name */
	[bufsize_hn]		= "HN",		/* host-name */
	[bufsize_un]		= "UN",		/* user-name */
	[bufsize_gn]		= "GN",		/* group-name */
	[bufsize_pn]		= "PN",		/* project-name */
	[bufsize_pw]		= "PW",		/* entry-passwd */
	[bufsize_sp]		= "SP",		/* entry-shadow */
	[bufsize_ua]		= "UA",		/* entry-userattr */
	[bufsize_gr]		= "GR",		/* entry-group */
	[bufsize_pj]		= "PJ",		/* entry-project */
	[bufsize_pr]		= "PR",		/* entry-protocol */
	[bufsize_nw]		= "NW",		/* entry-network */
	[bufsize_ho]		= "HO",		/* entry-host */
	[bufsize_sv]		= "SV",		/* entry-servicet */
	[bufsize_fs]		= "FS",		/* filesystem-type */
	[bufsize_sn]		= "SN",		/* symbol-name */
	[bufsize_zn]		= "ZN",		/* time-zone-name abbr */
	[bufsize_zi]		= "ZI",		/* zone-info-name */
	[bufsize_mailaddr]	= "MAILADDR",	/* mail-address */
	[bufsize_overlast]	= nullptr
} ; /* end array (bufsizes) */


