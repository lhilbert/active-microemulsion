#!/bin/bash


## With transcription, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_emulsion_1.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.1 -s 3000 --kRnaPlus 1e-2 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-3 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 10000 -T 10000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_emulsion_1.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_emulsion_1"
#end tell'
#
## Inhibited after RNA production, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_separation_1.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.1 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 200000 -T 200000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_separation_1.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_separation_1"
#end tell'
#
## With transcription, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_emulsion_2.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.2 -s 3000 --kRnaPlus 1e-2 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-3 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 10000 -T 10000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_emulsion_2.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_emulsion_2"
#end tell'
#
## Inhibited after RNA production, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_separation_2.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.2 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 200000 -T 200000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_separation_2.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_separation_2"
#end tell'



# With transcription, fast
python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_emulsion_3.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.3 -s 3000 --kRnaPlus 1e-2 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-3 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 10000 -T 10000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_emulsion_3.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_emulsion_3"
end tell'
#
## Inhibited after RNA production, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_separation_3.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.3 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 200000 -T 200000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_separation_3.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_separation_3"
#end tell'



#
## With transcription, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_emulsion_4.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.4 -s 3000 --kRnaPlus 1e-2 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-3 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 10000 -T 10000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_emulsion_4.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_emulsion_4"
#end tell'
#
## Inhibited after RNA production, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_separation_4.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.4 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 200000 -T 200000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_separation_4.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_separation_4"
#end tell'
#
#
#
## With transcription, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_emulsion_5.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-2 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-3 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 10000 -T 10000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_emulsion_5.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_emulsion_5"
#end tell'
#
## Inhibited after RNA production, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_separation_5.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 200000 -T 200000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_separation_5.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_separation_5"
#end tell'
#
#
#
#
## With transcription, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_emulsion_6.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.6 -s 3000 --kRnaPlus 1e-2 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-3 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 10000 -T 10000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_emulsion_6.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_emulsion_6"
#end tell'
#
## Inhibited after RNA production, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_separation_6.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.6 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 200000 -T 200000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_separation_6.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_separation_6"
#end tell'
#
#
#
## With transcription, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_emulsion_7.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.7 -s 3000 --kRnaPlus 1e-2 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-3 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 10000 -T 10000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_emulsion_7.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_emulsion_7"
#end tell'
#
## Inhibited after RNA production, fast
#python ./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/wParam_separation_7.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.7 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 200000 -T 200000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/wParam_separation_7.chains -o ~/academia/github_Repositories/active-microemulsion/utils/wParam/wParam_separation_7"
#end tell'
