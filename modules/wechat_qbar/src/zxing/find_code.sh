
if [ $# -lt 1 ]; then
    echo "Usage $0 <code_type>"
    exit
fi

#CODE_TYPE=datamatrix
CODE_TYPE=$1

grep "NotFoundException.h" * -rn |grep $CODE_TYPE
grep "ChecksumException.h" * -rn |grep $CODE_TYPE
grep "Exception.h" * -rn |grep $CODE_TYPE
grep "ReaderException.h" * -rn |grep $CODE_TYPE
grep "IllegalArgumentException.h" * -rn |grep $CODE_TYPE
grep "ReedSolomonException.h" * -rn |grep $CODE_TYPE
grep "FormatException.h" * -rn |grep $CODE_TYPE
grep "DetectorException.h" * -rn |grep $CODE_TYPE
grep "IllegalStateException.h" * -rn |grep $CODE_TYPE    
