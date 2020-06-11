#!/bin/bash
echo "core" > release.linux.x86_64/modules.ini
echo "token = "${VK_TOKEN} > release.linux.x86_64/bot.ini
echo "debug = true" >> release.linux.x86_64/bot.ini
echo "num_workers = 8" >> release.linux.x86_64/bot.ini
cd release.linux.x86_64/
./vkbot_x86_64
