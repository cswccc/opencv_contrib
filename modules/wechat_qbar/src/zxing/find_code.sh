
if [ $# -lt 1 ]; then
    echo "Usage $0 <code_type>"
    exit
fi

#CODE_TYPE=datamatrix
CODE_TYPE=$1

grep "NotFoundException.hpp" * -rn |grep $CODE_TYPE
grep "ChecksumException.hpp" * -rn |grep $CODE_TYPE
grep "Exception.hpp" * -rn |grep $CODE_TYPE
grep "ReaderException.hpp" * -rn |grep $CODE_TYPE
grep "IllegalArgumentException.hpp" * -rn |grep $CODE_TYPE
grep "ReedSolomonException.hpp" * -rn |grep $CODE_TYPE
grep "FormatException.hpp" * -rn |grep $CODE_TYPE
grep "DetectorException.hpp" * -rn |grep $CODE_TYPE
grep "IllegalStateException.hpp" * -rn |grep $CODE_TYPE    
