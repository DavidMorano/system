/* main (ANAS) */
/* lang=C++20 */

/* string anagrams */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-09-01, David A­D­ Morano
	Written for fun!

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        Given a string, we find all anagrams of it (and print them out).


*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<climits>
#include	<cstring>		/* for |strlen(3c)| */
#include	<new>
#include	<initializer_list>
#include	<utility>
#include	<functional>
#include	<algorithm>
#include	<unordered_map>
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
#include	<localmisc.h>


/* local defines */


/* imported namespaces */

using namespace	std ;


/* local typedefs */

typedef set<string>	res_t ;


/* external subroutines */

extern "C" cchar	*getourenv(cchar **,cchar *)  noex ;


/* local structures */


/* forward references */

static int	anas1(res_t *,cchar *)  ;
static int	anas2(res_t *,cchar *)  ;
static int	anas3(res_t *,cchar *)  ;

static void	printres(res_t *) ;


/* local variables */

static constexpr cchar	*cases[] = {
	"ab",
	"abc",
	"aab",
	"abcd",
	"accc",
	"abcdc",
	NULL
} ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv,mainv)  {
	res_t		res ;
	cint		algos[] = { 1, 2, 3 } ;
	int		ex = 0 ;
	int		rs = SR_OK ;

	for (auto sc : cases) {
	    if (sc != NULL) {
	        for (auto al : algos) {
	            rs = 0 ;
		    res.clear() ;
	            cout << "algo=" << al << " sc=" << sc << eol ;
	            switch (al) {
	            case 1:
	                rs = anas1(&res,sc) ;
		        break ;
	            case 2:
	                rs = anas2(&res,sc) ;
		        break ;
	            case 3:
	                rs = anas3(&res,sc) ;
		        break ;
	            } /* end switch */
	            printres(&res) ;
		    if (rs < 0) break ;
	        } /* end for (algo) */
	        cout << endl ;
	    } /* end if (non-null) */
	    if (rs < 0) break ;
	} /* end for (cases) */

	if (rs < 0) ex = 1 ;
	return ex ;
}
/* end subroutine (main) */


/* local subroutines */

struct anas1_char {
	int		ch ;
	int		max = 1 ;
	vector<int>	counts ;
	anas1_char(char _ch,int n)  : ch(_ch), counts(n,0) { } ;
} ;

typedef vector<anas1_char>	anas1map_t ;

struct anas1_head {
	anas1map_t	m ;
	string		src ;
	string		tmp ;
	res_t		*resp = NULL ;
	int		total = 0 ;
	int		n = 0 ;
	anas1_head() = default ;
	anas1_head(res_t *_rp,cchar *sp)  : src(sp), resp(_rp) {
	    n = src.length() ;
	    tmp.resize(n,'-') ;
	    for (auto ch : src) {
		loadchar(ch) ;
	    } /* end for */
	} ;
	void loadchar(char ch)  {
	    cint	wn = m.size() ;
	    bool	f = false ;
	    int		w{} ;
	    for (w = 0 ; w < wn ; w += 1) {
		f = (m[w].ch == ch) ;
		if (f) break ;
	    }
	    if (f) {
		m[w].max += 1 ;
	    } else {
		anas1_char	item(ch,n) ;
		m.push_back(item) ;
	    }
	} ;
	bool canadd(int i,int w)  {
	    bool	f = (m[w].counts[i] < m[w].max) ;
	    return f ;
	} ;
	void inc(int i,int w)  {
	    m[w].counts[i] += 1 ;
	} ;
	void enter(int i)  {
	    cint	n = m.size() ;
	    if (i > 0) {
		for (int w = 0 ; w < n ; w += 1) {
		    m[w].counts[i] = m[w].counts[i-1] ;
		}
	    } else {
		for (int w = 0 ; w < n ; w += 1) {
		    m[w].counts[i] = 0 ;
		}
	    }
	} ;
	bool mkstr(int i,int w)  {
	    bool	f = false ;
	    if (i < n) {
	        enter(i) ;
		    if (canadd(i,w)) {	/* restriction */
		        tmp[i] = m[w].ch ;
		        inc(i,w) ;
			f = true ;
		        if ((i+1) == n) {
	    	            resp->insert(tmp) ;
			    total += 1 ;
		        } else {
	                    recurse(i+1) ;
		        }
		    } /* end if (hit) */
	    } /* end if (possible) */
	    return f ;
	} ; /* end method (mkstr) */
	int num() const  {
	    return total ;
	} ;
	void recurse(int d)  {
	    cint	n = m.size() ;
	    for (int j = 0 ; j < n ; j += 1) {
    	        mkstr(d,j) ;
	    }
	} ;
} ; /* end struct (anas1_head) */

static int anas1(res_t *resp,cchar *sc)  {
	cint		n = strlen(sc) ;
	int		rs = SR_OK ;
	if (n > 0) {
	    anas1_head	worker(resp,sc) ;
	    worker.recurse(0) ;
	    rs = worker.num() ;
	    cout << "num=" << rs << endl ;
  	} /* end if (valid) */
	return rs ;
}
/* end subroutine (anas1) */

