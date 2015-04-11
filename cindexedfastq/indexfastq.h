#ifndef __INDEXFASTQ_H__
#define __INDEXFASTQ_H__

#include <cmph.h>
#include <bgzf.h>

typedef enum
{
    IFQ_OK,
    IFQ_BAD_FASTQ,
    IFQ_BAD_PREFIX,
    IFQ_BAD_HASH,
    IFQ_BAD_INDEX,
    IFQ_NOT_FOUND
} ifq_codes_t;

struct fastq_record_t
{
    char *name;
    char *sequence;
    char *quality;
};

struct fastq_index_t
{
    cmph_t *hash;
    BGZF *fastq_file;
    FILE *hash_file;
    int lookup_fd;
    uint64_t *table;
    off_t lookup_size;
};

ifq_codes_t open_fastq_index(char *fastq_path, char *index_prefix, struct fastq_index_t *index);
void destroy_fastq_index(struct fastq_index_t *index);
ifq_codes_t query_fastq_index(struct fastq_index_t *index, char *query, struct fastq_record_t *record);

struct fastq_record_t *new_fastq_record();
void destroy_fastq_record(struct fastq_record_t *record);

#endif /* End of __INDEXFASTQ_H__ */
