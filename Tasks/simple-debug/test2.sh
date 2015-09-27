sudo insmod md1.ko
sudo insmod mt1.ko
dmesg | tail -n40 | grep 'test_'
sudo rmmod md1 
