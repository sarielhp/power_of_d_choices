/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * hash_games.C -
 *
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include  <stdlib.h>
#include  <stdio.h>
#include  <assert.h>
#include  <time.h>
#include  <string.h>

#include  <algorithm>    // std::min
#include  <random>

std::mt19937 rand_generator;


/*--- Constants ---*/

class  IntDistrib
{
private:
    int  count;
    std::vector<int>  dist;

public:
    void  init() {
        count = 0;
        dist.resize( 0 );
    }

    IntDistrib() { init(); }

    void  count_int( int  num ) {
        assert( num >= 0 );

        while  ( (int)dist.size() <= num )
            dist.push_back( 0 );

        dist[ num ]++;
    }

    int  max_val() {
        int  mx = -1;
        for  ( int  ind = 0; ind < (int)dist.size(); ind++ )
            if  ( dist[ ind ] > 0 )
                mx = ind;
        return  mx;
    }

    void  print() {
        int  mx = max_val();

        printf( "-------------------------------------------------\n" );
        for  ( int  ind = 0; ind <= mx; ind++ )
            printf( "%3d | %'12d\n", ind, dist[ ind ] );
        printf( "-------------------------------------------------\n" );
    }

    void  dump( int  n, int  d ) {
        int  mx = max_val();

        printf( "-------------------------------------------------\n" );
        for  ( int  ind = 0; ind <= mx; ind++ )
            printf( "%3d | %'12d\n", ind, dist[ ind ] );
        printf( "-------------------------------------------------\n" );

        printf( "%'d & %d &", n, d );
        for  ( int  ind = 0; ind <= mx; ind++ ) {
            if  ( ind > 0 )
                printf( "&" );
            printf( " %'d ", dist[ ind ] );
        }
        printf( "%% LATEX \n" );
        printf( "-------------------------------------------------\n" );
    }
};

class  BinTable
{
private:
    int  n;
    std::vector<int>  bins;

public:
    BinTable() {
        n = -1;
    }

    void  init( int  _n ) {
        n = _n;
        bins.resize( n );
        for  ( int  ind = 0; ind < n; ind++ )
            bins[ ind ] = 0;;
    }

    int  insert_random_reg_trailing(  int  d ) {
        int  pos = rand_generator() % n;

        for  (int i = 0; i < d; i++ ) {
            int  rpos = (pos + i) % n;
            if  ( bins[ rpos ] == 0 ) {
                bins[ rpos ] = 1;
                return  1;
            }
        }
        return  0;
    }

    void  insert_random_reg() {
        int  pos = rand_generator() % n;

        bins[ pos ]++;
    }

    void  insert_random_d_choices( int  d ) {
        int  ps[ d ];

        for  ( int  ind = 0; ind < d; ind++ )
            ps[ ind ] = rand_generator() % n;

        int  min, min_pos;
        min = bins[ ps[ 0 ] ];
        min_pos = ps[ 0 ];
        for   ( int  ind = 0; ind < d; ind++ ) {
            int  pos = ps[ ind ];
            if  ( bins[ pos ] < min ) {
                min_pos = pos;
                min = bins[ pos ];
            }
        }
        bins[ min_pos ]++;
    }

    void  insert_by_d_choices( std::vector<int>  & ps,
                               int  d ) {

        int  min, min_pos;
        min = bins[ ps[ 0 ] ];
        min_pos = ps[ 0 ];
        for   ( int  ind = 0; ind < d; ind++ ) {
            int  pos = ps[ ind ];
            if  ( bins[ pos ] < min ) {
                min_pos = pos;
                min = bins[ pos ];
            }
        }
        bins[ min_pos ]++;
    }

    void  insert_random_d_choices_always_left( int  d ) {
        int  ps[ d ], delta;

        delta = n/d;
        for  ( int  ind = 0; ind < d; ind++ ) {
            int  start = ind * delta;
            int  end = start + delta;
            if  ( end + delta  > n )
                end = n;
            //printf( "[%d, %d]\n", start, end );
            ps[ ind ] = start + (rand_generator() % (end - start));
        }
        //printf( "delta: %d\n", delta );



        int  min, min_pos;
        min = bins[ ps[ 0 ] ];
        min_pos = ps[ 0 ];
        for   ( int  ind = 0; ind < d; ind++ ) {
            int  pos = ps[ ind ];
            //printf( "%d: [%d]   ", ind, pos );
            if  ( bins[ pos ] < min ) {
                min_pos = pos;
                min = bins[ pos ];
            }
        }
        //printf( "\n" );

        bins[ min_pos ]++;
    }

    void   collect_distr( IntDistrib  & distr ) {
        distr.init();
        for  ( int  ind = 0; ind < n; ind++ )
            distr.count_int( bins[ ind ] );
    }

