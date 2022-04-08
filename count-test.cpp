#include "index.h"
#include "avl.h"
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>

using namespace std;

ostream& operator << ( ostream& out, const vector<int>& x ) {
    for( int i = 0; i < x.size(); ++i ) {
        if( i > 0 ) out << ' ';
        out << x[i];
    }
    return out;
}

void load( istream& in, vector<int>& T, vector<int>& SA_even, vector<int>& SA_odd ) {
    int seed;
    int n;
    int sigma;
    in >> seed >> n >> sigma;
    T.resize(n);
    SA_even.resize(n+1);
    SA_odd .resize(n+1);
    for( int i = 0; i <  n; ++i ) { in >> T[i];       }
    for( int i = 0; i <= n; ++i ) { in >> SA_even[i]; }
    for( int i = 0; i <= n; ++i ) { in >> SA_odd[i];  }
}

vector<int> generate_isomorphic_string( const vector<int>& x ) {
    set<int> group_set;
    for( int i = 0; i < x.size(); ++i ) {
        int ch = x[i];
        group_set.insert( ch/2 );
    }
    vector<int> group( group_set.begin(), group_set.end() );
    vector<int> group_sorted = group;
    random_shuffle( group.begin(), group.end() );
    sort( group_sorted.begin(), group_sorted.end() );
    map<int,int> gmap;
    for( int i = 0; i < group.size(); ++i ) {
        gmap[ group_sorted[i] ] = group[i];
    }
    map<int,int> ingroup_shuffled;
    for( int i = 0; i < group.size(); ++i ) {
        ingroup_shuffled[ group_sorted[i] ] = rand()%2;
    }

    vector<int> ret;
    ret.reserve(x.size());
    for( int i = 0; i < x.size(); ++i ) {
        int c = x[i];
        int g = (c/2);
        int r = (c%2);

        int c_new = ((gmap.at( g ) << 1) | (r^ingroup_shuffled[g]));
        ret.push_back( c_new );
    }

    return ret;
}

vector<int> extract_random_substring( const vector<int>& T, int m ) {
    int N = T.size();
    int k = rand()%(N-m);
    vector<int> P( T.begin()+k, T.begin()+k+m );
    P = generate_isomorphic_string( P );
    return P;
}

bool match( const vector<int>& T, const vector<int>& P, int i = 0 ) {
    int n = T.size();
    int m = P.size();
    if( i < 0 || n < i+m ) return false;

    map<int,int> f;
    map<int,int> f_inv;
    for( int j = 0; j < m; ++j ) {
        int ch_t = T[i+j];
        int ch_p = P[j];  

        map<int,int>::const_iterator iter_ch = f.find( ch_p );
        if( iter_ch != f.end() ) {
            if( iter_ch->second != ch_t ) return false;
        } else {
            if( f_inv.find( ch_t ) != f_inv.end() ) return false;
            f[ ch_p ] = ch_t;
            f[ ch_p ^ 0x1 ] = ( ch_t ^ 0x1 );
            f_inv[ ch_t ] = ch_p;
            f_inv[ ch_t ^ 0x1 ] = ( ch_p ^ 0x1 );
        }
    }
    return true;
}

int main( void ) {
    srand( 10858814 );
    ifstream in( "example.suf" );
    vector<int> T, SA_even, SA_odd;
    load( in, T, SA_even, SA_odd );

    cout << "Index built" << endl;
    Index< sdsl::wt_int<sdsl::rrr_vector<31> >, sdsl::rrr_vector<31>, sdsl::rmq_succinct_sct<false> > I;
    I.construct( T, SA_even, SA_odd );

    for( int i = 0; i < 100; ++i ) {
        vector<int> P = extract_random_substring( T, 5 );
        int count = 0;
        for( int j = 0; j < T.size(); ++j ) {
            if( match( T, P, j ) ) {
                ++count;
            }
        }
        if( count != I.count( P ) ) {
            cout << "ERROR at " << P << endl;
            return 1;
        }
    }

    cout << "Test complete" << endl;
    return 0;
}
