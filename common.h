#ifndef __STRPATMATCH_COMMON_H__
#define __STRPATMATCH_COMMON_H__

#include <vector>
#include <map>
#include <set>

template <class STR>
std::vector<int> PS( const STR& T, std::vector<int>& D_even, std::vector<int>& D_odd ) {
    // Computes the pointer sequence representation of T with D_even and D_odd in Eqs (8) and (9)
    // [INPUT ] T: an s-string
    //          the group ID of a character c is assumed to be floor(c/2); e.g. g(0)=g(1), g(2)=g(3).
    // [OUTPUT] return PS_T: corresponding pointer sequence
    //          D_even: Eq (8) 
    //          D_odd : Eq (9)
    int n = T.size();
    std::vector<int> G (n); // group ID
    for( int i = 0; i < n; ++i ) {
        G[i] = T[i] / 2;
    }

    std::vector<int> nu(n); // distance to the next occurrence of T[i]
    std::vector<int> mu(n); // distance to the next occurrence of equal-group character of T[i]

    // for brevity, we use map.
    std::map<int,int> next_position_of_character;
    std::map<int,int> next_position_of_group;

    for( int i = n-1; i >= 0; --i ) {
        int c = T[i];
        int g = G[i];

        // Eq (3)
        if( next_position_of_character.count(c) == 0 ) {
            nu[i] = n; // "n" represents the infinity-symbol
        } else {
            nu[i] = next_position_of_character.at(c) - i;
        }
        
        // Eq (4)
        if( next_position_of_group.count(g) == 0 ) {
            mu[i] = n;
        } else {
            mu[i] = next_position_of_group.at(g) - i;
        }

        next_position_of_character[c] = i;
        next_position_of_group    [g] = i;
    }

    std::vector<int> PS_T(2*n); // resulting pointer sequence
    for( int i = 0; i < 2*n; ++i ) {
        // Eq (5)
        if( i%2 == 0 ) {
            int i_ = i/2;
            if( nu[i_] == n ) { PS_T[i] = 2*n; }
            else              { PS_T[i] = 2*nu[i_] + 1; }
        } else {
            int i_ = (i-1)/2;
            if( mu[i_] == n ) { PS_T[i] = 2*n; }
            else              { PS_T[i] = 2*mu[i_] - 1; }
        }
    }

    // compute D_even, D_odd by Eqs (8) and (9)
    D_even.resize(n);
    D_odd .resize(n);
    for( int i = 0; i < n; ++i ) {
        if( nu[i] == mu[i] ) D_even[i] = 0;
        else                 D_even[i] = 1;

        if( mu[i] == n ) {
            D_odd[i] = std::set<int>( G.begin()+i+1, G.end() ).size();
        } else {
            D_odd[i] = std::set<int>( G.begin()+i+1, G.begin()+i+mu[i] ).size();
        }
    }
    return PS_T;
}


#endif
