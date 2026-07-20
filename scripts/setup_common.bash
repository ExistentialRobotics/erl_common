#!/usr/bin/env bash
#
# Shared dependency-installation helpers for erl_common.
# This file is *sourced* by the per-release setup_ubuntu_<version>.bash scripts;
# it is not meant to be executed directly.
#
# The functions here install the base C++ / Python build dependencies that
# erl_common itself needs (build tools, Intel MKL / LAPACK, Eigen, plplot,
# yaml-cpp, boost, OpenCV, abseil, ...). Downstream packages layer their own
# extra dependencies on top: erl_geometry, for example, adds nanoflann, qhull
# and Open3D via its own scripts/setup_common.bash.

set -euo pipefail

# Use sudo only when we are not already root (works both in bare containers and
# on a normal user machine).
if [ "$(id -u)" -eq 0 ]; then SUDO=""; else SUDO="sudo"; fi
export DEBIAN_FRONTEND=noninteractive

NPROC="${SETUP_NPROC:-$(nproc)}"   # override with SETUP_NPROC to cap build parallelism

# Pinned third-party versions.
ABSEIL_TAG="20240722.1"

log() { echo -e "\n\033[1;34m==> $*\033[0m"; }

apt_update() { $SUDO apt-get update; }

apt_install() { $SUDO apt-get install -y "$@"; }

# The plplot Ada runtime package carries a version suffix that drifts across
# releases (libplplotada4 on 20.04, 4.1 on 22.04, 4.2 on 24.04/26.04, ...).
# Detect the highest available one instead of hardcoding it per release.
detect_plplot_ada() {
    apt-cache pkgnames 2>/dev/null | grep -E '^libplplotada[0-9]' | sort -V | tail -1
}

# Base C++ / Python build dependencies common to all supported Ubuntu releases.
#   $@ (opt): extra release-specific packages (e.g. libabsl-dev, octave-plplot)
install_base_apt() {
    local plplot_ada; plplot_ada="$(detect_plplot_ada || true)"
    log "Installing base apt dependencies (plplot-ada: ${plplot_ada:-none found})"
    apt_install \
        sudo apt-utils software-properties-common lsb-release ca-certificates \
        wget curl git zsh vim build-essential cmake ccache pkg-config \
        python3 python3-pip python3-dev \
        intel-mkl-full liblapack-dev liblapacke-dev libomp-dev libeigen3-dev \
        libfmt-dev libboost-all-dev nlohmann-json3-dev libopencv-dev \
        libplplot-dev libplplot-java libplplot-data ${plplot_ada:+"${plplot_ada}"} \
        plplot-tcl-dev plplot-driver-cairo python3-plplot libcairomm-1.0-dev \
        libyaml-cpp-dev pybind11-dev libgtest-dev libjsoncpp-dev \
        libzip-dev zipcmp zipmerge ziptool \
        "$@"
}

# plplot ships broken python .so stubs; touch them so downstream imports/find_package
# do not fail. (Harmless if the files already exist.)
patch_plplot() {
    log "Patching plplot python stubs"
    $SUDO touch /usr/lib/python3/dist-packages/_Pltk_init.so \
                /usr/lib/python3/dist-packages/_plplotc.so \
                /usr/lib/python3/dist-packages/plplot_pyqt5.so \
                /usr/bin/pltek 2>/dev/null || true

    # plplot's cmake package (24.04+) hard-fails find_package(plplot) via a
    # FATAL_ERROR when an imported binding target references a file that is not
    # installed — e.g. the Octave .oct (needs the heavy octave-plplot) or the
    # static Ada lib libplplotada_static.a (not packaged at all). erl uses none of
    # those bindings (only the cairo driver), so drop the export fragments whose
    # referenced files are missing. Release-independent (adapts to whatever is absent).
    local dir cfg base p missing
    for dir in /usr/lib/*/cmake/plplot; do
        [ -d "${dir}" ] || continue
        for cfg in "${dir}"/export_*-*.cmake; do
            [ -e "${cfg}" ] || continue
            missing=0
            while read -r p; do
                case "${p}" in /*) [ -e "${p}" ] || missing=1 ;; esac
            done < <(grep -oE '"/[^"]+"' "${cfg}" | tr -d '"')
            if [ "${missing}" -eq 1 ]; then
                base="${cfg%-*.cmake}"
                log "Removing broken plplot export fragment: $(basename "${base}")"
                $SUDO rm -f "${base}.cmake" "${base}"-*.cmake
            fi
        done
    done
}

# Older yaml-cpp (20.04) ships a cmake config with a wrong relative include path.
patch_yaml_cpp_config() {
    log "Patching yaml-cpp cmake config include path"
    local cfg=/usr/lib/x86_64-linux-gnu/cmake/yaml-cpp/yaml-cpp-config.cmake
    [ -f "${cfg}" ] && $SUDO sed -i 's#\.\./\.\./\.\./\.\./\.\./include#../../../../include#g' "${cfg}" || true
}

enable_openmp_spincount() {
    grep -q GOMP_SPINCOUNT /etc/profile 2>/dev/null || \
        echo "export GOMP_SPINCOUNT=1" | $SUDO tee -a /etc/profile >/dev/null
}

# abseil-cpp is not packaged on Ubuntu 20.04 (no libabsl-dev), so build it from
# source there. On 22.04+ install libabsl-dev via apt and skip this.
install_abseil_from_source() {
    if [ -d /usr/local/include/absl ] || [ -f /usr/local/lib/cmake/absl/abslConfig.cmake ]; then
        log "abseil-cpp already installed under /usr/local, skipping"; return
    fi
    log "Building abseil-cpp ${ABSEIL_TAG} (no libabsl-dev on this release)"
    local d; d="$(mktemp -d)"
    (
        cd "${d}"
        git clone --depth 1 --branch "${ABSEIL_TAG}" https://github.com/abseil/abseil-cpp.git
        cmake -S abseil-cpp -B abseil-cpp/build -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_CXX_STANDARD=17 -DBUILD_SHARED_LIBS=ON -DABSL_PROPAGATE_CXX_STD=ON \
            -DCMAKE_CXX_FLAGS="-DNDEBUG"
        cmake --build abseil-cpp/build -j"${NPROC}"
        $SUDO cmake --install abseil-cpp/build
    )
    rm -rf "${d}"
}
