SKIA_ARGS='is_official_build=true skia_use_egl=true skia_use_libwebp_decode=false skia_use_libwebp_encode=false'

build-skia: bin/gn
	cd skia && bin/gn gen out/Static --args=$(SKIA_ARGS)
	ninja -C skia/out/Static

bin/gn:
	cd skia && GIT_SYNC_DEPS_SKIP_EMSDK=1 python3 tools/git-sync-deps
