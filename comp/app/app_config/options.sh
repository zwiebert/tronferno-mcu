#!/bin/sh

echo 'enum class otok : int8_t {\nNONE = -1,'
LC_COLLATE=C sort -u options.txt | tr '-' '_' | sed -E 's/^(.+)$/k_\1,/'
echo 'SIZE\n};'

echo 'typedef const char * constCharPtrT;'

echo 'constexpr constCharPtrT otok_strings[] = {'
LC_COLLATE=C sort -u options.txt | sed -E 's/^(.+)$/"\1",/'
echo '};'
