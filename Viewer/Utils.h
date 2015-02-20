#ifndef GVS_VIEWER_UTILS_H
#define GVS_VIEWER_UTILS_H

#include <iostream>
#include <vector>

/** Get a list of filenames
 * @param path
 * @param extensions
 * @param filePathNames
 */
bool  GetFileNames( const std::string path, const std::vector<std::string> extensions,
                    std::vector<std::string> &filePathNames );


bool  FileEndsWith( const std::string filename, const std::string extension );

int   HaveNumInFilename( const std::string filename );

/** Get path of current executable.
 */
std::string GetProgPath();

#endif // GVS_VIEWER_UTILS_H

