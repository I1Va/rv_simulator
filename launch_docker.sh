#!/bin/bash

docker rm -f rv_tools_image_cont_example >/dev/null 2>&1 || true

# Ensure we use the current user's name if the env var isn't set
CURRENT_USER=${USER:-$(whoami)}

docker run \
   --interactive \
   --tty \
   --detach \
   --env "TERM=xterm-256color" \
   --env "KEEP_SUDO=" \
   --env "USER_ID=$(id -u)" \
   --env "USER_NAME=$CURRENT_USER" \
   --mount type=bind,source="$(pwd)",target="$(pwd)" \
   --ulimit nofile=1024:1024 \
   --workdir "$(pwd)" \
   --name rv_tools_image_cont_example \
   ghcr.io/riscv-technologies-lab/rv_tools_image:1.0.10


echo "Container started. Entering shell with RISC-V tools in PATH..."

docker exec -it rv_tools_image_cont_example bash -c "export PATH=\$PATH:/opt/sc-dt/riscv-gcc/bin:/opt/sc-dt/tools/bin && bash"


# commands:
# riscv64-unknown-elf-objdump -d sum.elf
# riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -o sum.elf sum.s

# qemu-riscv32 -d cpu,in_asm -one-insn-per-tb -D qemu_trace.log sum.elf


