#include <ifq.h>

int main(int argc, char **argv)
{
    if( argc != 4 )
    {
        printf( "Usage: findfastq fastq index key\n" );
        exit( 1 );
    }

    ifq_index_t index;
    if( ifq_open_index( argv[ 1 ], argv[ 2 ], &index ) != IFQ_OK )
    {
        printf( "error: Could not open index." );
        exit( 1 );
    }

    ifq_record_t *record = ifq_new_record( );
    if( ifq_query_index( &index, argv[ 3 ], record ) == IFQ_OK )
    {
        printf( "%s\n%s\n%s\n", record->name, record->sequence, record->quality );
    }
    else
    {
        printf( "record not found\n" );
    }

    ifq_destroy_record( record );
    ifq_destroy_index( &index );

    return 0;
}
