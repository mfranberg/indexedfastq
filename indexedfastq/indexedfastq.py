import cindexedfastq

class FastqRecord:
    def __init__(self, name, sequence, quality):
        self.name = name
        self.sequence = sequence
        self.quality = quality

    def __str__(self):
        return "@{0}\n{1}\n+\n{2}\n".format( self.name, self.sequence, self.quality )

    def __repr__(self):
        return "(@{0},{1},+,{2})".format( self.name, self.sequence, self.quality )

class IndexedFastq:
    def __init__(self, handle):
        self.handle = handle

    def fetch(self, query_iter):
        if not self.handle:
            return

        if isinstance( query_iter, basestring ):
            name, sequence, quality = cindexedfastq.query_indexed_fastq( self.handle, query_iter )
            if name != None:
                yield FastqRecord( name, sequence, quality )
                return

        for query in query_iter:
            name, sequence, quality = cindexedfastq.query_indexed_fastq( self.handle, query )

            if name != None:
                yield FastqRecord( name, sequence, quality )

    def close(self):
        if self.handle:
            handle = cindexedfastq.close_indexed_fastq( fastq_path, index_prefix )
            self.handle = None

def create_indexed_fastq(fastq_path, index_prefix=None, open=True):
    if not index_prefix:
        index_prefix = fastq_path

    cindexedfastq.create_indexed_fastq( fastq_path, index_prefix )

    if open:
        return cindexedfastq.open_indexed_fastq( fastq_path, index_prefix )
    else:
        return None

def open_indexed_fastq(fastq_path, index_prefix=None):
    if not index_prefix:
        index_prefix = fastq_path
    
    handle = cindexedfastq.open_indexed_fastq( fastq_path, index_prefix )

    return IndexedFastq( handle )
