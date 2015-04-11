#include <cmph.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <bgzf.h>

#include "indexing.h"

/**
 * Concatenates the given strings and returns the concatenated
 * string a + b.
 *
 * Note: User is responsible for calling free on the returned
 * string.
 *
 * @param a First string.
 * @param b Second string.
 *
 * @return pointer to the concatenated string.
 */
char *
concatenate(const char *a, const char *b)
{
    size_t total_length = strlen( a ) + strlen( b ) + 1;
    char *buffer = (char *) malloc( total_length );
    strncpy( buffer, a, total_length );
    strncat( buffer, b, total_length );

    return buffer;
}

char *
bgzf_fgets(char *buffer, size_t buffer_size, BGZF *fp)
{
    char c;
    size_t i = 0;
    while( i < buffer_size - 1 && ((c = bgzf_getc( fp )) > 0) )
    {
        buffer[ i++ ] = c;
        if( c == '\n' )
        {
            break;
        }
    }
    buffer[ i ] = '\0';

    if( i > 0 && c != -1 )
    {
        return buffer;
    }
    else
    {
        return NULL;
    }
}

int
read_one_line(char **line, cmph_uint32 *line_length, BGZF *fp)
{
    *line_length = 0;
    while(1)
    {
        char buffer[ BUFSIZ ];
        char *buffer_p = bgzf_fgets( buffer, BUFSIZ, fp );
        if( buffer_p == NULL )
        {
            return 0;
        }
        size_t buffer_length = strlen( buffer );

        *line = (char *) realloc( *line, *line_length + buffer_length + 1 );
        memcpy( *line + *line_length, buffer, buffer_length );
        *line_length += buffer_length;
        if( buffer[ buffer_length - 1 ] == '\n' )
        {
            break;
        }
    }

    if( *line_length > 0 && (*line)[ *line_length - 1 ] == '\n' )
    {
        (*line)[ *line_length - 1 ] = '\0';
        (*line_length)--;
    }

    return 1;
}

size_t count_fastq_sequences(BGZF *fastq_file)
{
    bgzf_seek( fastq_file, 0, SEEK_SET );

    cmph_uint32 count = 0;
    while( 1 )
    {
        char buffer[ BUFSIZ ];
        size_t bytes_read = bgzf_read( fastq_file, buffer, BUFSIZ );
        if( bytes_read <= 0 )
        {
            break;
        }

        int i;
        for(i = 0; i < bytes_read; i++)
        {
            if( buffer[ i ] == '@' )
            {
                count++;
            }
        }
    }

    bgzf_seek( fastq_file, 0, SEEK_SET );
    return count;
}

int key_fastq_read(void *data, char **key, cmph_uint32 *keylen)
{
    BGZF *fp = (BGZF *) data;

    char c;
    *keylen = 0;
    /* Find header start */
    while( ( c = bgzf_getc( fp ) ) != '@' && c >= 0 )
    {
    }

    *key = NULL;
    if( read_one_line( key, keylen, fp ) == 1 )
    {
        return (int) *keylen;
    }
    else
    {
        return -1;
    }
}

void key_fastq_dispose(void *data, char *key, cmph_uint32 keylen)
{
    free( key );
}

void key_fastq_rewind(void *data)
{
    BGZF *fastq_file = (BGZF *) data;
    bgzf_seek( fastq_file, 0, SEEK_SET );
}

cmph_io_adapter_t *
cmph_io_fastq_adapter(BGZF *fastq_file)
{
    cmph_io_adapter_t * key_source = (cmph_io_adapter_t *) malloc( sizeof( cmph_io_adapter_t ) );

    key_source->data = (void *) fastq_file;
    key_source->nkeys = count_fastq_sequences( fastq_file );
    key_source->read = key_fastq_read;
    key_source->dispose = key_fastq_dispose;
    key_source->rewind = key_fastq_rewind;

    return key_source;
};

void
populate_index(uint64_t *table, cmph_t *hash, BGZF *fastq_file)
{
    while( 1 )
    {
        /* Find @ */
        char c;
        while( ( c = bgzf_getc( fastq_file ) ) != '@' && c >= 0 )
        {
        }
        
        long pos = bgzf_tell( fastq_file );
        if( pos == -1 )
        {
            break;
        }

        char *accession = NULL;
        cmph_uint32 accession_length;
        if( read_one_line( &accession, &accession_length, fastq_file ) != 1 )
        {
            break;
        }

        /* Next char is sequence, save pos */
        unsigned int id = cmph_search( hash, accession, accession_length );
        table[ id ] = (uint64_t) pos;
    }
}

int create_index(BGZF *fastq_file, cmph_t *hash, char *seek_path)
{
    int fd = open( seek_path, O_CREAT | O_RDWR, 0644 );
    if( fd == -1 )
    {
        return 0;
    }

    off_t file_size = sizeof( uint64_t ) * cmph_size( hash );
    if( ftruncate( fd, file_size ) == -1 )
    {
        return 0;
    }

    uint64_t *table = (uint64_t *) mmap( NULL, file_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0 );
    if( table == MAP_FAILED )
    {
        return 0;
    }

    populate_index( table, hash, fastq_file );

    munmap( table, file_size );
    close( fd );

    return 1;
}

