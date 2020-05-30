#!/bin/bash

# export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
# sudo code --user-data-dir /home/robin/.vscode proj.code-workspace
#

./server_example_inputs lo 7102 cfg/IED1_XCBR.cfg cfg/IED1_XCBR.ext L 65000 &

./server_example_inputs lo 8102 cfg/IED2_PTOC.cfg cfg/IED2_PTOC.ext L 65001 &

./server_example_inputs lo 9102 cfg/IED3_SMV.cfg cfg/IED3_SMV.ext L 65002 &

sleep 1
read -n 1 -s -r -p "Press any key to stop"

[[ -z "$(jobs -p)" ]] || kill $(jobs -p)

echo ""
echo "All jobs are stopped"
echo ""
