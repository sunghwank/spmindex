#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "common.h"

using namespace std;

ostream& operator << ( ostream& out, const vector<int>& x ) {
    for( int i = 0; i < x.size(); ++i ) {
        if( i > 0 ) out << ' ';
        out << x[i];
    }
    return out;
}

struct suffix_sort {
    suffix_sort( const vector<int>& _PS_T ) {
        n = _PS_T.size();
        Encoded_PS_T.resize(n);
        for( int i = n-1; i >= 0; --i ) {
            Encoded_PS_T[i] = n; // empty set is considered to be largest
            if( _PS_T[i] != n ) Encoded_PS_T[i+_PS_T[i]] = _PS_T[i]; // incoming pointer length Eq (2)
        }
    }
    int n;
    vector<int> Encoded_PS_T;

    bool operator () ( int i, int j ) {
        if( i == j ) return false;

        int k = 0;
        while(1) {
            if( j+k >= n ) return false;
            if( i+k >= n ) return true;

            // Encoded characters at position k of suffixes i and j
            int e_i = Encoded_PS_T[i+k]; e_i=e_i<=k?e_i:n;
            int e_j = Encoded_PS_T[j+k]; e_j=e_j<=k?e_j:n;

            if( e_i < e_j ) return true;
            if( e_i > e_j ) return false;

            ++k;
        }
    }
};

void compute_suffix_array( const vector<int>& PS_T, vector<int>& SA_even, vector<int>& SA_odd ) {
    // Sorts the encoded suffixes.
    int n = PS_T.size()/2;
    SA_even.resize(n+1);
    SA_odd .resize(n+1);
    for( int i = 0; i <= n; ++i ) {
        SA_even[i] = 2*i;
        SA_odd [i] = 2*i+1;
    }
    suffix_sort comparator( PS_T );
    sort( SA_even.begin(), SA_even.end(), comparator );
    sort( SA_odd .begin(), SA_odd .end(), comparator );

    for( int i = 0; i <= n; ++i ) {
        SA_even[i] /= 2;
        SA_odd [i] /= 2;
    }
}

vector<int> random_vector( int n, int sigma ) {
    vector<int> ret(n,0);
    for( int i = 0; i < n; ++i ) {
        ret[i] = rand()%sigma;
    }
    return ret;
}

int main( int argc, char **argv ) {
    int n = atoi(argv[1]);
    int sigma = atoi(argv[2]);
    int seed = 59481239;
    if( argc > 3 ) seed = atoi(argv[3]);
    srand(seed);

    vector<int> T = random_vector( n, sigma );
    vector<int> D_even;
    vector<int> D_odd;

    vector<int> PS_T = PS(T, D_even, D_odd);

    vector<int> SA_even;
    vector<int> SA_odd;

    compute_suffix_array( PS_T, SA_even, SA_odd );

    cout << seed << '\t' << n << '\t' << sigma << endl;
    cout << T << endl;
    cout << SA_even << endl;
    cout << SA_odd << endl;

    return 0;
}
