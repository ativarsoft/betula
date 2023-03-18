#!/bin/bash -e

if test "$1" == "install"
then
wget https://github.com/alire-project/alire/releases/download/v1.2.2/alr-1.2.2-x86_64.AppImage
chmod +x alr-1.2.2-x86_64.AppImage
ln -s alr-1.2.2-x86_64.AppImage alire
elif test "$1" == "remove"
then
rm -f alr-1.2.2-x86_64.AppImage
rm -f alire
else
echo "Usage: $0 [install|remove]"
fi
