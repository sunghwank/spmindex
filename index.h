#include <sdsl/bit_vectors.hpp>
#include <sdsl/rrr_vector.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/rmq_support.hpp>
#include <sdsl/rmq_succinct_sct.hpp>
#include <sdsl/io.hpp>
#include <sdsl/bit_vectors.hpp>
#include <cassert>
#include <iostream>
#include "avl.h"

void build( const std::vector<int>& T, const std::vector<int>& SA_even, const std::vector<int>& SA_odd,
            std::vector<int>& F_even, std::vector<int>& L_even, std::vector<int>& F_odd, std::vector<int>& L_odd, std::vector<int>& LF_eo );

template <class WT, class BV, class RMQ>
class Index {
    public:

    void construct( const std::vector<int>& T, const std::vector<int>& SA_even, const std::vector<int>& SA_odd ) {
        std::vector<int> F_even;
        std::vector<int> L_even;
        std::vector<int> F_odd;
        std::vector<int> L_odd;
        std::vector<int> LF_eo_;

        // Assume 0 <= T[i] < Sigma
        Sigma = 0;
        for( int i = 0; i < T.size(); ++i ) {
            Sigma = T[i]>Sigma?T[i]:Sigma;
        }
        ++Sigma;

        // Compute related information
        build( T, SA_even, SA_odd, F_even, L_even, F_odd, L_odd, LF_eo_ ); 

        // build wavlet trees, rank/select dictionaries for bitvectors, RMQ
        N = F_even.size()-1;
        sdsl::int_vector<> iL(N+1,0);
        sdsl::int_vector<> iF(N+1,0);
        for( int i = 0; i <= N; ++i ) { iL[i] = L_even[i]+1; }
        for( int i = 0; i <= N; ++i ) { iF[i] = F_odd [i]+1; }

        sdsl::construct_im( WT_L_even, iL );
        sdsl::construct_im( WT_F_odd , iF );

        assert( L_odd[0] == -1 );
        sdsl::bit_vector bL(N,0);
        for( int i = 0; i < N; ++i ) { bL[i] = L_odd[i+1]; }
        BV_L_odd = BV(bL);
        Rank1_BV_L_odd = (typename BV::rank_1_type)(&BV_L_odd);

        assert( F_even[0] == -1 );
        sdsl::bit_vector bF(N,0);
        for( int i = 0; i < N; ++i ) { bF[i] = F_even[i+1]; }
        BV_F_even = BV(bF);
        Select0_BV_F_even = (typename BV::select_0_type)(&BV_F_even);
        Select1_BV_F_even = (typename BV::select_1_type)(&BV_F_even);
        RMQ_eo = RMQ(&LF_eo_);
    }

    size_t size_in_bytes( void ) const {
        return sdsl::size_in_bytes( WT_L_even )
             + sdsl::size_in_bytes( WT_F_odd  )
             + sdsl::size_in_bytes( RMQ_eo    )
             + sdsl::size_in_bytes( BV_L_odd  )
             + sdsl::size_in_bytes( BV_F_even )
             + sdsl::size_in_bytes( Rank1_BV_L_odd  )
             + sdsl::size_in_bytes( Select0_BV_F_even )
             + sdsl::size_in_bytes( Select1_BV_F_even );
    }

    int LF_eo( int i ) const {
        int l = L_even(i);
        return F_odd_select( l, L_even_rank( l, 0, i ) );
    }

    int LF_oe( int i ) const {
        int l = L_odd(i);
        return F_even_select( l, L_odd_rank( l, 0, i ) );
    }

    int L_even( int i ) const {
        return WT_L_even[i]-1;
    }

    int L_even_rank( int a, int i, int j ) const {
        if( j < i ) return 0;
        return WT_L_even.rank( j+1, a+1 ) - (i>0?WT_L_even.rank( i, a+1 ):0);
    }

    int L_even_rank_ge( int a, int i, int j ) const {
        if( j < i ) return 0;
        auto ret = WT_L_even.lex_count( i, j+1, a );
        return std::get<2>(ret);
    }

