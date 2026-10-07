/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "../VGTrueType.h"
#include "raqm.h"
#include <unicode/ubidi.h>
#include <unicode/uscript.h>
#include <unicode/unistr.h>
#include <freetype/ftoutln.h>
#include <freetype/ftbitmap.h>
#include <fontconfig/fontconfig.h>

static struct USING_FONTCONFIG
{
	USING_FONTCONFIG() { FcInit(); }
	~USING_FONTCONFIG() { FcFini(); }
} USING_FONTCONFIG;

static struct USING_FREETYPE
{
	USING_FREETYPE() { FT_Init_FreeType(&Library); }
	~USING_FREETYPE() { for (auto e : FaceMap) FT_Done_Face(e.second); FT_Done_FreeType(Library); }
	FT_Library Library;
	VGMap<VGString, FT_Face> FaceMap;
} USING_FREETYPE;

struct VGFontHash
{
	uint32_t Group;
	uint32_t Weight;
	uint32_t Style;
	uint32_t Size;

	bool operator < (VGFontHash const& e) const
	{
		if (Group != e.Group) return Group < e.Group;
		if (Weight != e.Weight) return Weight < e.Weight;
		if (Style != e.Style) return Style < e.Style;
		return Size < e.Size;
	}
};
static VGMap<VGFontHash, VGString> s_FontFileMap;

static auto split_func = [](const std::string& s, char delimiter)->VGList<VGString> {
	std::vector<std::string> tokens;
	std::string token;
	std::size_t start = 0;
	std::size_t end = s.find(delimiter);
	while (end != std::string::npos)
	{
		token = s.substr(start, end - start);
		tokens.push_back(token);
		start = end + 1;
		end = s.find(delimiter, start);
	}
	token = s.substr(start);
	if (!token.empty()) tokens.push_back(token);
	return tokens;
	};

static auto config_func = [](VGTextRaw element, VGString font)->VGString {
	auto filePath = VGString();
	auto weight = FC_WEIGHT_NORMAL;
	switch (element->getWeight())
	{
	case VGTextWeight::WeightThin: weight = FC_WEIGHT_THIN; break;
	case VGTextWeight::WeightUltralight: weight = FC_WEIGHT_ULTRALIGHT; break;
	case VGTextWeight::WeightLight: weight = FC_WEIGHT_LIGHT; break;
	case VGTextWeight::WeightSemilight: weight = FC_WEIGHT_SEMILIGHT; break;
	case VGTextWeight::WeightBook: weight = FC_WEIGHT_BOOK; break;
	case VGTextWeight::WeightNormal: weight = FC_WEIGHT_NORMAL; break;
	case VGTextWeight::WeightMedium: weight = FC_WEIGHT_MEDIUM; break;
	case VGTextWeight::WeightSemibold: weight = FC_WEIGHT_SEMIBOLD; break;
	case VGTextWeight::WeightBold: weight = FC_WEIGHT_BOLD; break;
	case VGTextWeight::WeightUltrabold: weight = FC_WEIGHT_ULTRABOLD; break;
	case VGTextWeight::WeightHeavy: weight = FC_WEIGHT_HEAVY; break;
	}
	auto style = FC_SLANT_ROMAN;
	switch (element->getStyle())
	{
	case VGTextStyle::StyleNormal: style = FC_SLANT_ROMAN; break;
	case VGTextStyle::StyleItalic: style = FC_SLANT_ITALIC; break;
	case VGTextStyle::StyleOblique: style = FC_SLANT_OBLIQUE; break;
	}
	FcConfig* config = nullptr;
	FcPattern* pattern = nullptr;
	FcPattern* match = nullptr;
	do
	{
		pattern = FcPatternCreate();
		if (!pattern) break;
		FcPatternAddString(pattern, FC_FAMILY, (const FcChar8*)font.c_str());
		FcPatternAddInteger(pattern, FC_WEIGHT, weight);
		FcPatternAddInteger(pattern, FC_SLANT, style);
		config = FcInitLoadConfigAndFonts();
		FcResult result;
		match = FcFontMatch(config, pattern, &result);
		if (match == nullptr) break;
		FcChar8* file;
		if (FcPatternGetString(match, FC_FILE, 0, &file) == FcResultMatch) filePath = (VGCString)file;
	} while (0);
	FcPatternDestroy(pattern);
	FcPatternDestroy(match);
	FcConfigDestroy(config);
	return filePath;
	};

