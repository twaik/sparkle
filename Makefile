
all: build

#===============================================================================

configure-were:
	(cd were;			./configure --prefix=/usr)

configure-drivers: configure-were
	(cd xf86-video-sparkle;		./configure --prefix=/usr)
	(cd xf86-input-sparklei;	./configure --prefix=/usr)
	(cd pcm-sparkle; 		./configure --prefix=/usr)

configure-pc: configure-were
	(cd pc;				./configure --prefix=/usr)

configure: configure-drivers configure-pc

#===============================================================================

build-were:
	make -C were

build-drivers: build-were
	make -C xf86-video-sparkle
	make -C xf86-input-sparklei
	make -C pcm-sparkle

build-pc: build-were
	make -C pc

build: build-drivers build-pc

#===============================================================================

clean:
	-make -C were clean
	-make -C xf86-video-sparkle clean
	-make -C xf86-input-sparklei clean
	-make -C pcm-sparkle clean
	-make -C pc clean
	-make -C apk clean

#===============================================================================

maintainer-clean:
	-make -C were maintainer-clean
	-make -C xf86-video-sparkle maintainer-clean
	-make -C xf86-input-sparklei maintainer-clean
	-make -C pcm-sparkle maintainer-clean
	-make -C pc maintainer-clean
	-make -C apk clean
	
#===============================================================================

install-drivers: build-drivers
	make -C xf86-video-sparkle install
	make -C xf86-input-sparklei install
	make -C pcm-sparkle install

#===============================================================================

.PHONY: all \
configure-were configure-drivers configure-pc configure \
build-were build-drivers build-pc build \
clean maintainer-clean install-drivers

