/* vecstrx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* vector-string container (Vector-String) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-13, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vecstrx

	Description:
	This object implements a vector of strings.  This are C-styled
	strings (of course, and as it should be).

*******************************************************************************/

#ifndef	VECSTRX_INCLUDE
#define	VECSTRX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

typedef int (*vecstrx_f)(cchar **,cchar **) noex ;

EXTERNC_end

struct vecstrx {
	vecstrx &operator = (const vecstrx &) = delete ;
	virtual int add(cchar *,int = -1) noex = 0 ;
	virtual int adduniq(cchar *,int = -1) noex = 0 ;
	virtual int addkeyval(cchar *,int,cchar *,int = -1) noex = 0 ;
	virtual int insert(int,cchar *,int = -1) noex = 0 ;
	virtual int get(int,cchar **) noex = 0 ;
	virtual int getvec(mainv *) noex = 0 ;
	virtual int finder(cchar *,vecstrx_f,cchar **) noex = 0 ;
	virtual int del(int = -1) noex = 0 ;
	virtual int sort(vecstrx_f = nullptr) noex = 0 ;
	int addpathclean(cchar *,int) noex ;
	int addpath(cchar *,int) noex ;
	int addcspath() noex ;
	int adds(cchar *,int) noex ;
	int addsyms(cchar *,mainv) noex ;
	int addsubdirs(cchar *) noex ;
	int adduniqs(cchar *,int) noex ;
	int envadd(cchar *,cchar *,int) noex ;
	int envadds(cchar *,int) noex ;
	int envset(cchar *,cchar *,int) noex ;
	int envget(cchar *,cchar **) noex ;
	int envfile(cchar *) noex ;
	int foilcmp(vecstrx *) noex ;
	int loadfile(int,cchar *) noex ;
	int loadgrusers(gid_t) noex ;
	int loadpjusers(cchar *) noex ;
	int srvargs(cchar *) noex ;
	int svcargs(int *,cchar *) noex ;
} ; /* end struct (vecstrx) */


#endif /* __cplusplus */
#endif /* VECSTRX_INCLUDE */


