/** AFR2-20261108-07. Product version compiled into the ATmega/ESP, read from the shared
 * Contracts header (same source as AFR2_FIRMWARE_VERSION); never a guess. */
import buildInfo from '../../../../AutoFinalRobo2-Contracts/contracts/afr2_build_info.h?raw';
export const FIRMWARE_VERSION = /#define\s+AFR2_FIRMWARE_VERSION\s+"([^"]+)"/.exec(buildInfo)?.[1] ?? '--';