bool VGTrueType::Path(VGTextRaw element, VGRect rect, VGString const& text, VGList<VGPoint>& inPoints, VGList<VGPointType>& inTypes)
{
	struct VGTextHash
	{
		uint32_t Group;
		uint32_t Glygh;
		uint32_t Weight;
		uint32_t Style;

		bool operator < (VGTextHash const& e) const
		{
			if (Group != e.Group) return Group < e.Group;
			if (Glygh != e.Glygh) return Glygh < e.Glygh;
			if (Weight != e.Weight) return Weight < e.Weight;
			return Style < e.Style;
		}
	};
	struct VGTextGlygh
	{
		VGList<VGPoint> Points;
		VGList<VGPointType> Types;
	};
	static VGMap<VGTextHash, VGRef<VGTextGlygh>> s_FontGlyphMap;

	auto fontPath = VGString();
	if (true)
	{
		VGFontHash key;
		key.Group = VGHash(element->getFamily().c_str());
		key.Weight = element->getWeight();
		key.Style = element->getStyle();
		auto result = s_FontFileMap.find(key);
		if (result == s_FontFileMap.end())
		{
			auto fontPath = config_func(element, element->getFamily());
			result = s_FontFileMap.emplace(key, fontPath).first;
		}
		fontPath = result->second;
	}
	if (fontPath.empty()) return false;

	FT_Library library = nullptr;
	FT_Face face = nullptr;

	do
	{
		// Step 2: Load the font file
		if (FT_New_Face(USING_FREETYPE.Library, fontPath.c_str(), 0, &face))
		{
			printf("Could not load font\n");
			break;
		}

		// Step 3: Set the character size
		FT_Set_Pixel_Sizes(face, 0, 100);

		// Step 4: Load the character glyph (e.g., 'A')

		raqm_t* raqm = raqm_create();
		if (raqm != NULL)
		{
			raqm_direction_t dir = RAQM_DIRECTION_DEFAULT;
			switch (element->getDirection())
			{
			case VGTextDirection::DirectionLeftToRight: dir = RAQM_DIRECTION_LTR; break;
			case VGTextDirection::DirectionRightToLeft: dir = RAQM_DIRECTION_RTL; break;
			default: dir = RAQM_DIRECTION_DEFAULT; break;
			}

			if (raqm_set_text_utf8(raqm, text.c_str(), text.length())
				&& raqm_set_freetype_face(raqm, face)
				&& raqm_set_par_direction(raqm, dir)
				&& raqm_set_language(raqm, "en", 0, text.length())
				&& raqm_layout(raqm))
			{
				size_t count;
				raqm_glyph_t* glyphs = raqm_get_glyphs(raqm, &count);
				if (glyphs == nullptr || count == 0) continue;

				VGPoint offset;
				for (size_t i = 0; i < count; ++i)
				{
					auto glyph_index = glyphs[i].index;
					auto offsetX = glyphs[i].x_offset;
					auto offsetY = glyphs[i].y_offset;
					auto advanceX = glyphs[i].x_advance;
					auto advanceY = glyphs[i].y_advance;

					VGTextHash keyGlyph;
					keyGlyph.Group = VGHash(element->getFamily().c_str());
					keyGlyph.Glygh = glyph_index;
					keyGlyph.Weight = element->getWeight();
					keyGlyph.Style = element->getStyle();
					auto result = s_FontGlyphMap.find(keyGlyph);
					if (result == s_FontGlyphMap.end())
					{
						if (FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_BITMAP))
						{
							printf("Could not load glyph\n");
							continue;
						}
						FT_GlyphSlot slot = face->glyph;
						FT_Outline& outline = slot->outline;
						auto advanceX = slot->metrics.horiAdvance;
						auto advanceY = slot->metrics.vertAdvance;

						struct VGTextGlygh2
						{
							VGList<VGPoint>* Points;
							VGList<VGPointType>* Types;
						} input;

						// Step 6: Process the outline
						VGList<VGPoint> newPoints;
						VGList<VGPointType> newTypes;

						input.Points = &newPoints;
						input.Types = &newTypes;

						auto MoveToFunction = [](const FT_Vector* to, void* user)->int {
							auto& newPoints = *((VGTextGlygh2*)user)->Points;
							auto& newTypes = *((VGTextGlygh2*)user)->Types;
							if (newPoints.size()) newTypes.emplace_back(VGPointType::Close);
							newPoints.push_back({ (float)to->x, (float)to->y });
							newTypes.emplace_back(VGPointType::MoveTo);
							return 0;
							};

						auto LineToFunction = [](const FT_Vector* to, void* user)->int {
							auto& newPoints = *((VGTextGlygh2*)user)->Points;
							auto& newTypes = *((VGTextGlygh2*)user)->Types;
							newPoints.push_back({ (float)to->x, (float)to->y });
							newTypes.emplace_back(VGPointType::LineTo);
							return 0;
							};

						auto ConicToFunction = [](const FT_Vector* ct, const FT_Vector* to, void* user)->int {
							auto& newPoints = *((VGTextGlygh2*)user)->Points;
							auto& newTypes = *((VGTextGlygh2*)user)->Types;
							newPoints.push_back({ (float)ct->x, (float)ct->y });
							newPoints.push_back({ (float)to->x, (float)to->y });
							newTypes.emplace_back(VGPointType::CurveTo);
							return 0;
							};

						auto CubicToFunction = [](const FT_Vector* ct1, const FT_Vector* ct2, const FT_Vector* to, void* user)->int {
							auto& newPoints = *((VGTextGlygh2*)user)->Points;
							auto& newTypes = *((VGTextGlygh2*)user)->Types;
							newPoints.push_back({ (float)ct1->x, (float)ct1->y });
							newPoints.push_back({ (float)ct2->x, (float)ct2->y });
							newPoints.push_back({ (float)to->x, (float)to->y });
							newTypes.emplace_back(VGPointType::CubicTo);
							return 0;
							};

						FT_Outline_Funcs callbacks;
						callbacks.move_to = MoveToFunction;
						callbacks.line_to = LineToFunction;
						callbacks.conic_to = ConicToFunction;
						callbacks.cubic_to = CubicToFunction;
						callbacks.shift = 0;
						callbacks.delta = 0;

						if (FT_Outline_Decompose(&outline, &callbacks, &input))
						{
							printf("Could not load glyph\n");
							continue;
						}

						for (size_t k = 0; k < newPoints.size(); ++k)
						{
							newPoints[k].Y = advanceY - newPoints[k].Y;
						}

						auto value = VGNew<VGTextGlygh>();
						value->Points = newPoints; value->Types = newTypes;
						s_FontGlyphMap.emplace(keyGlyph, value);
						result = s_FontGlyphMap.find(keyGlyph);
					}

					constexpr auto FT_SCALE = 1.0f / 64;
					auto& points = result->second->Points;
					auto& types = result->second->Types;

					for (size_t k = 0; k < points.size(); ++k)
					{
						inPoints.push_back({ (offset.X + offsetX + points[k].X) * FT_SCALE, (offset.Y + offsetY + points[k].Y) * FT_SCALE });
					}
					for (size_t k = 0; k < types.size(); ++k)
					{
						inTypes.push_back(types[k]);
					}
					inTypes.emplace_back(VGPointType::Close);
					offset.X += advanceX; offset.Y += advanceY;
				}
			}

			raqm_destroy(raqm);
		}

		// Clean up
		FT_Done_Face(face);
		return true;

	} while (0);

	// Clean up
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	return false;
}

