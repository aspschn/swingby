build-skia:
	cd skia && bin/gn gen out/Static --args='is_official_build=true skia_use_egl=true'
	ninja -C skia/out/Static

bin/gn:
	cd skia && python3 tools/git-sync-deps
