#!/usr/bin/env bash
#
# Install erl_common's base build dependencies on Ubuntu. The target release is
# auto-detected from /etc/os-release, so this one script covers 20.04, 22.04,
# 24.04 and 26.04. Run from anywhere after cloning the repository:
#
#     bash scripts/setup_ubuntu.bash
#
# Uses sudo for apt / system installs when not run as root. Set SETUP_NPROC to
# cap build parallelism.
#
# Only 20.04 (Focal) needs special handling: abseil is not packaged there (built
# from source) and its yaml-cpp cmake config needs a patch. 22.04 and newer share
# the same base package set (the plplot-ada runtime package is auto-detected).
#
# Downstream packages (erl_geometry, ...) install their own extra dependencies
# on top of these via their own scripts/setup_ubuntu.bash.

set -euo pipefail
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# shellcheck source=setup_common.bash
source "${SCRIPT_DIR}/setup_common.bash"

# Detect the Ubuntu release (e.g. 20.04, 22.04, 24.04, 26.04).
UBUNTU_VERSION=""
[ -r /etc/os-release ] && UBUNTU_VERSION="$( . /etc/os-release && echo "${VERSION_ID:-}" )"
log "Detected Ubuntu release: ${UBUNTU_VERSION:-unknown}"

apt_update
if [ "${UBUNTU_VERSION}" = "20.04" ]; then
    # 20.04 (Focal): abseil is not packaged (no libabsl-dev), so it is built from
    # source below. (plplot-ada is auto-detected in install_base_apt.)
    install_base_apt octave-plplot
    patch_plplot
    patch_yaml_cpp_config
    enable_openmp_spincount
    install_abseil_from_source
else
    # 22.04 / 24.04 / 26.04 (and newer): abseil (libabsl-dev) is packaged.
    case "${UBUNTU_VERSION}" in
        22.04 | 24.04 | 26.04) : ;;
        *) log "Untested Ubuntu '${UBUNTU_VERSION:-unknown}'; proceeding with the 22.04+ path." ;;
    esac
    install_base_apt libabsl-dev python3-vedo python3-tqdm
    patch_plplot
    enable_openmp_spincount
fi

log "erl_common Ubuntu ${UBUNTU_VERSION:-} setup complete."