    int L_odd( int i ) const {
        if( i == 0 ) return -1;
        return BV_L_odd[i-1];
    }

    int L_odd_rank( int b, int i, int j ) const {
        if( b == -1 ) {
            if( i <= 0 && 0 <= j ) return 1;
            else                   return 0;
        }

        int r1_j = j>=0?Rank1_BV_L_odd(j  ):0;
        int r1_i = i>0 ?Rank1_BV_L_odd(i-1):0;
        int r1 = r1_j - r1_i;

        if( b == 1 ) {
            return r1;
        } else {
            return j-i+1-r1-(!i);
        }
    }

    int F_even_select( int b, int i ) const {
        switch( b ) {
            case -1: return 0;
            case  0: return Select0_BV_F_even(i)+1;
            case  1: return Select1_BV_F_even(i)+1;
            default: return -1;
        }
    }

    int F_odd_select( int a, int i ) const {
        return WT_F_odd.select( i, a+1 );
    }

    int LF_eo_rMq( int i, int j ) const {
        return RMQ_eo( i, j );
    }

    void update_suffix_range_case1( int a, int b, int& p_s, int& p_e ) const {
        // Lines 3-12 in Algorithm 1
        int c;

        c   = L_even_rank ( a, p_s, p_e );
        p_e = F_odd_select( a, L_even_rank( a, 0, p_e ) );
        p_s = p_e - c + 1;

        c   = L_odd_rank   ( b, p_s, p_e );
        p_e = F_even_select( b, L_odd_rank( b, 0, p_e ) );
        p_s = p_s - p_e + 1;
    }

    void update_suffix_range_case2( int a, int b, int& p_s, int& p_e ) const {
        // Lines 14-19 in Algorithm 1
        int i = LF_eo_rMq( p_s, p_e );
        int l = L_even   ( i );
        int c = L_even_rank_ge( a, p_s, p_e );

        p_e = F_odd_select( L_even_rank( l, 0, i ) );
        p_s = p_e - c + 1;
    }

    int count( const std::vector<int>& P ) {
        // Counting the occurrences of P using Algorithm 1 repeatedly.

        int m = P.size();

        int p_s = 0;
        int p_e = N;

        std::vector<int> GP(m);
        for( int j = 0; j < m; ++j ) { GP[j] = P[j]/2; }

        std::map<int,int> last_group;
        AVLTree<int> pos(Sigma);

        for( int j = m-1; j >= 0; --j ) {
            if( p_s > p_e ) break;
            // Algorithm 1
            if( last_group.count( GP[j] ) ) {
                int c;

                int i_g = last_group.at( GP[j] );
                
                int a = pos.rank(i_g);
                //Identical to:
                //int a = std::set<int>( GP.begin()+j+1, GP.begin()+i_g ).size();
                c = L_even_rank( a, p_s, p_e );
                p_e = F_odd_select( a, L_even_rank( a, 0, p_e ) );
                p_s = p_e - c + 1;
                
                int b = (P[i_g] != P[j]);
                c = L_odd_rank( b, p_s, p_e );
                p_e = F_even_select( b, L_odd_rank( b, 0, p_e ) );
                p_s = p_e - c + 1;
            
                pos.remove(i_g); 
            } else { 
                int ii = LF_eo_rMq( p_s, p_e );
                int l = L_even( ii );
                int a = pos.size( pos.root() );
                //Identical to:
                //int a = std::set<int>( GP.begin()+j+1, GP.end() ).size();
                int c = L_even_rank_ge( a, p_s, p_e );
                p_e = F_odd_select( l, L_even_rank( l, 0, ii ) );
                p_s = p_e - c + 1;
            }

            pos.insert( j );
            last_group[ GP[j] ] = j;
        }
        return p_e - p_s + 1;
    }

    private:

    int N;
    int Sigma;

    WT  WT_L_even;
    WT  WT_F_odd;
    RMQ RMQ_eo;

    BV  BV_L_odd;
    BV  BV_F_even;

    typename BV::rank_1_type Rank1_BV_L_odd;
    typename BV::select_0_type Select0_BV_F_even;
    typename BV::select_1_type Select1_BV_F_even;
};
