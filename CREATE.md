A Partir des fichiers du premier commit : 
./autogen.sh
./configure --with-gtk-sourceview
cd po
msginit -l en
msginit -l fr
cd ..
make


