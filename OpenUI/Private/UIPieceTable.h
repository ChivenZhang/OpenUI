#pragma once
#include "../UI.h"

/// @brief Piece table by ChatGPT3.5
class UIPieceTable
{
public:
	UIPieceTable(const UIString& text = UIString())
	{
		originalBuffer = text;
		pieces.emplace_back(ORIGINAL, 0, text.length());
	}

	void insert(size_t position, const std::string& text)
	{
		size_t addStart = addBuffer.length();
		addBuffer.append(text);
		size_t addLength = text.length();

		UIList<Piece> newPieces;
		size_t currentPos = 0;

		for (auto& piece : pieces)
		{
			if (currentPos <= position && position < currentPos + piece.length)
			{
				if (currentPos < position)
				{
					size_t splitLength = position - currentPos;
					newPieces.emplace_back(piece.bufferType, piece.start, splitLength);
					piece.start += splitLength;
					piece.length -= splitLength;
				}
				newPieces.emplace_back(ADD, addStart, addLength);
				currentPos += addLength;
			}
			newPieces.push_back(piece);
			currentPos += piece.length;
		}

		if (currentPos <= position)
		{
			newPieces.emplace_back(ADD, addStart, addLength);
		}

		pieces = newPieces;
	}

	void remove(size_t position, size_t length)
	{
		UIList<Piece> newPieces;
		size_t currentPos = 0;

		for (auto& piece : pieces)
		{
			if (currentPos + piece.length <= position)
			{
				newPieces.push_back(piece);
				currentPos += piece.length;
			}
			else
			{
				if (position > currentPos)
				{
					size_t splitLength = position - currentPos;
					newPieces.emplace_back(piece.bufferType, piece.start, splitLength);
					piece.start += splitLength;
					piece.length -= splitLength;
				}

				if (piece.length > length)
				{
					piece.start += length;
					piece.length -= length;
					length = 0;
				}
				else
				{
					length -= piece.length;
					continue;
				}

				newPieces.push_back(piece);
				currentPos = position + length;
			}
		}

		pieces = newPieces;
	}

	UIString getText()
	{
		UIString result;

		for (const auto& piece : pieces)
		{
			if (piece.bufferType == ORIGINAL)
			{
				result.append(originalBuffer.substr(piece.start, piece.length));
			}
			else
			{
				result.append(addBuffer.substr(piece.start, piece.length));
			}
		}

		return result;
	}

	UIString getTextAt(size_t position, size_t length)
	{
		UIString result;
		size_t currentPos = 0;

		for (const auto& piece : pieces)
		{
			if (currentPos + piece.length <= position)
			{
				currentPos += piece.length;
				continue;
			}

			size_t pieceStart = (currentPos > position) ? 0 : position - currentPos;
			size_t pieceLength = std::min(piece.length - pieceStart, length);

			if (piece.bufferType == ORIGINAL)
			{
				result.append(originalBuffer.substr(piece.start + pieceStart, pieceLength));
			}
			else
			{
				result.append(addBuffer.substr(piece.start + pieceStart, pieceLength));
			}

			length -= pieceLength;
			currentPos += piece.length;

			if (length == 0) break;
		}

		return result;
	}

private:

	enum BufferType
	{
		ORIGINAL,
		ADD
	};
	struct Piece
	{
		BufferType bufferType;
		size_t start;
		size_t length;
	};

	UIString originalBuffer;
	UIString addBuffer;
	UIList<Piece> pieces;
};