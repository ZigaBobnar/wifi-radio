#!/bin/sh
PROJECT_NAME="${1}"
PROJECT_DIR="$(dirname ${2})"
NEW_NAME="${3}"

if [ ! -d "${PROJECT_DIR}"/"${NEW_NAME}" ]; then
  echo "Cloning to >> ${NEW_NAME} << ..."
  make clean -e PN="${PROJECT_NAME}" > /dev/null 2>&1
  mkdir "${PROJECT_DIR}"/"${NEW_NAME}"
  if [ -d "${PROJECT_DIR}"/"${NEW_NAME}" ]; then
    find . | cpio -pmd "${PROJECT_DIR}"/"${NEW_NAME}" > /dev/null 2>&1
    cd "${PROJECT_DIR}"/"${NEW_NAME}"
    mv "${PROJECT_NAME}".cbp "${NEW_NAME}".cbp
    mv "${PROJECT_NAME}".layout "${NEW_NAME}".layout
    sed -i "s/title=\"${PROJECT_NAME}\"/title=\"${NEW_NAME}\"/g" "${NEW_NAME}".cbp
    sed -i "s/output=\"${PROJECT_NAME}.elf\"/output=\"${NEW_NAME}.elf\"/g" "${NEW_NAME}".cbp
  fi
else
  echo "ERROR! Project with the name >> ${NEW_NAME} << already exists."
fi
