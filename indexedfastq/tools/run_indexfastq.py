from __future__ import print_function
from __future__ import unicode_literals

from indexedfastq import create_indexed_fastq
import sys
import subprocess

def main():
    if not len( sys.argv ) in (2, 3):
        print( "Usage: indexfastq path [index_prefix]" )
        exit( 1 )

    fastq_path = sys.argv[ 1 ]
    if not fastq_path.endswith( ".gz" ):
        cmd = [ "bgzip", fastq_path ]
        subprocess.check_call( cmd )
        fastq_path = fastq_path + ".gz"

    if len( sys.argv ) == 2:
        create_indexed_fastq( fastq_path, open = False )
    elif len( sys.argv ) == 3:
        create_indexed_fastq( fastq_path, sys.argv[ 2 ], open = False )

if __name__ == "__main__":
    main( )
