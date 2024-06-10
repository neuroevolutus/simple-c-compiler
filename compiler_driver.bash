#!/usr/bin/env bash

set -euo pipefail

function parse_file_name {
  if (( $# == 2 )); then
    echo "${2}"
  else
    echo "${1}"
  fi
}

function print_usage_message {
  >&2 echo "error: wrong number of arguments"
  >&2 echo "  usage: compiler_driver.bash [-(-(lex|parse|codegen|tacky|validate)|S)] /path/to/program.c"
}

function exit_with_usage_message {
  print_usage_message
  exit 1
}

function parse_option {
  if (( $# == 1)); then
    echo ""
  else
    while getopts ":-:S" OPTION; do
      case "${OPTION}" in
	S ) echo "S";;
	- )
	  case "${OPTARG}" in
	    lex ) echo --lex;;
	    parse ) echo --parse;;
	    codegen ) echo --codegen;;
	    tacky ) echo --tacky;;
	    validate ) echo --validate;;
	    * ) exit_with_usage_message;;
          esac
	  ;;
	: ) exit_with_usage_message;;
	* ) exit_with_usage_message;;
      esac
    done
  fi
}

function main {
  if (( $# > 2 )); then
    exit_with_usage_message
  fi
  FILE_NAME="$(parse_file_name "$@")"
  OPTION="$(parse_option "$@")"
  FILE_NAME_WITHOUT_EXTENSION="${FILE_NAME%.c}"
  PREPROCESSED_FILE="${FILE_NAME_WITHOUT_EXTENSION}.i"
  ASSEMBLY_FILE="${FILE_NAME_WITHOUT_EXTENSION}.s"
  gcc -E -P "${FILE_NAME}" -o "${PREPROCESSED_FILE}"
  if [ -n "${OPTION}" ]; then
     "${HOME}/Documents/simple-c-compiler/g++-build/sc2" "${OPTION}" "${PREPROCESSED_FILE}";
  else
     "${HOME}/Documents/simple-c-compiler/g++-build/sc2" "${PREPROCESSED_FILE}";
  fi
  EXIT_CODE="$?"
  if [ -n "${OPTION}" ]; then
    exit "${EXIT_CODE}"
  fi
  gcc "${ASSEMBLY_FILE}" -o "${FILE_NAME_WITHOUT_EXTENSION}"
  EXIT_CODE="$?"
  rm "${PREPROCESSED_FILE}"
  rm "${ASSEMBLY_FILE}"
  exit "${EXIT_CODE}"
}

main "$@"