    int     simulate_trailing_d_choices( int   _n, int  _d,
                                         IntDistrib  & distr ) {
        init( _n );

        int   count = 0;
        for  ( int  ind = 0; ind < n; ind++ ) {
            count += insert_random_reg_trailing( _d );
        }

        collect_distr( distr );
        return  count;
    }


    /// No choices.
    void   simulate_reg( int   _n, IntDistrib  & distr ) {
        init( _n );

        for  ( int  ind = 0; ind < n; ind++ )
            insert_random_reg();

        collect_distr( distr );
    }


    // Simulate d choices...
    void   simulate_d_choices( int   _n, int  _d, IntDistrib  & distr ) {
        init( _n );

        for  ( int  ind = 0; ind < n; ind++ )
            insert_random_d_choices( _d );

        collect_distr( distr );
    }

    void   simulate_streamed_d_choices( int   _n,
                                           int  _d,
                                           IntDistrib  & distr ) {
        int  c_pos;
        init( _n );
        std::vector<int>  choices;

        //fill_choices( choices, _d );
        for   ( int  ind  = 0; ind < _d; ind++ ) {
            choices.push_back( rand_generator() % n );
        }

        c_pos = 0;
        for  ( int  ind = 0; ind < n; ind++ ) {
            insert_by_d_choices( choices, _d );

            choices[ c_pos ] =  rand_generator() % n;
            c_pos = ( c_pos + 1 ) % _d;
        }

        collect_distr( distr );
    }

    void   simulate_d_choices_always_left( int   _n, int  _d,
                                           IntDistrib  & distr ) {
        init( _n );

        for  ( int  ind = 0; ind < n; ind++ )
            insert_random_d_choices_always_left( _d );

        collect_distr( distr );
    }
};

void  usage()
{
    printf( "\n\n"
            "\t choices  n d\n\n"
            "\n"
            );

    printf( "regular: throw ball into a random bin. d is ignored.\n\n" );
    printf( "choices: throw ball into a random bin out of d with "
             "lowest load.\n\n" );
    printf( "streamed choices:\n"
            "\t Try new location for each ball, and previous d-1 random\n"
            "\t choices made in earlier iterations. Overall, only n+d\n"
            "\t locations are choices\n\n" );
    printf( "always_left:\n"
             "\t Partition array into n/d blocks. Choose bin randomly in \n"
             "\t each block. Always throw ball into leftmost bin with min\n"
             "\t load.\n\n" );
    printf( "trailing:\n"
             "\t Weird variant where we try a  random location "
             "\t location. Insert ball\n"
             "\t into first empty bin in next "
             "d bins. Might not insert all balls, \n"
             "\t but the load is always 1.\n"
             );
    exit( -1 );
}



/*--- Start of Code ---*/


int  main( int  argc, char  ** argv )
{
    int  n, d;

    setlocale(LC_ALL,"");

    rand_generator.seed( time( 0 ) );

    IntDistrib  distr, distr_d;
    BinTable  bt_reg, bt;

    n = 1000000000;

    //n = 10000;
    d = 3;

    if    ( argc != 3 ) {
        usage();
    }
    n = atoi( argv[ 1 ] );
    d = atoi( argv[ 2 ] );

    IntDistrib  distr_reg, distr_trailing;

    /// Regular
    bt_reg.simulate_reg( n, distr_reg );
    printf( "%-30s   n: %'d\n", "regular single choice", n );

    distr_reg.print();


    printf( "\n" );
    // d choices
    IntDistrib  distr_d_choices;
    BinTable  bt_d_choices;
    bt_d_choices.simulate_d_choices( n, d, distr_d_choices );
    printf( "%-30s   n: %'d  d: %d\n",
            "d choices",
            n, d );
    distr_d_choices.print();


    // Streamed d choices
    IntDistrib  distr_s_d_choices;
    BinTable  bt_s_d_choices;
    bt_s_d_choices.simulate_streamed_d_choices( n, d, distr_s_d_choices );
    printf( "\n" );
    printf( "%-30s   n: %'d  d: %d\n",
            "Streamed d choices",
            n, d );
    distr_s_d_choices.print();

    // Streamed d choices always left
    IntDistrib  distr_d_choices_left;
    BinTable  bt_d_choices_left;
    bt_d_choices_left.simulate_d_choices_always_left( n, d,
                                                      distr_d_choices_left );
    printf( "\n" );
    printf( "%-30s   n: %'d  d: %d\n",
            "d choices go left",
            n, d );
    distr_d_choices_left.print();

    // trailing d choices
    int  n_in;
    n_in = bt.simulate_trailing_d_choices( n, d, distr_trailing );
    double  ratio = (double)n_in/ (double)n;
    printf( "\n" );
    printf( "%-30s   n: %'d  d: %d",
            "Trialing choices",
            n, d );
    printf( "\n\n"
            "Success        : %'15d   %%: %lg\n"
            "*not inserted* : %'15d \n",
            n_in,
            ratio * 100.0, n - n_in );


    return  0;
}

/* hash_games.C - End of File ------------------------------------------*/
