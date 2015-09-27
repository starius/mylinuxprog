sudo insmod memtim.ko size=$1
dmesg | tail -n30 | grep -v audit
