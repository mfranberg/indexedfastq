#ifndef __IFQ_H__
#define __IFQ_H__

#include <cmph.h>
#include <bgzf.h>

typedef enum
{
    /**
     * The call was successful.
     */
    IFQ_OK,

    /**
     * Unable to open the fastq file.
     */
    IFQ_BAD_FASTQ,

    /**
     * Unable to open the indexing files.
     */
    IFQ_BAD_PREFIX,

    /**
     * Unable to create the hash function.
     */
    IFQ_BAD_HASH,

    /**
     * Unable to open the index.
     */
    IFQ_BAD_INDEX,

    /**
     * Could not find the record that was searched for.
     */
    IFQ_NOT_FOUND
} ifq_codes_t;


typedef struct ifq_record
{
    /**
     * Sequence accession.
     */
    char *name;

    /**
     * The sequence.
     */
    char *sequence;

    /**
     * Quality values.
     */
    char *quality;
} ifq_record_t;

typedef struct ifq_index
{
    /**
     * Perfect hash function.
     */
    cmph_t *hash;

    /**
     * Compressed fastq file.
     */
    BGZF *fastq_file;

    /**
     * File that contains the hash function.
     */
    FILE *hash_file;

    /**
     * File that contains the mapping from hash value
     * to seek position.
     */
    int lookup_fd;

    /**
     * Mapping from hash value to seek position.
     */
    uint64_t *table;

    /**
     * Size of the lookup table in bytes.
     */
    off_t lookup_size;
} ifq_index_t;

/**
 * Create a new index at the given prefix, a .hsh and .lup file
 * will be created with the given prefix.
 *
 * @param fastq_path Path to the bgzipped fastq file.
 * @apram index_prefix The prefix path of the index.
 *
 * @return IFQ_OK if successful, IFQ_BAD_FASTQ if the fastq file
 *         could not be opened, IFQ_BAD_PREFIX if the index could
 *         not be created.
 */
ifq_codes_t ifq_create_index(char *fastq_path, char *index_prefix);

/**
 * Open an existing index.
 *
 * @apram fastq_path Path to the bgzipped fastq file.
 * @param index_prefix The prefix path of the index.
 * @param index The index.
 *
 * @return IFQ_OK if successful, IFQ_BAD_FASTQ if the fastq file
 *         could not be opened, IFQ_BAD_PREFIX if the index could
 *         not be opened.
 */
ifq_codes_t ifq_open_index(char *fastq_path, char *index_prefix, ifq_index_t *index);

/**
 * Close an opened index along with its allocated memory.
 *
 * @param index An opened index.
 */
void ifq_destroy_index(ifq_index_t *index);

/**
 * Query the index to find the desired fastq record.
 *
 * @param index The index.
 * @param query The accession of the record to find.
 * @param record A record, output will be stored here.
 *
 * @return IFQ_OK if successful, IFQ_NOT_FOUND if the record was missing.
 */
ifq_codes_t ifq_query_index(ifq_index_t *index, char *query, ifq_record_t *record);

/**
 * Create a new fastq record.
 *
 * @return The created object.
 */
ifq_record_t *ifq_new_record();

/**
 * Destroy a fastq record object along with its
 * allocated data.
 */
void ifq_destroy_record(ifq_record_t *record);

#endif /* End of __IFQ_H__ */