bool VGTrueType::Fill(VGTextRaw element, VGRect client, VGString const& text, VGPrimitiveRaw result)
{
	struct VGTextHash
	{
		FT_Face Face;
		uint32_t Size;
		uint32_t Glyph;

		struct image_t
		{
			uint32_t Width, Height, Stride, OffsetX, OffsetY;
			VGList<uint8_t> Pixel;
		};

		bool operator < (VGTextHash const& e) const
		{
			if (Face != e.Face) return Face < e.Face;
			if (Size != e.Size) return Size < e.Size;
			return Glyph < e.Glyph;
		}
	};
	static VGMap<VGTextHash, VGTextHash::image_t> s_FontImageMap;

	auto& outPoints = result->PointList;
	auto& outStyles = result->StyleList;
	auto& outImages = result->ImageList;

	VGList<FT_Face> faceList;
	auto fontList = split_func(element->getFamily(), ',');
	for (size_t i = 0; i < fontList.size(); ++i)
	{
		VGFontHash key;
		key.Group = VGHash(fontList[i].c_str());
		key.Style = element->getStyle();
		key.Weight = element->getWeight();
		key.Size = element->getSize();
		auto font = VGString();
		auto result1 = s_FontFileMap.find(key);
		if (result1 == s_FontFileMap.end()) font = s_FontFileMap.emplace(key, config_func(element, fontList[i])).first->second;
		else font = result1->second;

		FT_Face face = nullptr;
		auto result2 = USING_FREETYPE.FaceMap.find(font);
		if (result2 == USING_FREETYPE.FaceMap.end())
		{
			if (FT_New_Face(USING_FREETYPE.Library, font.c_str(), 0, &face) == FT_Err_Ok)
			{
				USING_FREETYPE.FaceMap.emplace(font, face);
			}
		}
		else face = result2->second;
		if (face) faceList.push_back(face);
	}

	auto utf16Text = icu::UnicodeString::fromUTF8(text);

	struct bidirun_t
	{
		bool Layout = false;
		VGCString Script = nullptr;
		size_t Start = 0, Count = 0;
	};
	VGList<bidirun_t> runList;

	auto bidi = ubidi_open();
	do
	{
		UErrorCode errorCode = U_ZERO_ERROR;
		UBiDiLevel level = UBIDI_DEFAULT_LTR;
		switch (element->getDirection())
		{
		case VGTextDirection::DirectionLeftToRight: level = UBIDI_DEFAULT_LTR; break;
		case VGTextDirection::DirectionRightToLeft: level = UBIDI_DEFAULT_RTL; break;
		}
		ubidi_setPara(bidi, utf16Text.getBuffer(), utf16Text.length(), level, nullptr, &errorCode);
		if (U_FAILURE(errorCode)) break;
		auto runCount = ubidi_countRuns(bidi, &errorCode);
		if (U_FAILURE(errorCode)) break;
		for (int32_t i = 0; i < runCount; ++i)
		{
			int32_t start, length;
			auto direction = ubidi_getVisualRun(bidi, i, &start, &length);
			auto script = uscript_getScript(utf16Text[start], &errorCode);
			auto scriptName = uscript_getName(script);
			auto& run = runList.emplace_back();
			run.Start = start;
			run.Count = length;
			run.Script = scriptName;
		}
	} while (0);
	ubidi_close(bidi);

	auto raqm = raqm_create();
	do
	{
		auto direction = RAQM_DIRECTION_DEFAULT;
		switch (element->getDirection())
		{
		case VGTextDirection::DirectionLeftToRight: direction = RAQM_DIRECTION_LTR; break;
		case VGTextDirection::DirectionRightToLeft: direction = RAQM_DIRECTION_RTL; break;
		}
		if (raqm_set_par_direction(raqm, direction) == false) break;
		if (raqm_set_freetype_load_flags(raqm, FT_LOAD_NO_BITMAP) == false) break;
		if (raqm_set_text_utf16(raqm, (uint16_t*)utf16Text.getBuffer(), utf16Text.length()) == false) break;
		if (raqm_set_letter_spacing_range(raqm, element->getSpacing() * 64, 0, utf16Text.length()) == false) break;

		for (size_t i = 0; i < runList.size(); ++i)
		{
			auto run = runList[i];
			for (size_t k = run.Start; k < run.Start + run.Count;)
			{
				auto start = k;
				UChar32 codePoint = 0;
				U16_NEXT(utf16Text, k, utf16Text.length(), codePoint);
				auto count = k - start;

				FT_Face face = nullptr;
				for (size_t f = 0; f < faceList.size(); ++f)
				{
					if (FT_Get_Char_Index(face = faceList[f], codePoint)) break;
					face = nullptr;
				}
				if (face == nullptr) continue;
				if (FT_Set_Pixel_Sizes(face, 0, element->getSize())) continue;
				if (raqm_set_language(raqm, runList[i].Script, start, count) == false) continue;
				if (raqm_set_freetype_face_range(raqm, face, start, count) == false) continue;
			}
		}

		if (raqm_layout(raqm) == false) break;
		size_t count = 0;
		auto glyphs = raqm_get_glyphs(raqm, &count);
		if (count == 0 || glyphs == nullptr) break;

		auto baseline = 0;
		for (size_t i = 0; i < count; ++i)
		{
			auto face = glyphs[i].ftface;
			if (FT_Load_Glyph(face, glyphs[i].index, FT_LOAD_NO_BITMAP)) continue;
			baseline = std::max<float>(baseline, std::max<float>(0, face->glyph->metrics.height - face->glyph->metrics.horiBearingY));
		}
		for (size_t i = 0; i < count; ++i)
		{
			VGTextHash key;
			key.Face = glyphs[i].ftface;
			key.Size = element->getSize();
			key.Glyph = glyphs[i].index;
			auto result1 = s_FontImageMap.find(key);
			if (result1 == s_FontImageMap.end())
			{
				auto face = glyphs[i].ftface;
				auto slot = face->glyph;
				if (FT_Load_Glyph(face, glyphs[i].index, FT_LOAD_COLOR)) continue;
				if (FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL)) continue;
				auto& _image = s_FontImageMap.emplace(key, VGTextHash::image_t()).first->second;
				switch (slot->bitmap.pixel_mode)
				{
				case FT_PIXEL_MODE_MONO:
				case FT_PIXEL_MODE_GRAY2:
				case FT_PIXEL_MODE_GRAY4:
				{
					auto bitmap = &face->glyph->bitmap;
					FT_Bitmap converted_bitmap;
					FT_Bitmap_New(&converted_bitmap);
					do
					{
						if (FT_Bitmap_Convert(USING_FREETYPE.Library, bitmap, &converted_bitmap, 1)) break;
						int width = converted_bitmap.width;
						int height = converted_bitmap.rows;
						_image.Pixel.resize(width * height * 4);
						auto rgba_buffer = _image.Pixel.data();
						for (int y = 0; y < height; y++)
						{
							for (int x = 0; x < width; x++)
							{
								unsigned char grayscale = converted_bitmap.buffer[y * converted_bitmap.pitch + x];
								int index = (y * width + x) * 4;
								rgba_buffer[index] = 255;            // R
								rgba_buffer[index + 1] = 255;        // G
								rgba_buffer[index + 2] = 255;        // B
								rgba_buffer[index + 3] = grayscale ? 255 : 0;  // A (fully opaque for 1, transparent for 0)
							}
						}
					} while (0);
					FT_Bitmap_Done(USING_FREETYPE.Library, &converted_bitmap);
				} break;
				case FT_PIXEL_MODE_GRAY:
				{
					auto bitmap = &face->glyph->bitmap;
					int width = bitmap->width;
					int height = bitmap->rows;
					_image.Pixel.resize(width * height * 4);
					auto rgba_buffer = _image.Pixel.data();
					for (int y = 0; y < height; y++)
					{
						for (int x = 0; x < width; x++)
						{
							unsigned char grayscale = bitmap->buffer[y * bitmap->pitch + x];
							int index = (y * width + x) * 4;
							rgba_buffer[index] = 255;            // R
							rgba_buffer[index + 1] = 255;        // G
							rgba_buffer[index + 2] = 255;        // B
							rgba_buffer[index + 3] = grayscale;  // A (use grayscale value directly)
						}
					}
				} break;
				case FT_PIXEL_MODE_LCD:
				{
					auto bitmap = &face->glyph->bitmap;
					int width = bitmap->width / 3; // since each pixel has 3 components in FT_PIXEL_MODE_LCD
					int height = bitmap->rows;
					_image.Pixel.resize(width * height * 4);
					auto rgba_buffer = _image.Pixel.data();
					for (int y = 0; y < height; y++)
					{
						for (int x = 0; x < width; x++)
						{
							int src_index = y * bitmap->pitch + x * 3;
							int dst_index = (y * width + x) * 4;
							rgba_buffer[dst_index] = bitmap->buffer[src_index];       // R
							rgba_buffer[dst_index + 1] = bitmap->buffer[src_index + 1]; // G
							rgba_buffer[dst_index + 2] = bitmap->buffer[src_index + 2]; // B
							rgba_buffer[dst_index + 3] = 255;                         // A (fully opaque)
						}
					}
				} break;
				case FT_PIXEL_MODE_LCD_V:
				{
					auto bitmap = &face->glyph->bitmap;
					int width = bitmap->width;
					int height = bitmap->rows / 3;  // since each pixel has 3 components in FT_PIXEL_MODE_LCD_V
					_image.Pixel.resize(width * height * 4);
					auto rgba_buffer = _image.Pixel.data();
					for (int y = 0; y < height; y++)
					{
						for (int x = 0; x < width; x++)
						{
							int src_index = (y * 3) * bitmap->pitch + x;
							int dst_index = (y * width + x) * 4;
							rgba_buffer[dst_index] = bitmap->buffer[src_index + 2 * bitmap->pitch]; // R
							rgba_buffer[dst_index + 1] = bitmap->buffer[src_index + bitmap->pitch]; // G
							rgba_buffer[dst_index + 2] = bitmap->buffer[src_index];                // B
							rgba_buffer[dst_index + 3] = 255;                                      // A (fully opaque)
						}
					}
				} break;
				case FT_PIXEL_MODE_BGRA:
				{
					auto bitmap = &face->glyph->bitmap;
					int width = bitmap->width;
					int height = bitmap->rows;
					_image.Pixel.resize(width * height * 4);
					auto rgba_buffer = _image.Pixel.data();
					for (int y = 0; y < height; y++)
					{
						for (int x = 0; x < width; x++)
						{
							int src_index = y * bitmap->pitch + x * 4;
							int dst_index = (y * width + x) * 4;
							rgba_buffer[dst_index] = bitmap->buffer[src_index + 2];   // R
							rgba_buffer[dst_index + 1] = bitmap->buffer[src_index + 1]; // G
							rgba_buffer[dst_index + 2] = bitmap->buffer[src_index];   // B
							rgba_buffer[dst_index + 3] = bitmap->buffer[src_index + 3]; // A
						}
					}
				} break;
				}
				_image.Width = slot->bitmap.width;
				_image.Height = slot->bitmap.rows;
				_image.Stride = _image.Width * 4;	// has converted into RGBA format uniformly
				_image.OffsetX = slot->metrics.horiBearingX;
				_image.OffsetY = face->glyph->metrics.vertAdvance - face->glyph->metrics.horiBearingY;
			}
		}

		VGPoint offset;
		constexpr auto FT_SCALE = 1.0f / 64;
		for (size_t i = 0; i < count; ++i)
		{
			auto face = glyphs[i].ftface;
			auto offsetX = glyphs[i].x_offset;
			auto offsetY = glyphs[i].y_offset;
			auto advanceX = glyphs[i].x_advance;
			auto advanceY = glyphs[i].y_advance;

			if (client.W <= (offset.X + advanceX) * FT_SCALE && element->getLineWrap())
			{
				offset.X = 0;
				offset.Y += element->getSize() * 64 * (1.0f + element->getLineSpacing());
			}

			if (offset.Y * FT_SCALE <= client.H)
			{
				VGTextHash key3;
				key3.Face = glyphs[i].ftface;
				key3.Size = element->getSize();
				key3.Glyph = glyphs[i].index;
				auto result1 = s_FontImageMap.find(key3);
				if (result1 != s_FontImageMap.end())
				{
					auto& _image = result1->second;
					auto bearingX = result1->second.OffsetX;
					auto bearingY = result1->second.OffsetY;
					auto x = client.X + (offset.X + offsetX + bearingX) * FT_SCALE;
					auto y = client.Y + (offset.Y + offsetY + bearingY - baseline) * FT_SCALE;
					auto w = (float)_image.Width, h = (float)_image.Height;
					auto styleIndex = (int32_t)outStyles.size();
					outPoints.push_back({ x, y, 0, 0, styleIndex, 0 });
					outPoints.push_back({ x, y + h, 0, 1, styleIndex, 0 });
					outPoints.push_back({ x + w, y + h, 1, 1, styleIndex, 0 });
					outPoints.push_back({ x, y, 0, 0, styleIndex, 0 });
					outPoints.push_back({ x + w, y + h, 1, 1, styleIndex, 0 });
					outPoints.push_back({ x + w, y, 1, 0, styleIndex, 0 });
					auto& style = outStyles.emplace_back();
					style.Image = (int32_t)outImages.size();
					auto& image = outImages.emplace_back();
					image.Width = _image.Width;
					image.Height = _image.Height;
					image.Stride = _image.Stride;
					image.Pixels = _image.Pixel.data();
				}
			}
			offset.X += advanceX; offset.Y += advanceY;
		}

		raqm_destroy(raqm);
		return true;
	} while (0);

	raqm_destroy(raqm);
	return false;
}

