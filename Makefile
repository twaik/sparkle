
all: build

configure:
	(cd were;			./configure --prefix=/usr)
	(cd xf86-video-sparkle;		./configure --prefix=/usr)
	(cd xf86-input-sparklei;	./configure --prefix=/usr)
	(cd pcm-sparkle; 		./configure --prefix=/usr)
	(cd pc;				./configure --prefix=/usr)

build:
	make -C were
	make -C xf86-video-sparkle
	make -C xf86-input-sparklei
	make -C pcm-sparkle
	make -C pc
	
clean:
	-make -C were clean
	-make -C xf86-video-sparkle clean
	-make -C xf86-input-sparklei clean
	-make -C pcm-sparkle clean
	-make -C pc clean
	-make -C apk clean

maintainer-clean:
	-make -C were maintainer-clean
	-make -C xf86-video-sparkle maintainer-clean
	-make -C xf86-input-sparklei maintainer-clean
	-make -C pcm-sparkle maintainer-clean
	-make -C pc maintainer-clean
	-make -C apk clean

install: build
	make -C xf86-video-sparkle install
	make -C xf86-input-sparklei install
	make -C pcm-sparkle install


.PHONY: all configure build clean maintainer-clean install


