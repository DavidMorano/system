/* filemagic HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* this object helps manage the magic information in (DB) files */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FILEMAGIC_INCLUDE
#define	FILEMAGIC_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	FILEMAGIC_MAGSZ		16	/* file-magic buffer size */
#define	FILEMAGIC_INFOSZ	4	/* file-magic buffer size */
#define	FILEMAGIC_SZ		(FILEMAGIC_MAGSZ + FILEMAGIC_INFOSZ)


#endif /* __cplusplus */
#endif /* FILEMAGIC_INCLUDE */


