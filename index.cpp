#include <vector>
#include <map>
#include <algorithm>
#include <set>
#include <cstdlib>
#include "common.h"

using std::vector;
using std::map;
using std::sort;
using std::set;

static vector<int> inverse( const vector<int>& fn ) {
    // Computes the inverse of a permutation.
    // [INPUT ] fn: a permutation of {0,...,n-1}
    // [OUTPUT] return: the inverse of fn
    vector<int> fn_inv(fn.size());
    for( int i = 0; i < fn.size(); ++i ) {
        fn_inv[fn[i]] = i;
    }
    return fn_inv;
}

static vector<int> LF( const vector<int>& sa, const vector<int>& sa_inv, int delta ) {
    // Computes LF() by Eqs (6) and (7).
    int n_1 = sa.size();
    vector<int> lf(n_1);
    for( int i = 0; i < n_1; ++i ) {
        lf[i] = sa_inv[ ( sa[i] - delta + n_1 ) % n_1 ];
    }
    return lf;
}

static vector<int> F( const vector<int>& sa, const vector<int>& d ) {
    // Computes F() by Eqs (10) and (11)
    vector<int> f(sa.size());
    f[0] = -1;
    for( int i = 1; i < sa.size(); ++i ) {
        f[i] = d[sa[i]];
    }
    return f;
}

static vector<int> L( const vector<int>& lf, const vector<int>& f ) {
    // Computes L() by Eqs (12) and (13)
    vector<int> l(f.size());
    for( int i = 0; i < f.size(); ++i ) {
        l[i] = f[lf[i]];
    }
    return l;
}

void build( const vector<int>& T, const vector<int>& SA_even, const vector<int>& SA_odd, vector<int>& F_even, vector<int>& L_even, vector<int>& F_odd, vector<int>& L_odd, vector<int>& LF_eo ) {
    int N = T.size();

    vector<int> D_even, D_odd;
    vector<int> PS_T = PS(T, D_even, D_odd);

    vector<int> SA_even_inv = inverse(SA_even);
    vector<int> SA_odd_inv = inverse(SA_odd);

    vector<int> LF_oe;

    LF_eo = LF( SA_even, SA_odd_inv , 1 );
    LF_oe = LF( SA_odd , SA_even_inv, 0 );

    F_even = F( SA_even, D_even );
    F_odd  = F( SA_odd , D_odd  );

    L_even = L( LF_eo  , F_odd  );
    L_odd  = L( LF_oe  , F_even );
}
