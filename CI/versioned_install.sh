#!/bin/sh -e
#
# build the framework with all options and depedencies and
# install into a versioned, platform-specific directory.
#
# must be called from project root; install base as first argument

install_base=$(readlink -f $1)
source_dir=${PWD}
build_dir=$(mktemp -d)
version=$(git describe --always --dirty)

source ${source_dir}/CI/lcg_variables.sh

install_version=${install_base}/${version}
install_dir=${install_version}/${lcg_platform}
build_options="
  -DCMAKE_INSTALL_PREFIX=${install_dir}
  -DCMAKE_BUILD_TYPE=RELEASE
  -DUSE_DD4HEP=on
  -DUSE_GEANT4=on
  -DUSE_HEPMC3=on
  -DUSE_PYTHIA8=on
  -DUSE_TGEO=on"

echo "=== using sources from ${source_dir}"
echo "=== using ${lcg_release} on ${lcg_platform}"
echo "=== building in ${build_dir}"
echo "=== installing to ${install_dir}"

mkdir -p ${install_dir}
(
  cd ${build_dir}
  source ${lcg_view}/setup.sh

  cmake -GNinja ${build_options} ${source_dir}
  cmake --build . -- install

  # construct setup script
  cat ${source_dir}/CI/lcg_variables.sh > setup.sh
  echo >> setup.sh
  echo "# activate lcg release" >> setup.sh
  echo 'source ${lcg_view}/setup.sh' >> setup.sh
  echo >> setup.sh
  echo "# activate installation" >> setup.sh
  echo -n "source ${install_version}/" >> setup.sh
  echo '${lcg_platform}/bin/setup.sh' >> setup.sh

  # install setup script
  install -D --target-directory ${install_version} setup.sh
)
rm -rf ${build_dir}
