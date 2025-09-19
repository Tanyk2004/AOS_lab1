qemu-system-x86_64 -enable-kvm -m 500 -smp 2 \
  -drive file=./jammy.qcow2,if=virtio \
  -netdev user,id=n0 \
  -device virtio-net-pci,netdev=n0 \
  -nographic
