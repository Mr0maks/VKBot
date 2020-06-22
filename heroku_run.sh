#!/bin/bash
echo "core" > release.linux.x86_64/modules.ini
echo "openssl" >> release.linux.x86_64/modules.ini

echo "token = "${VK_TOKEN} > release.linux.x86_64/bot.ini
echo "debug = true" >> release.linux.x86_64/bot.ini
echo "num_workers = 32" >> release.linux.x86_64/bot.ini
cd release.linux.x86_64/
jemalloc.sh ./vkbot_x86_64
