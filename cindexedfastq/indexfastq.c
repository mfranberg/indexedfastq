#include <ifq.h>

int main(int argc, char **argv)
{
    if( argc != 3 )
    {
        printf( "Usage: indexfastq fastq outputprefix\n" );
        exit( 1 );
    }

    if( ifq_create_index( argv[ 1 ], argv[ 2 ] ) != IFQ_OK )
    {
        printf( "Failed to create index\n" );
        return 1;
    }
    else
    {
        return 0;
    }
}
