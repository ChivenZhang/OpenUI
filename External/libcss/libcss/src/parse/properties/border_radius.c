/*
 * This file is part of LibCSS.
 * Licensed under the MIT License,
 *		  http://www.opensource.org/licenses/mit-license.php
 * Copyright 2017 ZhuYaDong <aeix@foxmail.com>
 */

#include <assert.h>
#include <string.h>

#include "bytecode/bytecode.h"
#include "bytecode/opcodes.h"
#include "parse/properties/properties.h"
#include "parse/properties/utils.h"

/**
 * Parse border-radius shorthand
 *
 * \param c	  Parsing context
 * \param vector  Vector of tokens to process
 * \param ctx	  Pointer to vector iteration context
 * \param result  Pointer to location to receive resulting style
 * \return CSS_OK on success,
 *	   CSS_NOMEM on memory exhaustion,
 *	   CSS_INVALID if the input is not valid
 *
 * Post condition: \a *ctx is updated with the next token to process
 *		   If the input is invalid, then \a *ctx remains unchanged.
 */
css_error css__parse_border_radius(css_language *c,
                                   const parserutils_vector *vector, int *ctx,
                                   css_style *result)
{
  /* we don't support '/' split value of border-radius */
  int orig_ctx = *ctx;
  int prev_ctx;
  const css_token *token;
  uint16_t val[4];
  css_fixed radius[4];
  uint32_t unit[4];
  uint32_t count = 0;
  bool match;
  css_error error;

  /* Firstly, handle inherit */
  token = parserutils_vector_peek(vector, *ctx);
  if (token == NULL)
    return CSS_INVALID;

  if (is_css_inherit(c, token)) {
    error = css_stylesheet_style_inherit(result, CSS_PROP_BORDER_TOP_LEFT_RADIUS);
    if (error != CSS_OK)
      return error;

    error = css_stylesheet_style_inherit(result, CSS_PROP_BORDER_TOP_RIGHT_RADIUS);
    if (error != CSS_OK)
      return error;

    error = css_stylesheet_style_inherit(result, CSS_PROP_BORDER_BOTTOM_RIGHT_RADIUS);
    if (error != CSS_OK)
      return error;

    error = css_stylesheet_style_inherit(result, CSS_PROP_BORDER_BOTTOM_LEFT_RADIUS);
    if (error == CSS_OK)
      parserutils_vector_iterate(vector, ctx);

    return error;
  }

  /* Attempt to parse up to 4 radius */
  do {
    prev_ctx = *ctx;

    if ((token != NULL) && (token->type == CSS_TOKEN_IDENT || is_css_inherit(c, token))) {
      *ctx = orig_ctx;
      return CSS_INVALID;
    }

    val[count] = BORDER_RADIUS_SET;

    error = css__parse_unit_specifier(c, vector, ctx, UNIT_PX, &radius[count], &unit[count]);
    if (error == CSS_OK) {
      if (unit[count] & UNIT_ANGLE||
          unit[count] & UNIT_TIME||
          unit[count] & UNIT_FREQ) {
        *ctx = orig_ctx;
        return CSS_INVALID;
      }

      ++count;

      consumeWhitespace(vector, ctx);

      token = parserutils_vector_peek(vector, *ctx);
    } else {
      token = NULL;
    }
  } while ((*ctx != prev_ctx) && (token != NULL) && (count < 4));

#define RADIUS_APPEND(OP,NUM) \
    error = css__stylesheet_style_appendOPV(result, (OP), 0, val[(NUM)]); \
    if (error != CSS_OK)  \
      break;  \
    if (val[(NUM)] == BORDER_RADIUS_SET) {  \
      error = css__stylesheet_style_append(result, radius[(NUM)]);  \
      if (error != CSS_OK)  \
        break;  \
      error = css__stylesheet_style_append(result, unit[(NUM)]);  \
      if (error != CSS_OK)  \
        break;  \
    }

  switch (count) {
    case 1:
      RADIUS_APPEND(CSS_PROP_BORDER_TOP_LEFT_RADIUS, 0);
      RADIUS_APPEND(CSS_PROP_BORDER_TOP_RIGHT_RADIUS, 0);
      RADIUS_APPEND(CSS_PROP_BORDER_BOTTOM_RIGHT_RADIUS, 0);
      RADIUS_APPEND(CSS_PROP_BORDER_BOTTOM_LEFT_RADIUS, 0);
      break;
    case 2:
      RADIUS_APPEND(CSS_PROP_BORDER_TOP_LEFT_RADIUS, 0);
      RADIUS_APPEND(CSS_PROP_BORDER_TOP_RIGHT_RADIUS, 1);
      RADIUS_APPEND(CSS_PROP_BORDER_BOTTOM_RIGHT_RADIUS, 0);
      RADIUS_APPEND(CSS_PROP_BORDER_BOTTOM_LEFT_RADIUS, 1);
      break;
    case 3:
      RADIUS_APPEND(CSS_PROP_BORDER_TOP_LEFT_RADIUS, 0);
      RADIUS_APPEND(CSS_PROP_BORDER_TOP_RIGHT_RADIUS, 1);
      RADIUS_APPEND(CSS_PROP_BORDER_BOTTOM_RIGHT_RADIUS, 2);
      RADIUS_APPEND(CSS_PROP_BORDER_BOTTOM_LEFT_RADIUS, 1);
      break;
    case 4:
      RADIUS_APPEND(CSS_PROP_BORDER_TOP_LEFT_RADIUS, 0);
      RADIUS_APPEND(CSS_PROP_BORDER_TOP_RIGHT_RADIUS, 1);
      RADIUS_APPEND(CSS_PROP_BORDER_BOTTOM_RIGHT_RADIUS, 2);
      RADIUS_APPEND(CSS_PROP_BORDER_BOTTOM_LEFT_RADIUS, 3);
      break;
    default:
      error = CSS_INVALID;
  }

  if (error != CSS_OK)
    *ctx = orig_ctx;

  return error;
}
