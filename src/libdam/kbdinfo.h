/* kbdinfo HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* keyboard-information database access */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	KBDINFO_INCLUDE
#define	KBDINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<keysymer.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<termcmd.h>		/* LIBUC */


#define	KBDINFO			struct kbdinfo_head
#define	KBDINFO_FL		struct kbdinfo_flags
#define	KBDINFO_ENT		struct kbdinfo_entry
#define	KBDINFO_CUR		struct kbdinfo_cursor
#define	KBDINFO_MAGIC		0x24182138

/* types (enums) */
enum kbdinfo_types {
	kbdinfot_reg,
	kbdinfot_esc,
	kbdinfot_csi,
	kbdinfot_dcs,
	kbdinfot_pf,
	kbdinfot_fkey,
	kbdinfot_overlast
} ; /* end enum (kbdinfo_types) */

/* types (defines) */
#define	KBDINFO_TREG		kbdinfot_reg
#define	KBDINFO_TESC		kbdinfot_esc
#define	KBDINFO_TCSI		kbdinfot_csi
#define	KBDINFO_TDCS		kbdinfot_dcs
#define	KBDINFO_TPF		kbdinfot_pf
#define	KBDINFO_TFKEY		kbdinfot_fkey
#define	KBDINFO_TOVERLAST	kbdinfot_overlast


struct kbdinfo_flags {
	uint		dummy:1 ;	/* new mail arrived */
} ; /* end struct */

struct kbdinfo_entry {
	char		*a ;		/* the memory allocation */
	cchar		*keyname ;	/* keysym */
	cchar		*istr ;
	cchar		*dstr ;
	short		*p ;		/* parameters */
	int		type ;		/* key type */
	int		name ;		/* key name */
	int		keynum ;	/* key number */
	int		nparams ;	/* number of paramters */
} ; /* end struct */

struct kbdinfo_cursor {
	int		i, j ;
} ; /* end struct */

struct kbdinfo_head {
	keysymer	*ksp ;
	vecobj		types[KBDINFO_TOVERLAST] ;
	KBDINFO_FL	fl ;
	uint		magval ;
} ; /* end struct */

typedef	KBDINFO			kbdinfo ;
typedef	KBDINFO_ENT		kbdinfo_ent ;
typedef	KBDINFO_CUR		kbdinfo_cur ;

EXTERNC_begin

extern int kbdinfo_open		(kbdinfo *,keysymer *,cchar *) noex ;
extern int kbdinfo_count	(kbdinfo *) noex ;
extern int kbdinfo_lookup	(kbdinfo *,char *,int,termcmd *) noex ;
extern int kbdinfo_curbegin	(kbdinfo *,kbdinfo_cur *) noex ;
extern int kbdinfo_curend	(kbdinfo *,kbdinfo_cur *) noex ;
extern int kbdinfo_curenum	(kbdinfo *,kbdinfo_cur *,kbdinfo_ent **) noex ;
extern int kbdinfo_close	(kbdinfo *) noex ;

EXTERNC_end


#endif /* KBDINFO_INCLUDE */


