/* sysauxinfonames HEADER */
/* lang=C20 */

/* UNIX� system-auxillary-information */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 2000 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This file contains enumerations that represent individual
	system-auxillary-information items that can be retrieved
	from any particular operating system.  These enumerations
	are not used directly by any public interface, but rather
	provide a consistent set of numerical values for use in
	defining preprocessir symbols for use in the public interface
	of |uc_sysauxinfo(3uc)|. For each operating system that is
	supported, the code that supports any particular operaring
	system defines some preprocessor symbols that represent
	those items that are supported by the operaring system being
	coded.  The practice is that any preprocessor symbol that
	is defined, it is defined to be equal to the enumberation
	provided below with the same name.

	For example, if the code to support a particular operating
	system wants to provide the system-auxilliary-information
	(SAI) item by the name of 'SAI_ARCHITECTURE', it would
	define that proprocessor symbol to be equal to the enumberation
	'sysauxinfo_architecture'. Strictly this equivalence between
	the value of a preprocessor symbol and the enumerations
	below is not neccessary, it provides the added benefit that
	all preprocessor symbols for all operating systems will
	have the same value.

*******************************************************************************/

#ifndef	SYSAUXINFONAMES_INCLUDE
#define	SYSAUXINFONAMES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


enum sysauxinfos {
	sysauxinfo_architecture,
	sysauxinfo_platform,
	sysauxinfo_hwprovider,
	sysauxinfo_hwserial,
	sysauxinfo_srpcdomain,
	sysauxinfo_overlast
} ;


#endif /* SYSAUXINFONAMES_INCLUDE */