bool VGTrueType::Stroke(VGTextRaw element, VGRect client, VGString const& text, VGPrimitiveRaw result)
{
	return false;
}

bool VGTrueType::Measure(VGTextRaw element, VGRect client, VGString const& text, float cursor, VGRect* cursorRect, VGRect& result)
{
	VGList<FT_Face> faceList;
	auto fontList = split_func(element->getFamily(), ',');
	for (size_t i = 0; i < fontList.size(); ++i)
	{
		VGFontHash key;
		key.Group = VGHash(fontList[i].c_str());
		key.Style = element->getStyle();
		key.Weight = element->getWeight();
		key.Size = element->getSize();
		auto font = VGString();
		auto result1 = s_FontFileMap.find(key);
		if (result1 == s_FontFileMap.end()) font = s_FontFileMap.emplace(key, config_func(element, fontList[i])).first->second;
		else font = result1->second;

		FT_Face face = nullptr;
		auto result2 = USING_FREETYPE.FaceMap.find(font);
		if (result2 == USING_FREETYPE.FaceMap.end())
		{
			if (FT_New_Face(USING_FREETYPE.Library, font.c_str(), 0, &face) == FT_Err_Ok)
			{
				USING_FREETYPE.FaceMap.emplace(font, face);
			}
		}
		else face = result2->second;
		if (face) faceList.push_back(face);
	}

	auto utf16Text = icu::UnicodeString::fromUTF8(text);

	struct bidirun_t
	{
		bool Layout = false;
		VGCString Script = nullptr;
		size_t Start = 0, Count = 0;
	};
	VGList<bidirun_t> runList;
	auto bidi = ubidi_open();
	do
	{
		UErrorCode errorCode = U_ZERO_ERROR;
		UBiDiLevel level = UBIDI_DEFAULT_LTR;
		switch (element->getDirection())
		{
		case VGTextDirection::DirectionLeftToRight: level = UBIDI_DEFAULT_LTR; break;
		case VGTextDirection::DirectionRightToLeft: level = UBIDI_DEFAULT_RTL; break;
		}
		ubidi_setPara(bidi, utf16Text.getBuffer(), utf16Text.length(), level, nullptr, &errorCode);
		if (U_FAILURE(errorCode)) break;
		auto runCount = ubidi_countRuns(bidi, &errorCode);
		if (U_FAILURE(errorCode)) break;
		for (int32_t i = 0; i < runCount; ++i)
		{
			int32_t start, length;
			auto direction = ubidi_getVisualRun(bidi, i, &start, &length);
			auto script = uscript_getScript(utf16Text[start], &errorCode);
			auto scriptName = uscript_getName(script);
			auto& run = runList.emplace_back();
			run.Start = start;
			run.Count = length;
			run.Script = scriptName;
		}
	} while (0);
	ubidi_close(bidi);

	auto raqm = raqm_create();
	do
	{
		auto direction = RAQM_DIRECTION_DEFAULT;
		switch (element->getDirection())
		{
		case VGTextDirection::DirectionLeftToRight: direction = RAQM_DIRECTION_LTR; break;
		case VGTextDirection::DirectionRightToLeft: direction = RAQM_DIRECTION_RTL; break;
		}
		if (raqm_set_par_direction(raqm, direction) == false) break;
		if (raqm_set_freetype_load_flags(raqm, FT_LOAD_NO_BITMAP) == false) break;
		if (raqm_set_text_utf16(raqm, (uint16_t*)utf16Text.getBuffer(), utf16Text.length()) == false) break;
		if (raqm_set_letter_spacing_range(raqm, element->getSpacing() * 64, 0, utf16Text.length()) == false) break;

		for (size_t i = 0; i < runList.size(); ++i)
		{
			auto run = runList[i];
			for (size_t k = run.Start; k < run.Start + run.Count;)
			{
				auto start = k;
				UChar32 codePoint = 0;
				U16_NEXT(utf16Text, k, utf16Text.length(), codePoint);
				auto count = k - start;

				FT_Face face = nullptr;
				for (size_t f = 0; f < faceList.size(); ++f)
				{
					if (FT_Get_Char_Index(face = faceList[f], codePoint)) break;
					face = nullptr;
				}
				if (face == nullptr) continue;
				if (FT_Set_Pixel_Sizes(face, 0, element->getSize())) continue;
				if (raqm_set_language(raqm, runList[i].Script, start, count) == false) continue;
				if (raqm_set_freetype_face_range(raqm, face, start, count) == false) continue;
			}
		}

		if (raqm_layout(raqm) == false) break;
		size_t count = 0;
		auto glyphs = raqm_get_glyphs(raqm, &count);
		if (count == 0 || glyphs == nullptr) break;

		VGPoint offset;
		constexpr auto FT_SCALE = 1.0f / 64;
		for (size_t i = 0; i < count; ++i)
		{
			auto face = glyphs[i].ftface;
			auto offsetX = glyphs[i].x_offset;
			auto offsetY = glyphs[i].y_offset;
			auto advanceX = glyphs[i].x_advance;
			auto advanceY = glyphs[i].y_advance;

			if (client.W <= (offset.X + advanceX) * FT_SCALE && element->getLineWrap())
			{
				offset.X = 0;
				offset.Y += element->getSize() * 64 * (1.0f + element->getLineSpacing());
			}
			offset.X += advanceX; offset.Y += advanceY;
		}

		auto region = client;
		region.W = (offset.X) * FT_SCALE;
		region.H = (offset.Y + element->getSize() * 64) * FT_SCALE;
		result = VGOverlap(client, region);

		if (cursorRect)
		{
			size_t index = cursor;
			int32_t posX = 0, posY = 0;
			if (raqm_index_to_position(raqm, &index, &posX, &posY) == false) break;
			*cursorRect = VGRect{ (float)posX / 64, (float)posY / 64, (float)0, (float)element->getSize() };
		}

		raqm_destroy(raqm);
		return true;
	} while (0);

	raqm_destroy(raqm);
	return false;
}

