@echo off
setlocal

set SKIA_ARGS=is_official_build=true ^
    skia_use_libwebp_decode=false ^
    skia_use_libwebp_encode=false ^
    skia_use_system_libjpeg_turbo=false ^
    skia_use_system_zlib=false ^
    skia_use_system_harfbuzz=false ^
    skia_use_system_libpng=false ^
    skia_use_system_expat=false ^
    skia_use_system_icu=false

if not exist skia\bin\gn call :sync_deps

call :build_skia

endlocal
exit /b

:sync_deps
cd skia
set GIT_SYNC_DEPS_SKIP_EMSDK=1
python3 tools/git-sync-deps
cd ..
exit /b

:build_skia
cd skia
bin\gn gen out/Static --args="%SKIA_ARGS%"

ninja -C out/Static

cd ..
exit /b
