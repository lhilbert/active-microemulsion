#!/bin/bash

# Without transcription
./chainConfigurator.py -W 100 -H 100 -C 0.5 -s -I 0.0 -n 100 ./ChainFiles/LongTerm_1.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 100 -H 100 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 8.3333e-4 --kChromPlus 1.5e-2 --kChromMinus 1.6666e-3 --kOn 1e-2 --kOff 0 --kRnaTransfer 3.3e-3 -t 30 --additional-snapshots --flavopiridol 0 -E 3000 -T 3000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_1.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inactive --no-sticky-boundary"
end tell'

# With transcription
./chainConfigurator.py -W 100 -H 100 -C 0.5 -s -I 0.0 -n 100 ./ChainFiles/LongTerm_2.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 100 -H 100 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 8.3333e-4 --kChromPlus 1.5e-3 --kChromMinus 1.6666e-3 --kOn 1e-3 --kOff 3.3333e-3 --kRnaTransfer 1.5e-2 -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 5400 -T 5400 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_2.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Active --no-sticky-boundary"
end tell'

# Inhibited after RNA production
./chainConfigurator.py -W 100 -H 100 -C 0.5 -s -I 0.0 -n 100 ./ChainFiles/LongTerm_3.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 100 -H 100 -w 0.5 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 600 -E 18000 -T 18000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_3.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inhibited --no-sticky-boundary"
end tell'
