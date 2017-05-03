package=native_ccache
<<<<<<< HEAD
$(package)_version=3.2.5
$(package)_download_path=http://samba.org/ftp/ccache
$(package)_file_name=ccache-$($(package)_version).tar.bz2
$(package)_sha256_hash=7a553809e90faf9de3a23ee9c5b5f786cfd4836bf502744bedb824a24bee1097
=======
$(package)_version=3.1.9
$(package)_download_path=http://samba.org/ftp/ccache
$(package)_file_name=ccache-$($(package)_version).tar.bz2
$(package)_sha256_hash=04d3e2e438ac8d4cc4b110b68cdd61bd59226c6588739a4a386869467f5ced7c
>>>>>>> upstream/0.10

define $(package)_set_vars
$(package)_config_opts=
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

define $(package)_postprocess_cmds
  rm -rf lib include
endef
