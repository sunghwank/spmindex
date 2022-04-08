#ifndef __MY_AVL_H__
#define __MY_AVL_H__

#include <vector>
#include <cassert>

template <class K>
class AVLTree {
    public:
    typedef K   key_t;
    typedef int node_t;

    AVLTree( int max_size ) {
        m_root = -1;
        m_key       .resize( max_size );
        m_left      .resize( max_size );
        m_right     .resize( max_size );
        m_height    .resize( max_size );
        m_size      .resize( max_size );
        m_free_nodes.resize( max_size );
        for( int i = 0; i < max_size; ++i ) {
            m_free_nodes[i] = max_size-i-1;
        }
    }

    inline node_t root  ( void     ) const { return m_root;    }
    inline size_t size  ( node_t v ) const { if( v == -1 ) return 0; else return m_size  [v]; }
    inline size_t height( node_t v ) const { if( v == -1 ) return 0; else return m_height[v]; }
    inline node_t left  ( node_t v ) const { return m_left[v]; }
    inline node_t right ( node_t v ) const { return m_right[v];}
    
    void insert( key_t k ) {
        if( m_root == -1 ) m_root = m_new_node( k );
        else m_root = m_insert( m_root, k );
    }

    void remove( key_t k ) {
        m_root = m_remove( m_root, k );
    }

    size_t rank( K k ) {
        size_t count = 0;
        node_t v = m_root;
        while( v != -1 ) {
            if( k < m_key[v] ) {
                v = left(v);
            } else if( k > m_key[v] ) {
                count += size(left(v))+1;
                v = right(v);
            } else {
                return count+size(left(v));
            }
        }
        return count;
    }

    private:
    int bf( node_t v ) {
        if( v == -1 ) return 0;
        int h_l = height(left (v));
        int h_r = height(right(v));
        return h_l - h_r;
    }

    void m_update( node_t v ) {
        m_size[v] = size(left(v))+size(right(v))+1;
        int h_l = height(left(v));
        int h_r = height(right(v));
        int h = h_l>h_r?h_l:h_r;
        m_height[v] = h + 1;
    }

    node_t m_balance( node_t v ) {
        if( bf(v) > 1 ) { // left heavy
            if( bf(left(v)) < 0 ) {
                m_left[v] = rotate_l(left(v));
            }
            return rotate_r(v);
        } else if( bf(v) < -1 ) { // right heavy
            if( bf(right(v)) > 0 ) {
                m_right[v] = rotate_r(right(v));
            }
            return rotate_l(v);
        }
        return v;
    }

    node_t m_insert( node_t v, key_t k ) {
        if( v == -1 ) return m_new_node( k );
        if( k < m_key[v] ) {
            m_left[v] = m_insert( left(v), k );
        } else if( k > m_key[v] ) {
            m_right[v] = m_insert( right(v), k );
        } else {
            return v;
        }
        m_update(v);
        return m_balance(v);
    }

    node_t m_remove( node_t v, key_t k ) {
        if( v == -1 ) return v;

        if( k < m_key[v] ) {
            m_left[v] = m_remove( left(v), k );
        } else if( k > m_key[v] ) {
            m_right[v] = m_remove( right(v), k );
        } else {
            if( left(v) != -1 && right(v) != -1 ) {
                node_t u = right(v);
                while( left(u) != -1 ) {
                    u = left(u);
                }
                int k_u = m_key[u];
                m_right[v] = m_remove( right(v), k_u );
                m_key  [v] = k_u;
            } else {
                m_free_node(v);
                if( left(v) == -1 ) {
                    return right(v);
                } else if( right(v) == -1 ) {
                    return left(v);
                }
            }
        }
        m_update(v);
        return m_balance(v);
    }

    node_t m_new_node( key_t k ) { 
        size_t n = m_free_nodes.size();
        node_t v = m_free_nodes[n-1];
        m_free_nodes.resize(n-1);
        m_key   [v] = k;
        m_left  [v] = m_right[v] = -1;
        m_height[v] = 1;
        m_size  [v] = 1;
        return v;
    }

    void m_free_node( node_t v ) {
        m_free_nodes.push_back( v ); 
    }

    node_t rotate_r( node_t v ) {
        node_t l  = left (v);
        node_t lr = right(l);
        m_right[l] = v;
        m_left [v] = lr;
        m_update(v);
        m_update(l);
        return l;
    }

    node_t rotate_l( node_t v ) {
        node_t r  = right(v);
        node_t rl = left (r);
        m_left [r] = v;
        m_right[v] = rl;
        m_update(v);
        m_update(r);
        return r;
    }

    node_t m_root;

    std::vector<node_t> m_key;

    std::vector<node_t> m_left;
    std::vector<node_t> m_right;
    std::vector<size_t> m_height;
    std::vector<size_t> m_size;

    std::vector<node_t> m_free_nodes;
};

#endif
