/* minmax */

/* minimum and maximum */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_STRENGTH	0		/* use strength reduction */


/* revision history:

	= 1998-07-01, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

        These subroutines calculate the minimum or the maximum (respectively) of
        two (integer) values.

	Synopsis:

	int min(int a,int b)

	Arguments:

	a		value-1
	b		value-2

	Returns:

	-		the minimum of the two values


*******************************************************************************/


#include	<envstandards.h>

#include	<sys/types.h>

#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


#if	CF_STRENGTH

int min(int a,int b)
{
	cint	sbit = ((szof(int)*8)-1) ;
	cint	d = (a-b) ;
	cint	sel = (!(((a-b) >> sbit)&1)) ;
	return (a - (sel*d)) ;
}

int max(int a,int b)
{
	cint	sbit = ((szof(int)*8)-1) ;
	cint	d = (a-b) ;
	cint	sel = (((a-b) >> sbit)&1) ;
	return (a - (sel*d)) ;
}

long lmin(long a,long b)
{
	cint	sbit = ((szof(long)*8)-1) ;
	clong	d = (a-b) ;
	clong	sel = (!(((a-b) >> sbit)&1)) ;
	return (a - (sel*d)) ;
}

long lmax(long a,long b)
{
	cint	sbit = ((szof(long)*8)-1) ;
	clong	d = (a-b) ;
	clong	sel = (((a-b) >> sbit)&1) ;
	return (a - (sel*d)) ;
}

longlong llmin(longlong a,longlong b)
{
	cint	sbit = ((szof(longlong)*8)-1) ;
	clonglong	d = (a-b) ;
	clonglong	sel = (!(((a-b) >> sbit)&1)) ;
	return (a - (sel*d)) ;
}

longlong llmax(longlong a,longlong b)
{
	cint	sbit = ((szof(longlong)*8)-1) ;
	clonglong	d = (a-b) ;
	clonglong	sel = (((a-b) >> sbit)&1) ;
	return (a - (sel*d)) ;
}


#else /* CF_STRENGTH */

int min(int a,int b)
{
	int	v = a ;
	if (b < v) v = b ;
	return v ;
}
/* end subroutine (min) */


int max(int a,int b)
{
	int	v = a ;
	if (b > v) v = b ;
	return v ;
}
/* end subroutine (max) */


long lmin(long a,long b)
{
	long	v = a ;
	if (b < v) v = b ;
	return v ;
}
/* end subroutine (lmin) */


long lmax(long a,long b)
{
	long	v = a ;
	if (b > v) v = b ;
	return v ;
}
/* end subroutine (lmax) */


LONG llmin(LONG a,LONG b)
{
	LONG	v = a ;
	if (b < v) v = b ;
	return v ;
}
/* end subroutine (llmin) */


LONG llmax(LONG a,LONG b)
{
	LONG	v = a ;
	if (b > v) v = b ;
	return v ;
}
/* end subroutine (llmax) */

#endif /* CF_STRENGTH */


