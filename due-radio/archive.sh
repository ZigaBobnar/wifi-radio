#!/bin/sh
PROJECT_NAME="${1}"
PROJECT_PATH="${2}"
PROJECT_DIR="$(dirname ${2})"
PROJECT_BASE="$(basename ${2})"
ARCHIVE_NAME="${PROJECT_NAME}_$(date +%Y-%m-%d-%H.%M).tar.gz"

echo " Creating archive >> ${ARCHIVE_NAME} << ..."
make clean -e PN="${PROJECT_NAME}" > /dev/null 2>&1
cd "${PROJECT_DIR}"
tar -czf "${ARCHIVE_NAME}" "${PROJECT_BASE}"
