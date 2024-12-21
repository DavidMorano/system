/* mktxtindexcmds HEADER */
/* lang=C20 ( <- MUST be C-language) */

/* these enumerations are commands used in text indexing operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mktxtindexcmds

	Description:
	This module contains the c-string names of the various
	text indexing command operations.

	Note:
	This module MUST be written in the C-language, because C++ 
	if brain damaged due to its inability to initialize arrays
	out of order (or by member identification at all).

*******************************************************************************/

#ifndef	MKTXTINDEXCMDS_INCLUDE
#define	MKTXTINDEXCMDS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>


const cpcchar	mktxtindexcmds[] = {
	"minwordlen",
	"maxwordlen",
	"eigenwords",
	"nkeys",
	"tablen",
	"sdn",
	"sfn",
	"lang",
	nullptr
} ; /* end if (mktxtindexcmds) */


#endif /* MKTXINDEXCMDS_INCLUDE */


