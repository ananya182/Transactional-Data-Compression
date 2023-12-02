## Usage: 
# Compression: bash interface.h C <path/to/dataset.dat> <path/to/output.dat>
# Decompression: bash interface.h D <path/to/compressed_dataset.dat> <path/to/reconstructed.dat>

inputfile=$2;
outputfile=$3;
method=$1

./run $method $inputfile $outputfile;