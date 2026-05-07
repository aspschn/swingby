SKIA_ARGS='is_official_build=true skia_use_egl=true skia_use_libwebp_decode=false skia_use_libwebp_encode=false skia_enable_pdf=false skia_enable_skottie=false skia_enable_svg=false skia_use_dng_sdk=false skia_use_wuffs=false'

build-skia: bin/gn
	cd skia && bin/gn gen out/Static --args=$(SKIA_ARGS)
	ninja -C skia/out/Static

bin/gn:
	cd skia && bin/fetch-gn