struct anas2_item {
	int		i = 0 ; /* index */
	int		w = 0 ; /* which-type */
	anas2_item()  { } ;
	anas2_item(int _i,int _w)  : i(_i), w(_w) { } ;
	anas2_item &operator = (const anas2_item &wi) = default ;
	/* do not need 'move' constructor or assignment (too simple) */
	void setindex(int ai)  {
	    i = ai ;
	} ;
	void setwhich(int aw)  {
	    w = aw ;
	} ;
} ;

struct anas2_char {
	int		ch ;
	int		max = 1 ;
	vector<int>	counts ;
	anas2_char(char _ch,int n)  : ch(_ch), counts(n,0) { } ;
} ;

typedef vector<anas2_char>	anas2map_t ;
typedef stack<anas2_item>	anas2stack_t ;

struct anas2_head {
	anas2stack_t	work ;
	anas2map_t	m ;
    	anas2_item	wi ;
	string		src ;
	string		tmp ;
	res_t		*resp = NULL ;
	int		total = 0 ;
	int		n = 0 ;
	anas2_head() = default ;
	anas2_head(res_t *_rp,cchar *sp) : src(sp), resp(_rp) {
	    n = src.length() ;
	    tmp.resize(n,'-') ;
	    for (auto ch : src) {
		loadchar(ch) ;
	    } /* end for */
	} ;
	void loadchar(char ch) {
	    cint	wn = m.size() ;
	    bool	f = false ;
	    int		w{} ;
	    for (w = 0 ; w < wn ; w += 1) {
		f = (m[w].ch == ch) ;
		if (f) break ;
	    }
	    if (f) {
		m[w].max += 1 ;
	    } else {
		anas2_char	item(ch,n) ;
		m.push_back(item) ;
	    }
	} ;
	bool canadd(int i,int w) {
	    bool	f = (m[w].counts[i] < m[w].max) ;
	    return f ;
	} ;
	void inc(int i,int w) {
	    m[w].counts[i] += 1 ;
	} ;
	void enter(int i) {
	    cint	n = m.size() ;
	    if (i > 0) {
		for (int w = 0 ; w < n ; w += 1) {
		    m[w].counts[i] = m[w].counts[i-1] ;
		}
	    } else {
		for (int w = 0 ; w < n ; w += 1) {
		    m[w].counts[i] = 0 ;
		}
	    }
	} ;
	int mkstr() {
	    anas2_item	wi ;
	    int		i, w ;
    	    while (! work.empty()) {
      		wi = work.top() ;
      		work.pop() ;
		i = wi.i ;
		w = wi.w ;
	        if (i < n) {
	            enter(i) ;
		    if (canadd(i,w)) {	/* restriction */
		        tmp[i] = m[w].ch ;
		        inc(i,w) ;
		        if ((i+1) == n) {
	    	            resp->insert(tmp) ;
			    total += 1 ;
		        } else {
	                    pushall(i+1) ;
		        }
		    } /* end if (hit) */
	        } /* end if (possible) */
	    } /* end while (stack) */
	    return total ;
	} ; /* end method (mkstr) */
	int num() const {
	    return total ;
	} ;
	void push(int i,int w) {
	    anas2_item	wi(i,w) ;
	    work.push(wi) ;
	} ;
	void pushall(int i) {
	    cint	n = m.size() ;
	    for (int w = 0 ; w < n ; w += 1) {
    	        push(i,w) ;
	    }
	} ;
	int mkstrs() {
	    pushall(0) ;
	    return mkstr() ;
	} ;
} ; /* end struct (anas2_head) */

static int anas2(res_t *resp,cchar *sc)  {
	cint		n = strlen(sc) ;
	int		rs = SR_OK ;
	if (n > 0) {
	    anas2_head	worker(resp,sc) ;
	    rs = worker.mkstrs() ;
	    cout << "num=" << worker.num() << endl ;
  	} /* end if (valid) */
	return rs ;
}
/* end subroutine (anas2) */

static int anas3(res_t *resp,cchar *sc)  {
	cint		n = strlen(sc) ;
	int		rs = SR_OK ;
	if (n > 0) {
	    string	s(sc) ;
	    {
		string::iterator	end = s.end() ;
		string::iterator	it = s.begin() ;
		int			c = 1 ;
	        resp->insert(s) ;
		while (prev_permutation(it,end)) {
	            resp->insert(s) ;
		    c += 1 ;
		}
		s = sc ; end = s.end() ; it = s.begin() ;
	        while (next_permutation(it,end)) {
	            resp->insert(s) ;
		    c += 1 ;
		}
	        cout << "num=" << c << endl ;
		rs = c ;
	    }
  	} /* end if (valid) */
	return rs ;
}
/* end subroutine (anas3) */

static void printres(res_t *rp)  {
	res_t::iterator	end = rp->end() ;
	res_t::iterator	it = rp->begin() ;
	while (it != end) {
	    cout << (*it) << endl ;
	    it++ ;
	}
}
/* end subroutine (printres) */


