#ifndef CPR_CPRVER_H
#define CPR_CPRVER_H

/**
 * CPR version as a string.
 **/
#define CPR_VERSION "1.7.0"

/**
 * CPR version split up into parts.
 **/
#define CPR_VERSION_MAJOR 1
#define CPR_VERSION_MINOR 7
#define CPR_VERSION_PATCH 0

/**
 * CPR version as a single hex digit.
 * it can be split up into three parts:
 * 0xAABBCC
 * AA: The current CPR major version number in a hex format.
 * BB: The current CPR minor version number in a hex format.
 * CC: The current CPR patch version number in a hex format.
 *
 * Examples:
 * '0x010702' -> 01.07.02 -> CPR_VERSION: 1.7.2
 * '0xA13722' -> A1.37.22 -> CPR_VERSION: 161.55.34
 **/
#define CPR_VERSION_NUM 0x010702

#endif
