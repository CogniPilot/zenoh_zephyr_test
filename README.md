# Zenoth Zephyr Test

## Build

```bash
west init -l .
west update
west build -b native_posix -p
./build/zephyr/zephyr.elf
```

## Test

First run the publisher using the zenoh pico example app

```bash
./build/examples/z_pub -m peer -e udp/224.0.0.123:7447#iface=zeth
```

Now on the zephyr shell, start the program with:

```bash
zenoh peer udp/224.0.0.123:7447#iface=eth0
```
