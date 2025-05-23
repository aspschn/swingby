#!/bin/sh

STABLE_DIR=/usr/share/wayland-protocols/stable
STABLE_TARGET_DIR=include/wayland-protocols/stable

STAGING_DIR=/usr/share/wayland-protocols/staging
STAGING_TARGET_DIR=include/wayland-protocols/staging

UNSTABLE_DIR=/usr/share/wayland-protocols/unstable
UNSTABLE_TARGET_DIR=include/wayland-protocols/unstable

wayland-scanner client-header $STABLE_DIR/xdg-shell/xdg-shell.xml $STABLE_TARGET_DIR/xdg-shell.h
wayland-scanner private-code  $STABLE_DIR/xdg-shell/xdg-shell.xml $STABLE_TARGET_DIR/xdg-shell.c

wayland-scanner client-header $STABLE_DIR/tablet/tablet-v2.xml $STABLE_TARGET_DIR/tablet-v2.h
wayland-scanner private-code  $STABLE_DIR/tablet/tablet-v2.xml $STABLE_TARGET_DIR/tablet-v2.c

wayland-scanner client-header $STAGING_DIR/cursor-shape/cursor-shape-v1.xml $STAGING_TARGET_DIR/cursor-shape-v1.h
wayland-scanner private-code  $STAGING_DIR/cursor-shape/cursor-shape-v1.xml $STAGING_TARGET_DIR/cursor-shape-v1.c

wayland-scanner client-header $UNSTABLE_DIR/text-input/text-input-unstable-v3.xml $UNSTABLE_TARGET_DIR/text-input-unstable-v3.h
wayland-scanner private-code  $UNSTABLE_DIR/text-input/text-input-unstable-v3.xml $UNSTABLE_TARGET_DIR/text-input-unstable-v3.c
