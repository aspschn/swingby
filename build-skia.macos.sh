#!/bin/sh

SKIA_ARGS='is_official_build=true
    target_cpu="arm64"
    skia_use_metal=true
    skia_use_libwebp_decode=false
    skia_use_libwebp_encode=false
    skia_use_icu=false
    skia_use_harfbuzz=false
    skia_enable_pdf=false
    skia_enable_skottie=false
    skia_enable_svg=false
    skia_use_system_libjpeg_turbo=false
    skia_use_system_zlib=false
    skia_use_system_libpng=false
    skia_use_system_expat=false'

sync_deps() {
    cd skia
    GIT_SYNC_DEPS_SKIP_EMSDK=1 python3 tools/git-sync-deps
    cd ..
}

build_skia() {
    cd skia
    bin/gn gen out/Static --args="$SKIA_ARGS"
    ninja -C out/Static
    cd ..
}

if [ ! -x skia/bin/gn ]; then
    sync_deps
fi

build_skia

exit 0
