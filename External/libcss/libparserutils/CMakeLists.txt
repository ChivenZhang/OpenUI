set(PARSER_UTILS_C_FILES
  src/charset/codecs/codec_8859.c
  src/charset/codecs/codec_ascii.c
  src/charset/codecs/codec_ext8.c
  src/charset/codecs/codec_utf16.c
  src/charset/codecs/codec_utf8.c
  src/charset/encodings/utf16.c
  src/charset/encodings/utf8.c
  src/charset/aliases.c
  src/charset/codec.c
  src/input/filter.c
  src/input/inputstream.c
  src/utils/buffer.c
  src/utils/errors.c
  src/utils/stack.c
  src/utils/vector.c)

include_directories(src/)
add_library(parserutils STATIC ${PARSER_UTILS_C_FILES})