bool VGTrueType::Measure(VGTextRaw element, VGRect client, VGString const& text, float posX, float posY, float* cursor, VGRect* cursorRect, VGRect& result)
{
	VGList<FT_Face> faceList;
	auto fontList = split_func(element->getFamily(), ',');
	for (size_t i = 0; i < fontList.size(); ++i)
	{
		VGFontHash key;
		key.Group = VGHash(fontList[i].c_str());
		key.Style = element->getStyle();
		key.Weight = element->getWeight();
		key.Size = element->getSize();
		auto font = VGString();
		auto result1 = s_FontFileMap.find(key);
		if (result1 == s_FontFileMap.end()) font = s_FontFileMap.emplace(key, config_func(element, fontList[i])).first->second;
		else font = result1->second;

		FT_Face face = nullptr;
		auto result2 = USING_FREETYPE.FaceMap.find(font);
		if (result2 == USING_FREETYPE.FaceMap.end())
		{
			if (FT_New_Face(USING_FREETYPE.Library, font.c_str(), 0, &face) == FT_Err_Ok)
			{
				USING_FREETYPE.FaceMap.emplace(font, face);
			}
		}
		else face = result2->second;
		if (face) faceList.push_back(face);
	}

	auto utf16Text = icu::UnicodeString::fromUTF8(text);

	struct bidirun_t
	{
		bool Layout = false;
		VGCString Script = nullptr;
		size_t Start = 0, Count = 0;
	};
	VGList<bidirun_t> runList;
	auto bidi = ubidi_open();
	do
	{
		UErrorCode errorCode = U_ZERO_ERROR;
		UBiDiLevel level = UBIDI_DEFAULT_LTR;
		switch (element->getDirection())
		{
		case VGTextDirection::DirectionLeftToRight: level = UBIDI_DEFAULT_LTR; break;
		case VGTextDirection::DirectionRightToLeft: level = UBIDI_DEFAULT_RTL; break;
		}
		ubidi_setPara(bidi, utf16Text.getBuffer(), utf16Text.length(), level, nullptr, &errorCode);
		if (U_FAILURE(errorCode)) break;
		auto runCount = ubidi_countRuns(bidi, &errorCode);
		if (U_FAILURE(errorCode)) break;
		for (int32_t i = 0; i < runCount; ++i)
		{
			int32_t start, length;
			auto direction = ubidi_getVisualRun(bidi, i, &start, &length);
			auto script = uscript_getScript(utf16Text[start], &errorCode);
			auto scriptName = uscript_getName(script);
			auto& run = runList.emplace_back();
			run.Start = start;
			run.Count = length;
			run.Script = scriptName;
		}
	} while (0);
	ubidi_close(bidi);

	auto raqm = raqm_create();
	do
	{
		auto direction = RAQM_DIRECTION_DEFAULT;
		switch (element->getDirection())
		{
		case VGTextDirection::DirectionLeftToRight: direction = RAQM_DIRECTION_LTR; break;
		case VGTextDirection::DirectionRightToLeft: direction = RAQM_DIRECTION_RTL; break;
		}
		if (raqm_set_par_direction(raqm, direction) == false) break;
		if (raqm_set_freetype_load_flags(raqm, FT_LOAD_NO_BITMAP) == false) break;
		if (raqm_set_text_utf16(raqm, (uint16_t*)utf16Text.getBuffer(), utf16Text.length()) == false) break;
		if (raqm_set_letter_spacing_range(raqm, element->getSpacing() * 64, 0, utf16Text.length()) == false) break;

		for (size_t i = 0; i < runList.size(); ++i)
		{
			auto run = runList[i];
			for (size_t k = run.Start; k < run.Start + run.Count;)
			{
				auto start = k;
				UChar32 codePoint = 0;
				U16_NEXT(utf16Text, k, utf16Text.length(), codePoint);
				auto count = k - start;

				FT_Face face = nullptr;
				for (size_t f = 0; f < faceList.size(); ++f)
				{
					if (FT_Get_Char_Index(face = faceList[f], codePoint)) break;
					face = nullptr;
				}
				if (face == nullptr) continue;
				if (FT_Set_Pixel_Sizes(face, 0, element->getSize())) continue;
				if (raqm_set_language(raqm, runList[i].Script, start, count) == false) continue;
				if (raqm_set_freetype_face_range(raqm, face, start, count) == false) continue;
			}
		}

		if (raqm_layout(raqm) == false) break;
		size_t count = 0;
		auto glyphs = raqm_get_glyphs(raqm, &count);
		if (count == 0 || glyphs == nullptr) break;

		VGPoint offset;
		constexpr auto FT_SCALE = 1.0f / 64;
		for (size_t i = 0; i < count; ++i)
		{
			auto face = glyphs[i].ftface;
			auto offsetX = glyphs[i].x_offset;
			auto offsetY = glyphs[i].y_offset;
			auto advanceX = glyphs[i].x_advance;
			auto advanceY = glyphs[i].y_advance;

			if (client.W <= (offset.X + advanceX) * FT_SCALE && element->getLineWrap())
			{
				offset.X = 0;
				offset.Y += element->getSize() * 64 * (1.0f + element->getLineSpacing());
			}
			offset.X += advanceX; offset.Y += advanceY;
		}

		auto region = client;
		region.W = (offset.X) * FT_SCALE;
		region.H = (offset.Y + element->getSize() * 64) * FT_SCALE;
		result = VGOverlap(client, region);

		if (cursorRect)
		{
			size_t index = 0;
			if (raqm_position_to_index(raqm, posX * 64, posY * 64, &index) == false) break;
			int32_t _posX = 0, _posY = 0;
			if (raqm_index_to_position(raqm, &index, &_posX, &_posY) == false) break;
			if (cursor) *cursor = index;
			*cursorRect = VGRect{ (float)_posX / 64, (float)_posY / 64, (float)0, (float)element->getSize() };
		}

		raqm_destroy(raqm);
		return true;
	} while (0);

	raqm_destroy(raqm);
	return false;
}
