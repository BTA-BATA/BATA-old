package=native_cctools
<<<<<<< HEAD
$(package)_version=807d6fd1be5d2224872e381870c0a75387fe05e6
$(package)_download_path=https://github.com/theuni/cctools-port/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=a09c9ba4684670a0375e42d9d67e7f12c1f62581a27f28f7c825d6d7032ccc6a
$(package)_build_subdir=cctools
$(package)_clang_version=3.7.1
$(package)_clang_download_path=http://llvm.org/releases/$($(package)_clang_version)
$(package)_clang_download_file=clang+llvm-$($(package)_clang_version)-x86_64-linux-gnu-ubuntu-14.04.tar.xz
$(package)_clang_file_name=clang-llvm-$($(package)_clang_version)-x86_64-linux-gnu-ubuntu-14.04.tar.xz
$(package)_clang_sha256_hash=99b28a6b48e793705228a390471991386daa33a9717cd9ca007fcdde69608fd9
$(package)_extra_sources=$($(package)_clang_file_name)

=======
$(package)_version=ee31ae567931c426136c94aad457c7b51d844beb
$(package)_download_path=https://github.com/theuni/cctools-port/archive
$(package)_file_name=$($(package)_version).tar.gz
$(package)_sha256_hash=ef107e6ab1b3994cb22e14f4f5c59ea0c0b5a988e6b21d42ed9616b018bbcbf9
$(package)_build_subdir=cctools
$(package)_clang_version=3.3
$(package)_clang_download_path=http://llvm.org/releases/$($(package)_clang_version)
$(package)_clang_download_file=clang+llvm-$($(package)_clang_version)-amd64-Ubuntu-12.04.2.tar.gz
$(package)_clang_file_name=clang-llvm-$($(package)_clang_version)-amd64-Ubuntu-12.04.2.tar.gz
$(package)_clang_sha256_hash=60d8f69f032d62ef61bf527857ebb933741ec3352d4d328c5516aa520662dab7
>>>>>>> upstream/0.10
define $(package)_fetch_cmds
$(call fetch_file,$(package),$($(package)_download_path),$($(package)_download_file),$($(package)_file_name),$($(package)_sha256_hash)) && \
$(call fetch_file,$(package),$($(package)_clang_download_path),$($(package)_clang_download_file),$($(package)_clang_file_name),$($(package)_clang_sha256_hash))
endef

define $(package)_extract_cmds
<<<<<<< HEAD
  mkdir -p $($(package)_extract_dir) && \
  echo "$($(package)_sha256_hash)  $($(package)_source)" > $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  echo "$($(package)_clang_sha256_hash)  $($(package)_source_dir)/$($(package)_clang_file_name)" >> $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  $(build_SHA256SUM) -c $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  mkdir -p toolchain/bin toolchain/lib/clang/3.5/include && \
  tar --strip-components=1 -C toolchain -xf $($(package)_source_dir)/$($(package)_clang_file_name) && \
  rm -f toolchain/lib/libc++abi.so* && \
=======
  mkdir -p toolchain/bin toolchain/lib/clang/3.5/include && \
  tar --strip-components=1 -C toolchain -xf $($(package)_source_dir)/$($(package)_clang_file_name) && \
>>>>>>> upstream/0.10
  echo "#!/bin/sh" > toolchain/bin/$(host)-dsymutil && \
  echo "exit 0" >> toolchain/bin/$(host)-dsymutil && \
  chmod +x toolchain/bin/$(host)-dsymutil && \
  tar --strip-components=1 -xf $($(package)_source)
endef

define $(package)_set_vars
<<<<<<< HEAD
$(package)_config_opts=--target=$(host) --disable-lto-support
=======
$(package)_config_opts=--target=$(host) --disable-libuuid
>>>>>>> upstream/0.10
$(package)_ldflags+=-Wl,-rpath=\\$$$$$$$$\$$$$$$$$ORIGIN/../lib
$(package)_cc=$($(package)_extract_dir)/toolchain/bin/clang
$(package)_cxx=$($(package)_extract_dir)/toolchain/bin/clang++
endef

define $(package)_preprocess_cmds
  cd $($(package)_build_subdir); ./autogen.sh
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install && \
  cd $($(package)_extract_dir)/toolchain && \
  mkdir -p $($(package)_staging_prefix_dir)/lib/clang/$($(package)_clang_version)/include && \
  mkdir -p $($(package)_staging_prefix_dir)/bin $($(package)_staging_prefix_dir)/include && \
<<<<<<< HEAD
  cp bin/clang $($(package)_staging_prefix_dir)/bin/ &&\
  cp -P bin/clang++ $($(package)_staging_prefix_dir)/bin/ &&\
  cp lib/libLTO.so $($(package)_staging_prefix_dir)/lib/ && \
  cp -rf lib/clang/$($(package)_clang_version)/include/* $($(package)_staging_prefix_dir)/lib/clang/$($(package)_clang_version)/include/ && \
  cp bin/llvm-dsymutil $($(package)_staging_prefix_dir)/bin/$(host)-dsymutil && \
=======
  cp -P bin/clang bin/clang++ $($(package)_staging_prefix_dir)/bin/ &&\
  cp lib/libLTO.so $($(package)_staging_prefix_dir)/lib/ && \
  cp -rf lib/clang/$($(package)_clang_version)/include/* $($(package)_staging_prefix_dir)/lib/clang/$($(package)_clang_version)/include/ && \
  cp bin/$(host)-dsymutil $($(package)_staging_prefix_dir)/bin && \
>>>>>>> upstream/0.10
  if `test -d include/c++/`; then cp -rf include/c++/ $($(package)_staging_prefix_dir)/include/; fi && \
  if `test -d lib/c++/`; then cp -rf lib/c++/ $($(package)_staging_prefix_dir)/lib/; fi
endef
