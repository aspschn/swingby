#!/bin/sh

STABLE_DIR=/usr/share/wayland-protocols/stable
STABLE_TARGET_DIR=include/wayland-protocols/stable

wayland-scanner client-header $STABLE_DIR/xdg-shell/xdg-shell.xml $STABLE_TARGET_DIR/xdg-shell.h
wayland-scanner private-code  $STABLE_DIR/xdg-shell/xdg-shell.xml $STABLE_TARGET_DIR/xdg-shell.c

