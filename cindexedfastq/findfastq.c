#include "indexing.h"

int main(int argc, char **argv)
{
    if( argc != 4 )
    {
        printf( "Usage: findfastq fastq index key\n" );
        exit( 1 );
    }

    struct fastq_index_t index;
    if( open_fastq_index( argv[ 1 ], argv[ 2 ], &index ) != IFQ_OK )
    {
        printf( "error: Could not open index." );
        exit( 1 );
    }

    struct fastq_record_t *record = new_fastq_record( );
    if( query_fastq_index( &index, argv[ 3 ], record ) == IFQ_OK )
    {
        printf( "%s\n%s\n%s\n", record->name, record->sequence, record->quality );
    }
    else
    {
        printf( "record not found\n" );
    }

    destroy_fastq_record( record );
    destroy_fastq_index( &index );

    return 0;
}
