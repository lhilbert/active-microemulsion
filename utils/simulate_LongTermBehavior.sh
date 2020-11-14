#!/bin/bash

# Without transcription
#./chainConfigurator.py -W 100 -H 100 -C 1.0 -I 0.0 -n 400 -b 100 ./ChainFiles/LongTerm_1.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 100 -H 100 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 8.3333e-4 --kChromPlus 1.5e-2 --kChromMinus 1.6666e-3 --kOn 1e-2 --kOff 0 --kRnaTransfer 3.3e-3 -t 30 --additional-snapshots --flavopiridol 0 -E 18000 -T 18000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_1.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inactive --no-sticky-boundary"
#end tell'

# With transcription
#./chainConfigurator.py -W 100 -H 100 -C 1.0 -I 0.0 -n 400 -b 100 ./ChainFiles/LongTerm_2.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 100 -H 100 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 8.3333e-4 --kChromPlus 1.5e-3 --kChromMinus 1.6666e-3 --kOn 1e-3 --kOff 3.3333e-3 --kRnaTransfer 1.5e-2 -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 18000 -T 18000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_2.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Active --no-sticky-boundary"
#end tell'

# Inhibited after RNA production
#./chainConfigurator.py -W 100 -H 100 -C 1.0 -I 0.0 -n 400 -b 100 ./ChainFiles/LongTerm_Block.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 100 -H 100 -w 0.5 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 18000 -T 18000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_Block.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inhibited_Block --no-sticky-boundary"
#end tell'



# Without transcription - dispersed
#./chainConfigurator.py -W 100 -H 100 -C 0.25 -I 0.0 -n 100 -s ./ChainFiles/LongTerm_1_dispersed.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 100 -H 100 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 8.3333e-4 --kChromPlus 1.5e-2 --kChromMinus 1.6666e-3 --kOn 1e-2 --kOff 0 --kRnaTransfer 3.3e-3 -t 30 --additional-snapshots --flavopiridol 0 -E 18000 -T 18000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_1_dispersed.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inactive_Dispersed --no-sticky-boundary"
#end tell'

# With transcription - dispersed
#./chainConfigurator.py -W 100 -H 100 -C 0.25 -I 0.0 -n 100 -s ./ChainFiles/LongTerm_2_dispersed.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 100 -H 100 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 8.3333e-4 --kChromPlus 1.5e-3 --kChromMinus 1.6666e-3 --kOn 1e-3 --kOff 3.3333e-3 --kRnaTransfer 1.5e-2 -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 18000 -T 18000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_2_dispersed.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Active_Dispersed --no-sticky-boundary"
#end tell'

# Inhibited after RNA production - dispersed
#./chainConfigurator.py -W 100 -H 100 -C 0.25 -I 0.0 -n 100 -s ./ChainFiles/LongTerm_3_dispersed.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 100 -H 100 -w 0.5 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 18000 -T 18000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_3_dispersed.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inhibited_Dispersed --no-sticky-boundary"
#end tell'



# Without transcription, fast
#./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/LongTerm_1_Fast.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-1 --kRnaMinusRbp 8.3333e-4 --kChromPlus 1.5e-2 --kChromMinus 1.6666e-3 --kOn 1e-2 --kOff 0 --kRnaTransfer 3.3e-3 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 -E 18000 -T 18000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_1_Fast.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inactive_Fast"
#end tell'

# With transcription, fast
./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/LongTerm_2_Fast.chains
osascript -e 'tell app "Terminal"
do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e-2 --kRnaMinusRbp 1e-4 --kChromPlus 1e-3 --kChromMinus 1e-2 --kOn 1e-3 --kOff 0e-3 --kRnaTransfer 1e-2 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 -E 10000 -T 10000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_2_Fast.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Active_Fast"
end tell'

# Inhibited after RNA production, fast
#./chainConfigurator.py -W 50 -H 50 -C 0.2 -I 0.0 -n 100 ./ChainFiles/LongTerm_3_Fast.chains
#osascript -e 'tell app "Terminal"
#do script "~/academia/github_Repositories/active-microemulsion/active-microemulsion --threads 1 -W 50 -H 50 -w 0.5 -s 3000 --kRnaPlus 1e+1 --kRnaMinusRbp 0 --kChromPlus 5e-3 --kChromMinus 1.6666e-2 --kOn 2.500e-4 --kOff 3.3333e-3 --kRnaTransfer 3.3e-3 --RNP-boundary --no-sticky-boundary -t 30 --additional-snapshots --flavopiridol 0 --activate 60 --flavopiridol 1000 -E 200000 -T 200000 -P ~/academia/github_Repositories/active-microemulsion/utils/ChainFiles/LongTerm_3_Fast.chains -o ~/academia/github_Repositories/active-microemulsion/utils/LongTermOut/Inhibited_Fast"
#end tell'
