/* main (METAS) */

/* meta strings */


#define	CF_DEBUGS	1		/* compile-time debugging */


/* revision history:

	= 2017-09-01, David A�D� Morano
	Written for fun!

*/

/* Copyright � 2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Meta-strings; see what they are on-line.


*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<climits>
#include	<cstring>
#include	<new>
#include	<initializer_list>
#include	<utility>
#include	<functional>
#include	<algorithm>
#include	<unordered_map>
#include	<array>
#include	<set>
#include	<vector>
#include	<list>
#include	<stack>
#include	<deque>
#include	<string>
#include	<fstream>
#include	<iostream>
#include	<ostream>
#include	<iomanip>
#include	<usystem.h>
#include	<ctdec.h>
#include	<localmisc.h>


/* local defines */

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	45
#endif

#define	VARDEBUGFNAME	"METAS_DEBUGFILE"


/* name spaces */

using namespace	std ;


/* typedefs */


/* external subroutines */

#if	CF_DEBUGS
extern "C" int	debugopen(cchar *) ;
extern "C" int	debugprintf(cchar *,...) ;
extern "C" int	debugclose() ;
extern "C" int	strlinelen(cchar *,cchar *,int) ;
#endif

extern "C" cchar	*getourenv(cchar **,cchar *) ;


/* local structures */


/* forward references */

static int	metas1(vector<int> &) ;
static int	metas2(vector<int> &) ;

static string	strvec(vector<int>) ;

static void	printres(vector<int> &,int) ;


/* local variables */

static vector<int>	cases[] = {
	{ 2, 2, 2 },
	{ 0 },
	{ 1, 1 },
	{ 1, 3, 2, 2, 3 },
	{ 2, 0, 2, 0, 3 },
	{ 2, 1, 0, 10, 11, 12, 13 },
	{ 2, 3, 6, 2, -1, 1, 1 }
} ;


/* exported subroutines */


/* ARGSUSED */
int main(int argc,cchar **argv,cchar **envv)
{
	const int	algos[] = { 1 } ;
	int		ex = 0 ;
	int		rs = SR_OK ;

#if	CF_DEBUGS
	{
	    cchar	*cp ;
	    if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL) {
	        rs = debugopen(cp) ;
	        debugprintf("main: starting DFD=%d\n",rs) ;
	    }
	}
#endif /* CF_DEBUGS */

	for (auto &a : cases) {
	    cout << "a=" << strvec(a) << endl ;
	        for (auto al : algos) {
	            rs = 0 ;
	            switch (al) {
	            case 1:
	                rs = metas1(a) ;
		        break ;
	            case 2:
	                rs = metas2(a) ;
		        break ;
	            } /* end switch */
		    printres(a,rs) ;
		    if (rs < 0) break ;
	        } /* end for (algo) */
	        cout << endl ;
	    if (rs < 0) break ;
	} /* end for (cases) */

#if	CF_DEBUGS
	debugprintf("main: ret rs=%d\n",rs) ;
	debugclose() ;
#endif

	if (rs < 0) ex = 1 ;
	return ex ;
}
/* end subroutine (main) */


/* local subroutines */


static int metas1(vector<int> &a)
{
	const int	n = a.size() ;
	int		f = FALSE ;
	int		p = 0 ;
	for (int i = 0 ; i < n ; i += 1) {
	    p += a[p] ;
	    p = (p % n) ; if (p < 0) p += n ;
	    if (p == 0) {
		f = (i == (n-1)) ; /* possible early exit on failure */
		if (!f) break ;
	    }
	} /* end for */
	return f ;
}
/* end subroutine (metas1) */


static int metas2(vector<int> &a)
{
	const int	n = a.size() ;
	int		f = FALSE ;
	{
	    vector<bool>	visited(n,false) ;
	    int			p = 0 ;
	    for (int i = 0 ; i < n ; i += 1) {
		if (visited[p]) break ;
		if (p > 0) visited[p] = true ;
	        p += a[p] ;
	        p = (p % n) ; if (p < 0) p += n ;
	        if (p == 0) {
		    f = (i == (n-1)) ; /* possible early exit on failure */
		    if (!f) break ;
	        }
	    } /* end for */
	} /* end block (vector-visited) */
	return f ;
}
/* end subroutine (metas2) */


static string strvec(vector<int> a) 
{
	string		res ;
	const int	dlen = DIGBUFLEN ;
	int		c = 0 ;
	char		dbuf[DIGBUFLEN+1] ;
	res += '{' ;
	for (auto &e:a) {
	    if (c++ > 0) res += ',' ;
	    ctdeci(dbuf,dlen,e) ;
	    res += dbuf ;
	}
	res += '}' ;
	return res ;
}
/* end subroutine (strvec) */


static void printres(vector<int> &a,int f)
{
	const char	*s = (f) ? "true " : "false" ;
	cout << s << " " << strvec(a) << endl ;
}
/* end subroutine (printres) */


