#!/bin/bash

for i in 9 10 11 12
do
./chainConfigurator.py -W 50 -H 50 -C 0.5 -s -I 0.0 -n 25 --one-active-chain ./ChainFiles/SingleFocus_$i.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 10000 --kRnaPlus 1e-1 --kRnaMinusRbp 8.3333e-4 --kChromPlus 8.3333e-3 --kChromMinus 0 --kOn 1e-0 --kOff 0 --kRnaTransfer 3.3333e-3 -t 20 --flavopiridol 0 --activate 600 -E 1500 -T 1500 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SingleFocus_'$i'.chains -o ~/academia/github_Repositories/active-microemulsion/FocusResults_50_50/FocusOut_'$i' --no-sticky-boundary"
end tell'
done

