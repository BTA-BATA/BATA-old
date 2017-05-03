<<<<<<< HEAD
packages:=boost openssl libevent
darwin_packages:=zeromq
linux_packages:=zeromq
=======
packages:=boost openssl
>>>>>>> upstream/0.10
native_packages := native_ccache native_comparisontool

qt_native_packages = native_protobuf
qt_packages = qrencode protobuf

<<<<<<< HEAD
qt_x86_64_linux_packages:=qt expat dbus libxcb xcb_proto libXau xproto freetype fontconfig libX11 xextproto libXext xtrans
qt_i686_linux_packages:=$(qt_x86_64_linux_packages)
=======
qt46_linux_packages = qt46 expat dbus libxcb xcb_proto libXau xproto freetype libX11 xextproto libXext xtrans libICE libSM
qt5_linux_packages= qt expat dbus libxcb xcb_proto libXau xproto freetype fontconfig libX11 xextproto libXext xtrans
>>>>>>> upstream/0.10

qt_darwin_packages=qt
qt_mingw32_packages=qt

<<<<<<< HEAD
=======
qt_linux_$(USE_LINUX_STATIC_QT5):=$(qt5_linux_packages)
qt_linux_:=$(qt46_linux_packages)
qt_linux_packages:=$(qt_linux_$(USE_LINUX_STATIC_QT5))
>>>>>>> upstream/0.10

wallet_packages=bdb

upnp_packages=miniupnpc

<<<<<<< HEAD
darwin_native_packages = native_biplist native_ds_store native_mac_alias

ifneq ($(build_os),darwin)
darwin_native_packages += native_cctools native_cdrkit native_libdmg-hfsplus
=======
ifneq ($(build_os),darwin)
darwin_native_packages=native_cctools native_cdrkit native_libdmg-hfsplus
>>>>>>> upstream/0.10
endif
