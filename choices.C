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

class  IntTable1d
{
public:
    std::vector<int>  row;

    void  add_cols_if_needed( int  cols ) {
        while   ( (int)row.size() <= cols )
            row.push_back( 0 );
    }
    void  set( int  c, int val ) {
        assert( c >= 0 );
        assert( (int)row.size() > c );
        row[ c ] = val;
    }
    int  get( int  c) {
        if  ( ( c < 0 ) || ( c >= (int)row.size() ) )
            return  0;
        return  row[ c ];
    }
};

class  IntTable2d
{
private:
    std::vector<IntTable1d>  rows;
    int   rows_n, cols_n;

public:
    IntTable2d() {
        rows_n = cols_n = 0;
    }
    void  init( int  _rows, int   _cols ) {
        rows_n = _rows;
        cols_n = _cols;
    }

    int  get( int r, int  c) {
        if  ( ( r < 0 ) || ( r >= (int)rows.size() ) )
            return  0;
        IntTable1d  &row( rows[ r ] );
        return  row.get( c );
    }



    void  add_rows_if_needed( int  rw ) {
        while  ( rw >= (int)rows.size() ) {
            IntTable1d itb;
            rows.push_back( itb );
        }
        if  ( rw >= rows_n )
            rows_n = rw + 1;
    }

    void   resize_on_need( int  r, int  c )
    {
        add_rows_if_needed( r );
        rows[ r ].add_cols_if_needed( c );
        if  ( c >= cols_n )
            cols_n = c + 1;
    }

    void  set( int   r, int  c, int  val ) {
        resize_on_need( r, c );
        rows[ r ].set( c, val );
    }

    void   dump_latex() {
        for  ( int  r = 0; r < (int)rows.size(); r++ ) {
            IntTable1d  & itd( rows[ r ] );
            for  ( int  c = 0; c < (int)itd.row.size(); c++ ) {
                if  ( c > 0 )
                    printf(  "&" );
                printf( "%'d ", itd.row[ c ] );
            }
            printf( "\\\\\n" );
        }
    }

    void  dump_latex_to_file( const char  * filename ) {
        FILE  * fl;

        fl = fopen( filename, "wt" );
        assert( fl != NULL );

        fprintf( fl, "\\begin{tabular}{|" );
        for  ( int  ind = 0; ind <= cols_n; ind++ )
            fprintf( fl, "c|" );
        fprintf( fl, "}\n" );
        printf( "cols_n: %d\n", cols_n );
        printf( "rows_n: %d\n", rows_n );
        for  ( int  r = 0; r < rows_n; r++ ) {
            fprintf( fl, "  %d &", r );
            for  ( int  c = 0; c < cols_n; c++ )  {
                if  ( c > 0 )
                    fprintf( fl, " & " );
                int  num = get( r, c );
                if  ( num != 0 )
                    fprintf( fl, "%'12d ", num );
                else
                    fprintf( fl, "%12s ", "" );
            }
            fprintf( fl, "\\\\\n" );
        }
        fprintf( fl, "\\end{tabular}\n\n\n" );

        fclose( fl );

    }

};



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

    void  store_in_col( IntTable2d &tbl, int  col ) {
        int  mx = max_val();

        for  ( int  ind = 0; ind <= mx; ind++ )
            tbl.set( ind, col, dist[ ind ] );
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
    IntTable2d  table;

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