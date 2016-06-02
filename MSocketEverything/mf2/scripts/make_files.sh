#/bin/bash
rm ~/files/file*
dd if=/dev/zero of=~/files/file_1K bs=1 count=0 seek=1K
dd if=/dev/zero of=~/files/file_4K bs=1 count=0 seek=4K
#dd if=/dev/zero of=file_16K bs=1 count=0 seek=16K
#dd if=/dev/zero of=file_64K bs=1 count=0 seek=64K
#dd if=/dev/zero of=file_256K bs=1 count=0 seek=256K
dd if=/dev/zero of=~/files/file_1M bs=1 count=0 seek=1M
dd if=/dev/zero of=~/files/file_10M bs=1 count=0 seek=10M
#dd if=/dev/zero of=file_100M bs=1 count=0 seek=100M
dd if=/dev/zero of=~/files/file_200M bs=1 count=0 seek=200M
#dd if=/dev/zero of=file_500M bs=1 count=0 seek=500M
#dd if=/dev/zero of=file_1G bs=1 count=0 seek=1G

