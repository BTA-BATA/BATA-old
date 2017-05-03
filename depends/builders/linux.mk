build_linux_SHA256SUM = sha256sum
<<<<<<< HEAD
build_linux_DOWNLOAD = curl --location --fail --connect-timeout $(DOWNLOAD_CONNECT_TIMEOUT) --retry $(DOWNLOAD_RETRIES) -o
=======
build_linux_DOWNLOAD = wget --timeout=$(DOWNLOAD_CONNECT_TIMEOUT) --tries=$(DOWNLOAD_RETRIES) -nv -O
>>>>>>> upstream/0.10
