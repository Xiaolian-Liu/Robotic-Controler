export PATH=$PATH:/home/vito/Robotic-Controler/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/vito/Robotic-Controler/lib
cd ../kinematics
for i in *.h
do
    ln -b $i ../include/$i
done

/etc/init.d/ethercat start

chmod 606 /dev/cpu_dma_latency
echo "1024">/proc/sys/fs/mqueue/msg_max