ifq_codes_t index_fastq(char *fastq_path, char *index_prefix)
{
    char *hash_path = concatenate( index_prefix, ".hsh" );
    char *seek_path = concatenate( index_prefix, ".lup" );
    ifq_codes_t ret = IFQ_OK;
    
    /* Open output files */
    BGZF *fastq_file = bgzf_open( fastq_path, "r" );
    if( fastq_file == NULL )
    {
        ret = IFQ_BAD_FASTQ;
        goto index_fastq_fail;
    }
    
    FILE *hash_file = fopen( hash_path, "w" );
    if( hash_file == NULL )
    {
        ret = IFQ_BAD_PREFIX;
        goto index_prefix_fail;
    }

    /* Create hash function */
    cmph_io_adapter_t *source = cmph_io_fastq_adapter( fastq_file );
    cmph_config_t *config = cmph_config_new( source );
    cmph_config_set_algo( config, CMPH_CHD );
    cmph_config_set_mphf_fd( config, hash_file );
    cmph_t *hash = cmph_new( config );
    if( hash == NULL )
    {
        ret = IFQ_BAD_HASH;
        goto index_hash_fail;
    }

    /* Create the file index using the hash */
    bgzf_seek( fastq_file, 0, SEEK_SET );
    if( create_index( fastq_file, hash, seek_path ) != 1 )
    {
        ret = IFQ_BAD_INDEX;
        goto index_create_fail;
    }
index_fastq_fail:
    free( hash_path );
    free( seek_path );

index_create_fail:
    cmph_config_destroy( config );
    cmph_dump( hash, hash_file );
    cmph_destroy( hash );
    free( source );

index_hash_fail:
    fclose( hash_file );

index_prefix_fail:
    bgzf_close( fastq_file );

    return IFQ_OK;
}

ifq_codes_t
open_fastq_index(char *fastq_path, char *index_prefix, struct fastq_index_t *index)
{
    char *hash_path = concatenate( index_prefix, ".hsh" );
    char *lookup_path = concatenate( index_prefix, ".lup" );

    ifq_codes_t ret = IFQ_OK;

    index->fastq_file = bgzf_open( fastq_path , "r" );
    if( index->fastq_file == NULL )
    {
        ret = IFQ_BAD_FASTQ;
        goto index_error;
    }
    
    index->hash_file = fopen( hash_path , "r" );
    if( index->hash_file == NULL )
    {
        ret = IFQ_BAD_PREFIX;
        goto index_error;
    }

    index->hash = cmph_load( index->hash_file );
    if( index->hash == NULL )
    {
        ret = IFQ_BAD_HASH;
        goto index_error;
    }

    index->lookup_fd = open( lookup_path, O_RDWR );
    if( index->lookup_fd == -1 )
    {
        ret = IFQ_BAD_PREFIX;
        goto index_error;
    }
    
    struct stat sb;
    fstat( index->lookup_fd, &sb );
    index->lookup_size = sb.st_size;

    index->table = (uint64_t *) mmap( NULL, index->lookup_size, PROT_READ, MAP_FILE | MAP_SHARED, index->lookup_fd, 0 );
    if( index->table == MAP_FAILED )
    {
        ret = IFQ_BAD_INDEX;
        goto index_error;
    }

index_error: 
    free( hash_path );
    free( lookup_path );

    return ret;
}

void
destroy_fastq_index(struct fastq_index_t *index)
{
    if( index != NULL )
    {
        cmph_destroy( index->hash );
        munmap( index->table, index->lookup_size );
        fclose( index->hash_file );
        bgzf_close( index->fastq_file );
        close( index->lookup_fd );
    }
}

ifq_codes_t
query_fastq_index(struct fastq_index_t *index, char *query, struct fastq_record_t *record)
{
    // Find key
    unsigned int id = cmph_search( index->hash, query, (cmph_uint32) strlen( query ) );
    uint64_t pos = index->table[ id ];
    if( bgzf_seek( index->fastq_file, pos, SEEK_SET ) < 0 )
    {
        return IFQ_NOT_FOUND;
    }

    cmph_uint32 length;
    read_one_line( &record->name, &length, index->fastq_file );
    if( strncmp( record->name, query, length ) == 0 )
    {
        read_one_line( &record->sequence, &length, index->fastq_file );
        read_one_line( &record->quality, &length, index->fastq_file );
        read_one_line( &record->quality, &length, index->fastq_file );
    }
    else
    {
        return IFQ_NOT_FOUND;
    }

    return IFQ_OK;
}

struct fastq_record_t *
new_fastq_record()
{
    struct fastq_record_t *record = (struct fastq_record_t *) malloc( sizeof( struct fastq_record_t ) );
    if( record != NULL )
    {
        record->name = NULL;
        record->sequence = NULL;
        record->quality = NULL;

        return record;
    }
    else
    {
        return NULL;
    }
}

void
destroy_fastq_record(struct fastq_record_t *record)
{
    if( record != NULL )
    {
        if( record->name != NULL )
        {
            free( record->name );
        }
        if( record->sequence != NULL )
        {
            free( record->sequence );
        }
        if( record->quality != NULL )
        {
            free( record->quality );
        }
    }
}
