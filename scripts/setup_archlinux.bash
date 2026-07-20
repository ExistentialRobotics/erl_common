#!/usr/bin/env bash
#
# Install erl_common's base build dependencies on Arch Linux (rolling).
# Run from anywhere after cloning the repository:
#
#     bash scripts/setup_archlinux.bash
#
# Uses sudo for pacman / system installs when not run as root. Set SETUP_NPROC
# to cap build parallelism. Set SETUP_PACMAN_ONLY=1 to install only the pacman
# packages and exit (handy for CI / quick validation without the AUR builds).
#
# Arch specifics vs. the Ubuntu scripts:
#   * Almost everything comes from pacman; only plplot is built from the AUR
#     (not in the official repos). Everything builds with the system gcc (>= 15).
#   * Intel MKL comes from the official intel-oneapi-toolkit meta-package (~9 GiB;
#     the full oneAPI toolkit) and installs under /opt, so we register its lib dir
#     with ldconfig for runtime linking. This keeps the Linux default
#     ERL_USE_INTEL_MKL=ON working; pass -DERL_USE_INTEL_MKL=OFF to fall back to
#     the system LAPACK/LAPACKE instead.
#
# Downstream packages (erl_geometry, ...) install their own extra dependencies
# on top of these via their own scripts/setup_archlinux.bash.

set -euo pipefail
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# Reuse the shared helpers: log(), NPROC, SUDO, enable_openmp_spincount. The
# apt-specific helpers in that file are not used here — Arch gets its deps from
# pacman/AUR instead.
# shellcheck source=setup_common.bash
source "${SCRIPT_DIR}/setup_common.bash"

# --------------------------------------------------------------------------- #
# pacman helpers
# --------------------------------------------------------------------------- #

pacman_sync() {
    # Full -Syu first: on a rolling distro, installing onto a partially-updated
    # system (plain -Sy) is unsupported and can pull mismatched libraries.
    log "Synchronizing and upgrading the system (pacman -Syu)"
    $SUDO pacman -Syu --noconfirm --needed
}

pacman_install() { $SUDO pacman -S --needed --noconfirm "$@"; }

# Official-repo build/runtime dependencies for erl_common.
install_base_pacman() {
    log "Installing base pacman dependencies"
    pacman_install \
        base-devel gcc cmake ccache pkgconf git wget curl zsh vim sudo \
        python python-pip python-numpy python-scipy python-matplotlib python-tqdm \
        intel-oneapi-toolkit lapack lapacke cblas openmp eigen \
        fmt boost boost-libs nlohmann-json opencv \
        yaml-cpp pybind11 gtest jsoncpp libzip cairomm \
        abseil-cpp
}

# intel-oneapi-toolkit installs MKL under /opt (outside the default linker search path);
# make its shared libraries findable at runtime. erl builds MKL as sequential+dynamic, so
# only the MKL libs are needed (no libiomp5).
register_mkl_ldconfig() {
    local libdir
    libdir="$(ls -d /opt/intel/oneapi/mkl/*/lib 2>/dev/null | sort -V | tail -1 || true)"
    [ -n "${libdir}" ] || { log "Intel MKL not found under /opt, skipping ldconfig"; return 0; }
    log "Registering Intel MKL lib dir with ldconfig: ${libdir}"
    echo "${libdir}" | $SUDO tee /etc/ld.so.conf.d/erl-intel-mkl.conf >/dev/null
    $SUDO ldconfig
}

# --------------------------------------------------------------------------- #
# AUR helpers (makepkg cannot run as root, so build under a throwaway user when needed)
# --------------------------------------------------------------------------- #

AUR_BUILD_USER="${SETUP_AUR_USER:-aurbuild}"
AUR_RUNAS=()   # command prefix used to drop to a non-root build user (empty => run as self)

ensure_aur_build_user() {
    if [ "$(id -u)" -ne 0 ]; then
        # Already a normal user: run makepkg directly; its `-s` will use `sudo pacman`
        # (may prompt for your password).
        AUR_RUNAS=()
        return
    fi
    if ! id "${AUR_BUILD_USER}" >/dev/null 2>&1; then
        log "Creating AUR build user '${AUR_BUILD_USER}' (makepkg refuses to run as root)"
        useradd -m -s /bin/bash "${AUR_BUILD_USER}"
    fi
    # makepkg -s installs build deps via pacman; grant the build user passwordless pacman.
    echo "${AUR_BUILD_USER} ALL=(ALL) NOPASSWD: /usr/bin/pacman" \
        | $SUDO tee /etc/sudoers.d/erl-aurbuild >/dev/null
    $SUDO chmod 0440 /etc/sudoers.d/erl-aurbuild
    AUR_RUNAS=(sudo -u "${AUR_BUILD_USER}")
}

# First AUR helper (paru/yay) found on PATH, or empty. Such helpers refuse to run as
# root, so they are only usable when the script runs as a normal user.
detect_aur_helper() {
    local h
    for h in paru yay; do
        command -v "${h}" >/dev/null 2>&1 && { echo "${h}"; return; }
    done
}

# aur_install <pkg> [pkg...] : ensure each AUR package is installed.
#   1. Skip packages already installed (checked BEFORE any build-user/helper setup, so an
#      up-to-date host is left untouched — no throwaway user, no sudoers edits, no rebuild).
#   2. For the rest, prefer an existing AUR helper (paru/yay) when running as a normal user.
#   3. Otherwise fall back to a manual git-clone + makepkg build (under a throwaway non-root
#      user when running as root, e.g. in a container).
aur_install() {
    local pkg missing=()
    for pkg in "$@"; do
        if pacman -Qi "${pkg}" >/dev/null 2>&1; then
            log "AUR package ${pkg} already installed, skipping"
        else
            missing+=("${pkg}")
        fi
    done
    [ "${#missing[@]}" -eq 0 ] && return 0

    local helper=""; [ "$(id -u)" -ne 0 ] && helper="$(detect_aur_helper)"
    if [ -n "${helper}" ]; then
        log "Installing AUR packages with ${helper}: ${missing[*]}"
        "${helper}" -S --needed --noconfirm "${missing[@]}"
        return
    fi

    ensure_aur_build_user
    local d
    for pkg in "${missing[@]}"; do
        log "Building AUR package from source: ${pkg}"
        d="$(mktemp -d)"
        git clone --depth 1 "https://aur.archlinux.org/${pkg}.git" "${d}/${pkg}"
        # The build dir must be writable by the (non-root) build user.
        [ "$(id -u)" -eq 0 ] && $SUDO chown -R "${AUR_BUILD_USER}:${AUR_BUILD_USER}" "${d}"
        ( cd "${d}/${pkg}" && "${AUR_RUNAS[@]}" makepkg -si --noconfirm --needed )
        rm -rf "${d}"
    done
}

# --------------------------------------------------------------------------- #
# main
# --------------------------------------------------------------------------- #

pacman_sync
install_base_pacman
register_mkl_ldconfig

if [ -n "${SETUP_PACMAN_ONLY:-}" ]; then
    log "SETUP_PACMAN_ONLY set — pacman packages installed, skipping AUR builds."
    exit 0
fi

enable_openmp_spincount

aur_install plplot             # required: ERL_USE_PLPLOT is ON by default (with cairomm)

# Optional AUR extra: vedo (python visualization). Non-fatal.
aur_install python-vedo || log "python-vedo AUR build failed (optional visualization dep); continuing"

log "erl_common Arch Linux setup complete."
