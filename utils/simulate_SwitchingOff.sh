#!/bin/bash

# Switching on
python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOn_1.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOn_1.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOn_1"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOn_2.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOn_2.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOn_2"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOn_3.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOn_3.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOn_3"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOn_4.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOn_4.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOn_4"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOn_5.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOn_5.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOn_5"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOn_6.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOn_6.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOn_6"
end tell'



# Switching off
python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOff_1.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 0e-3 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --actinomycin-D 1800 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOff_1.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOff_1"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOff_2.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 0e-3 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --actinomycin-D 1800 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOff_2.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOff_2"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOff_3.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 0e-3 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --actinomycin-D 1800 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOff_3.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOff_3"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOff_4.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 0e-3 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --actinomycin-D 1800 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOff_4.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOff_4"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOff_5.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 0e-3 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --actinomycin-D 1800 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOff_5.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOff_5"
end tell'

python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/SwitchingOff_6.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 0e-3 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-1 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --actinomycin-D 1800 --activate 60 -E 6000 -T 6000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/SwitchingOff_6.chains -o ~/academia/github_Repositories/active-microemulsion/utils/SwitchingRate/SwitchingOff_6"
end tell'
