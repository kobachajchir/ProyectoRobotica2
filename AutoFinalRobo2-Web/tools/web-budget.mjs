/** AFR2-20261024-04. User requested 3 MiB LittleFS on a candidate 4 MiB ESP.
 * Installed Arduino linker reserves EEPROM/RF sectors: actual FS is smaller
 * than nominal 3 MiB. This is a build policy, not physical flash detection. */
export const candidateLinkerScript = 'eagle.flash.4m3m.ld';
export const filesystemBudgetBytes = 2800 * 1024;
export const filesystemBlockBytes = 8192;
export const filesystemReserveBytes = 64 * 1024;
export const estimateFilesystemBytes = files => files.reduce((sum, file) =>
  sum + Math.ceil((file.gzipBytes + 512) / filesystemBlockBytes) * filesystemBlockBytes,
  filesystemReserveBytes);
