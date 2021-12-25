chmod a-x src/build/*.mac
`find src/build -maxdepth 1 -type f -perm /001` -d
while [ -f to_be_executed.sh ]
do
    script=$( cat to_be_executed.sh )
    rm to_be_executed.sh
    eval $script
done
