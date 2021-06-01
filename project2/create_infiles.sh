#!/bin/bash

if [ "$#" -ne 5 ]; then
    echo Wrong Argumnets
    exit
fi

if [ "$4" -le 0 ]; then
    echo numFilesPerDirectory has to be positive
    exit
fi

if [ "$5" -le 0 ]; then
    echo numRecordsPerFile has to be positive
    exit
fi

diseaseFile=`pwd`/$1
countriesFile=`pwd`/$2
input_dir=`pwd`/$3
numFilesPerDirectory=$4
numRecordsPerFile=$5

numCoutries=`wc -l < $countriesFile`

numDiseases=`wc -l < $diseaseFile`

mkdir $input_dir
cd "$input_dir"

for ((i=1; i<=$numCoutries; i++)) do
    subdirectory=`head -$i $countriesFile | tail -1`
    mkdir $subdirectory
    cd "$subdirectory"

    for ((j=1; j<=$numFilesPerDirectory; j++)) do
        FLOOR=0                                 #generate day
        RANGE=31
        day=0
        while [ "$day" -le $FLOOR ]
        do
            day=$RANDOM
            let "day %= $RANGE"
        done
        if [ "$day" -le 9 ]; then
            day=0$day
        fi

        FLOOR=0                              #generate month
        RANGE=13
        month=0
        while [ "$month" -le $FLOOR ]
        do
            month=$RANDOM
            let "month %= $RANGE"
        done

        if [ "$month" -le 9 ]; then
            month=0$month
        fi

        FLOOR=1999                          #generate day
        RANGE=2021
        year=0
        while [ "$year" -le $FLOOR ]
        do
            year=$RANDOM
            let "year %= $RANGE"
        done

        fileName=$day-$month-$year
        touch $fileName

        for ((k=1; k<=$numRecordsPerFile; k++)) do  #generate record
            FLOOR=0                                 #generate recordID
            RANGE=10001
            recordID=0
            while [ "$recordID" -le $FLOOR ]
            do
                recordID=$RANDOM
                let "recordID %= $RANGE"
            done

            BINARY=2                        #ENTER or EXIT
            T=1
            number=$RANDOM
            let "number %= $BINARY"
            #  Note that    let "number >>= 14"    gives a better random distribution
            #+ (right shifts out everything except last binary digit).
            if [ "$number" -eq $T ] || [ "$j" -eq 1 ] || [ "$k" -eq 1 ]
            then
                condition=ENTER
            else
                condition=EXIT
                pickedCondition=EXIT
                while [ "$pickedCondition" != "ENTER" ]
                do
                    FLOOR=0
                    RANGE=$(($j+1))
                    pickedFile=0
                    while [ "$pickedFile" -le $FLOOR ]
                    do
                      pickedFile=$RANDOM
                      let "pickedFile %= $RANGE"
                    done

                    pickedFileName=`ls -c | head -$pickedFile | tail -1`
                    #echo $j $pickedFile $pickedFileName

                    FLOOR=0
                    if [ "$pickedFile" -eq 1 ]
                    then
                        RANGE=$(($k+1))
                    else
                        RANGE=$(($numRecordsPerFile+1))
                    fi
                    pickedRecordNumber=0
                    while [ "$pickedRecordNumber" -le $FLOOR ]
                    do
                        pickedRecordNumber=$RANDOM
                        let "pickedRecordNumber %= $RANGE"
                    done
                    #echo $pickedRecordNumber
                    pickedRecord=`head -$pickedRecordNumber $pickedFileName | tail -1`
                    recordID=`echo $pickedRecord | cut -d " " -f 1`
                    pickedCondition=`echo $pickedRecord | cut -d " " -f 2`
                    patientFirstName=`echo $pickedRecord | cut -d " " -f 3`
                    patientLastName=`echo $pickedRecord | cut -d " " -f 4`
                    disease=`echo $pickedRecord | cut -d " " -f 5`
                    age=`echo $pickedRecord | cut -d " " -f 6`
                done
                record="$recordID $condition $patientFirstName $patientLastName $disease $age"
                echo $record >> $fileName
                continue;
            fi

            FLOOR=3                                 #generate day
            RANGE=12
            nameLength=0
            while [ "$nameLength" -le $FLOOR ]
            do
                nameLength=$RANDOM
                let "nameLength %= $RANGE"
            done
            patientFirstName=`head /dev/urandom | tr -dc A-Z | head -c 1`       #generate first name
            patientFirstName=$patientFirstName`head /dev/urandom | tr -dc a-z | head -c $nameLength`

            FLOOR=3                                 #generate day
            RANGE=12
            nameLength=0
            while [ "$nameLength" -le $FLOOR ]
            do
                nameLength=$RANDOM
                let "nameLength %= $RANGE"
            done
            patientLastName=`head /dev/urandom | tr -dc A-Z | head -c 1`        #generate last name
            patientLastName=$patientLastName`head /dev/urandom | tr -dc a-z | head -c $nameLength`

            FLOOR=0                                 #pick disease
            RANGE=$((numDiseases + 1))
            pick=0
            while [ "$pick" -le $FLOOR ]
            do
                pick=$RANDOM
                let "pick %= $RANGE"
            done
            disease=`head -$pick $diseaseFile | tail -1`

            FLOOR=0                                 #generate age
            RANGE=121
            age=0
            while [ "$age" -le $FLOOR ]
            do
                age=$RANDOM
                let "age %= $RANGE"
            done

            record="$recordID $condition $patientFirstName $patientLastName $disease $age"
            echo $record >> $fileName
        done
    done

    cd ..
done
