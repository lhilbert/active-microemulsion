#!/bin/bash

# Without transcription
./chainConfigurator.py -W 60 -H 60 -s -I 0.0 -n 36 ./ChainFiles/LongTerm_1.chains
osascript -e 'tell app "Terminal"
    do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 60 -H 60 -w 0.27 -s 3000 --kRnaPlus 1e-0 --kRnaMinusRbp 8.3333e-4 --kChromPlus 1.5e-3 --kChromMinus 1.6666e-3 --kOn 1e-2 --kOff 0 --kRnaTransfer 3.3e-3 -t 5 --additional-snapshots --flavopiridol 0 -E 1800 -T 1800 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_1.chains --no-sticky-boundary -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inactive"
end tell'

# With transcription
./chainConfigurator.py -W 60 -H 60 -s -I 0.0 -n 36 ./ChainFiles/LongTerm_2.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 60 -H 60 -w 0.27 -s 3000 --kRnaPlus 1e-0 --kRnaMinusRbp 8.3333e-4 --kChromPlus 1.5e-3 --kChromMinus 1.6666e-3 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 -t 5 --additional-snapshots --flavopiridol 0 --activate 60 -E 3000 -T 3000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_2.chains --no-sticky-boundary -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Active"
end tell'

# Inhibited after RNA production
./chainConfigurator.py -W 60 -H 60 -s -I 0.0 -n 36 ./ChainFiles/LongTerm_3.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 60 -H 60 -w 0.27 -s 3000 --kRnaPlus 1e-0 --kRnaMinusRbp 0 --kChromPlus 1.5e-3 --kChromMinus 1.6666e-3 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 -t 5 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 600 -E 4200 -T 4200 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_3.chains --no-sticky-boundary -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inhibited"
end tell'
