# Indexed Fastq files

This Python package allows you to index fastq files and query them for the accession. It is highly experimental. It creates a perfect hash map from accession -> file position as an index. A query is then performed by reading the hash function, mmap:ing the index, and hashing the query to get a file position, seeking to that position, and returning the data. The perfect hash map is created through the high performance cmph library.

Note: I have no evidence that an index based on a perfect hash map will be better (in terms of size and speed of the index) than regular hash map, it was just a neat way to learn cmph.

# Installing

The package is installed by

    python setup.py install

# Indexing and accessing the fastq

In its simplest form an index is created by first bgzip:ing the file:

    bgzip /path/to/fastq

and then accessing it from python

    import indexedfastq
    
    ifq = indexedfastq.create_indexed_fastq( "/path/to/fastq.gz" )
    accessions = [ "QUERY_50000_S00:6:1101:3331:4151#AGCTA/1", "QUERY_50001_S00:6:1101:3331:4151#AGCTA/1" ]

    for record in ifq.fetch( accessions ):
        print( "@{0}\n{1}\n+\n{2}\n", record.name, record.sequence, record.quality )


