/*=================================================
* Copyright © 2020-2022 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
*=====================History========================
*
*
* =================================================*/

#include "RmPieceTable.h"
#include <assert.h>
#define LINE_WRAP '\n'

static int GetU8ByteCount(char ch)
{
	if (0 <= uint8_t(ch) && uint8_t(ch) < 0x80) {
		return 1;
	}
	if (0xC2 <= uint8_t(ch) && uint8_t(ch) < 0xE0) {
		return 2;
	}
	if (0xE0 <= uint8_t(ch) && uint8_t(ch) < 0xF0) {
		return 3;
	}
	if (0xF0 <= uint8_t(ch) && uint8_t(ch) < 0xF8) {
		return 4;
	}
	return 0;
}

size_t UTF8ByteNum(const RmArray<char, 5>& u8Ch)
{
	return GetU8ByteCount(u8Ch[0]);
}

using utf8_foreach_t = RmLambda<bool(size_t index, size_t offset, size_t length)>;
void UTF8Foreach(const RmString& u8Str, size_t start, size_t end, utf8_foreach_t func)
{
	if (func == nullptr || end <= start) return;
	size_t index = 0;
	size_t offset = 0;
	for (size_t i = 0; i + start < end; i += offset, ++index)
	{
		offset = ::UTF8ByteNum({ u8Str[i + start] });
		if (offset == 0)break;
		if (func(index, i, offset) == false) break;
	}
}

RmPieceTable::RmPieceTable()
	:
	m_BatchNum(0),
	m_RecordIndex(-1)
{
	reset();
}

RmPieceTable::~RmPieceTable()
{
}

void RmPieceTable::reset()
{
	m_TextList.clear();
	m_RecordList.clear();
	m_PieceList.clear();
	m_RecordIndex = -1;
	m_BatchNum = 0;
	m_TextList.push_back({ "\n", 1 });
}

void RmPieceTable::commit()
{
	m_RecordList.clear();
	m_RecordIndex = -1;
	m_BatchNum = 0;
	for (size_t i = 0; i < m_PieceList.size(); ++i) purify_line(i);
}

bool RmPieceTable::insert(size_t& row, size_t& col, const RmString& utf8)
{
	// 规范行列
	if (row < m_PieceList.size())
	{
		size_t icol = 0;
		for (auto& e : m_PieceList[row]) icol += e.Right.Utf8 - e.Left.Utf8;
		if (icol < col) col = icol;
	}
	else if (row == m_PieceList.size()) col = 0;
	else return false;

	// 清理记录
	m_RecordList.resize(m_RecordIndex + 1);

	// 缓存文本
	m_TextList.emplace_back();
	auto& text = m_TextList.back();
	text.Text = utf8;
	text.Utf8 = utf8.length();

	// 添加记录
	size_t lastUtf8 = 0, lastIndex = 0, nextUtf8 = 0, nextIndex = 0;
	::UTF8Foreach(utf8, 0, utf8.length(), [&](auto index, auto offset, auto length)->bool {
		if (utf8[offset] == LINE_WRAP)
		{
			// 记录换行符之前的文本
			if (lastIndex < offset)
			{
				m_RecordList.emplace_back();
				auto& record = m_RecordList.back();
				record.Batch = m_BatchNum;
				record.File = m_TextList.size() - 1;
				record.Left.Utf8 = lastUtf8;
				record.Left.Index = lastIndex;
				record.Right.Utf8 = index;
				record.Right.Index = offset;
				record.Row = row;
				record.Column = col;
				record.Insert = true;

				col += index - lastUtf8;
			}

			m_RecordList.emplace_back();
			auto& record = m_RecordList.back();
			record.Batch = m_BatchNum;
			record.File = 0;
			record.Left = { 0,0 };
			record.Right = { 1,1 };
			record.Row = row;
			record.Column = col;
			record.Insert = true;

			lastUtf8 = index + 1;
			lastIndex = offset + 1;

			++row;
			col = 0;
		}
		nextUtf8 = index + 1;
		nextIndex = offset + length;
		return true;
		});
	if (lastIndex < nextIndex)
	{
		m_RecordList.emplace_back();
		auto& record = m_RecordList.back();
		record.Batch = m_BatchNum;
		record.File = m_TextList.size() - 1;
		record.Left.Utf8 = lastUtf8;
		record.Left.Index = lastIndex;
		record.Right.Utf8 = nextUtf8;
		record.Right.Index = nextIndex;
		record.Row = row;
		record.Column = col;
		record.Insert = true;

		col += nextUtf8 - lastUtf8;
	}

	// 插入结点
	for (size_t i = m_RecordIndex + 1; i < m_RecordList.size(); ++i)
	{
		auto& record = m_RecordList[i];
		auto result = insert_line(record);
		assert(result);
	}
	m_RecordIndex = m_RecordList.size() - 1;

	// 更新批处理编号
	++m_BatchNum;
	return true;
}

bool RmPieceTable::remove(size_t& row, size_t& col, size_t count)
{
	// 规范行列
	if (row < m_PieceList.size())
	{
		size_t icol = 0;
		for (auto& e : m_PieceList[row]) icol += e.Right.Utf8 - e.Left.Utf8;
		if (icol < col) col = icol;
	}
	else return false;

	// 清理记录
	m_RecordList.resize(m_RecordIndex + 1);

	// 添加记录
	auto& line = m_PieceList[row];
	size_t icol = 0;
	for (auto itr = line.begin(); itr != line.end(); ++itr)
	{
		auto& e = *itr;
		auto& text = m_TextList[e.File];
		auto num = e.Right.Utf8 - e.Left.Utf8;

		size_t xleft = 0, xright = 0;
		if (icol <= col)
		{
			if (col <= icol + num)
			{
				// icol <= col <= icol+num <= col+count
				if (icol - col + num <= count) // <=> (icol + num <= col + count)
				{
					xleft = col - icol;
					xright = num;
				}
				// ico <= col <= col+count <= icol+num
				else
				{
					xleft = col - icol;
					xright = col - icol + count;
				}
			}
		}
		else
		{
			if (icol <= col + count)
			{
				// col <= icol <= icol+num <= col+count
				if (icol - col + num <= count)
				{
					xleft = 0;
					xright = num;
				}
				// col <= icol <= col+count <= icol+num
				else
				{
					xleft = 0;
					xright = col - icol + count;
				}
			}
		}

		if (xleft == xright);
		else
		{
			m_RecordList.emplace_back();
			auto& record = m_RecordList.back();
			record.Batch = m_BatchNum;
			record.File = e.File;
			record.Row = row;
			record.Column = col;
			record.Insert = false;

			size_t lastUtf8 = 0, lastIndex = 0, nextUtf8 = 0, nextIndex = 0;
			::UTF8Foreach(text.Text, e.Left.Index, e.Right.Index, [&](auto index, auto offset, auto length)->bool {
				nextUtf8 = index + 1;
				nextIndex = offset + length;
				if (index == xleft)
				{
					record.Left.Utf8 = e.Left.Utf8 + index;
					record.Left.Index = e.Left.Index + offset;
				}
				if (index == xright)
				{
					record.Right.Utf8 = e.Left.Utf8 + index;
					record.Right.Index = e.Left.Index + offset;
					return false;
				}
				return true;
				});
			if (nextUtf8 == xright)
			{
				record.Right.Utf8 = e.Left.Utf8 + nextUtf8;
				record.Right.Index = e.Left.Index + nextIndex;
			}
			if (count <= icol - col + num) break;
		}
		icol += num;
	}
	if (m_RecordIndex + 1 == m_RecordList.size() && icol == col)
	{
		// 删除下一行
		if (row + 1 < m_PieceList.size())
		{
			m_RecordList.emplace_back();
			auto& record = m_RecordList.back();
			record.Batch = m_BatchNum;
			record.File = 0;
			record.Left = { 0,0 };
			record.Right = { 1,1 };
			record.Row = row;
			record.Column = col;
			record.Insert = false;
		}
	}

	// 删除结点
	for (size_t i = m_RecordIndex + 1; i < m_RecordList.size(); ++i)
	{
		auto& record = m_RecordList[i];
		auto result = remove_line(record);
		assert(result);
	}
	m_RecordIndex = m_RecordList.size() - 1;

	// 更新批处理编号
	++m_BatchNum;
	return true;
}

bool RmPieceTable::undo(size_t& row, size_t& col)
{
	if (0 <= m_RecordIndex && m_RecordIndex < m_RecordList.size())
	{
		const auto LAST_BATCH = m_RecordList[m_RecordIndex].Batch;
		while (0 <= m_RecordIndex
			&& m_RecordIndex < m_RecordList.size()
			&& m_RecordList[m_RecordIndex].Batch == LAST_BATCH)
		{
			auto& record = m_RecordList[m_RecordIndex];
			// 撤销插入操作
			if (record.Insert)
			{
				remove_line(record);
				row = record.Row;
				col = record.Column;
				--m_RecordIndex;
			}
			// 撤销删除操作
			else
			{
				insert_line(record);
				row = record.Row;
				col = record.Column;
				if (m_TextList[record.File].Text[record.Left.Index] == LINE_WRAP)
				{
					++row;
					col = 0;
				}
				else
				{
					col += record.Right.Utf8 - record.Left.Utf8;
				}
				--m_RecordIndex;
			}
		}
		return true;
	}
	return false;
}

bool RmPieceTable::redo(size_t& row, size_t& col)
{
	if (0 <= m_RecordIndex && m_RecordIndex + 1 < m_RecordList.size())
	{
		const auto LAST_BATCH = m_RecordList[m_RecordIndex + 1].Batch;
		while (0 <= m_RecordIndex
			&& m_RecordIndex + 1 < m_RecordList.size()
			&& m_RecordList[m_RecordIndex + 1].Batch == LAST_BATCH)
		{
			auto& record = m_RecordList[m_RecordIndex + 1];
			// 执行插入操作
			if (record.Insert)
			{
				insert_line(record);
				row = record.Row;
				col = record.Column;
				if (m_TextList[record.File].Text[record.Left.Index] == LINE_WRAP)
				{
					++row;
					col = 0;
				}
				else
				{
					col += record.Right.Utf8 - record.Left.Utf8;
				}
				++m_RecordIndex;
			}
			// 执行删除操作
			else
			{
				remove_line(record);
				row = record.Row;
				col = record.Column;
				++m_RecordIndex;
			}
		}
		return true;
	}
	return false;
}

bool RmPieceTable::text(size_t row, size_t col, size_t count, RmString& buffer)
{
	if (row < m_PieceList.size() && count)
	{
		size_t icol = 0, iword = 0;
		auto& line = m_PieceList[row];
		for (auto& e : line)
		{
			auto& text = m_TextList[e.File];
			auto num = e.Right.Utf8 - e.Left.Utf8;
			if (col < icol + num)
			{
				::UTF8Foreach(text.Text, e.Left.Index, e.Right.Index, [&](auto index, auto offset, auto length)->bool {
					if (col <= icol + index)
					{
						buffer += text.Text.substr(e.Left.Index + offset, length);
						++iword;
						if (count <= iword) return false;
					}
					return true;
					});
				if (count <= iword) return false;
			}
			icol += num;
		}
		return true;
	}
	return false;
}

void RmPieceTable::text(RmString& buffer)
{
	for (size_t i = 0; i < m_PieceList.size(); ++i)
	{
		for (auto& e : m_PieceList[i])
		{
			auto& text = m_TextList[e.File];
			buffer += text.Text.substr(e.Left.Index, e.Right.Index - e.Left.Index);
		}
		if (i + 1 < m_PieceList.size())
		{
			buffer += LINE_WRAP;
		}
	}
}

bool RmPieceTable::insert_line(const RmPieceRecord& record)
{
	//printf("insert %d %d %d %d\n", record.Row, record.Column, record.Left.Utf8, record.Right.Utf8);

	if (record.Row < m_PieceList.size())
	{
		auto& line = m_PieceList[record.Row];
		size_t icol = 0;
		for (auto itr = line.begin(); itr != line.end(); ++itr)
		{
			auto& e = *itr;
			auto num = e.Right.Utf8 - e.Left.Utf8;
			if (icol <= record.Column && record.Column < icol + num)
			{
				// 分割结点
				::UTF8Foreach(m_TextList[e.File].Text, e.Left.Index, e.Right.Index, [&](auto index, auto offset, auto length)->bool {
					if (icol + index == record.Column)
					{
						RmPieceNode two;
						two.File = e.File;
						two.Left = e.Left;
						two.Right.Utf8 = two.Left.Utf8 + index;
						two.Right.Index = two.Left.Index + offset;
						e.Left = two.Right;
						itr = line.emplace(itr, two);
						return false;
					}
					return true;
					});

				// 行中间插入换行
				if (m_TextList[record.File].Text[record.Left.Index] == LINE_WRAP)
				{
					std::list<RmPieceNode> nextLine;
					if (itr != line.end())
					{
						nextLine.splice(nextLine.begin(), line, ++itr, line.end());
					}
					m_PieceList.emplace(m_PieceList.begin() + record.Row + 1, nextLine);
					purify_line(record.Row + 1);
				}
				else
				{
					RmPieceNode one;
					one.File = record.File;
					one.Left = record.Left;
					one.Right = record.Right;
					itr = line.insert(++itr, one);
					purify_line(record.Row);
				}
				return true;
			}
			icol += num;
		}
		if (m_TextList[record.File].Text[record.Left.Index] == LINE_WRAP)
		{
			m_PieceList.emplace(m_PieceList.begin() + record.Row + 1);
		}
		else
		{
			RmPieceNode one;
			one.File = record.File;
			one.Left = record.Left;
			one.Right = record.Right;
			line.emplace_back(one);
		}
		return true;
	}
	else if (record.Row == m_PieceList.size())
	{
		m_PieceList.emplace_back();
		if (m_TextList[record.File].Text[record.Left.Index] == LINE_WRAP);
		else
		{
			auto& line = m_PieceList.back();
			RmPieceNode one;
			one.File = record.File;
			one.Left = record.Left;
			one.Right = record.Right;
			line.emplace_back(one);
		}
		return true;
	}
	return false;
}

bool RmPieceTable::remove_line(const RmPieceRecord& record)
{
	//printf("remove %d %d %d %d\n", record.Row, record.Column, record.Left.Utf8, record.Right.Utf8);

	// 调整或删除结点
	if (record.Row < m_PieceList.size())
	{
		auto& line = m_PieceList[record.Row];
		if (line.empty())
		{
			m_PieceList.erase(m_PieceList.begin() + record.Row);
		}
		else
		{
			size_t icol = 0;
			size_t col = record.Column;
			size_t count = record.Right.Utf8 - record.Left.Utf8;
			for (auto itr = line.begin(); itr != line.end(); )
			{
				auto& e = *itr;
				auto& text = m_TextList[e.File];
				auto num = e.Right.Utf8 - e.Left.Utf8;

				size_t xleft = 0, xright = 0;
				if (icol <= col)
				{
					if (col <= icol + num)
					{
						// icol <= col <= icol+num <= col+count
						if (icol - col + num <= count) // <=> (icol + num <= col + count)
						{
							xleft = col - icol;
							xright = num;
						}
						// ico <= col <= col+count <= icol+num
						else
						{
							xleft = col - icol;
							xright = col - icol + count;
						}
					}
				}
				else
				{
					if (icol <= col + count)
					{
						// col <= icol <= icol+num <= col+count
						if (icol - col + num <= count)
						{
							xleft = 0;
							xright = num;
						}
						// col <= icol <= col+count <= icol+num
						else
						{
							xleft = 0;
							xright = col - icol + count;
						}
					}
				}

				if (xleft == xright);
				else
				{
					// xxxxxx
					if (xleft == 0 && xright == num)
					{
						itr = line.erase(itr);
						if (count <= icol - col + num) break;
						icol += num;
						continue;
					}
					// xxxxoo
					else if (xleft == 0)
					{
						e.Left = record.Right;
					}
					// ooxxxx
					else if (xright == num)
					{
						e.Right = record.Left;
					}
					// ooxxoo
					else
					{
						RmPieceNode one;
						one.File = record.File;
						one.Left = record.Right;
						one.Right = e.Right;
						e.Right = record.Left;
						itr = line.insert(++itr, one);
					}
					if (count <= icol - col + num) break;
				}

				icol += num;
				++itr;
			}
			if (m_TextList[record.File].Text[record.Left.Index] == LINE_WRAP)
			{
				auto& nextLine = m_PieceList[record.Row + 1];
				line.splice(line.end(), nextLine, nextLine.begin(), nextLine.end());
				m_PieceList.erase(m_PieceList.begin() + record.Row + 1);
			}
			purify_line(record.Row);
		}
		return true;
	}
	return false;
}

bool RmPieceTable::purify_line(size_t row)
{
	if (row < m_PieceList.size())
	{
		auto& line = m_PieceList[row];
		for (auto itr = line.begin(); itr != line.end(); )
		{
			auto& e = *itr;
			if (e.Left.Utf8 == e.Right.Utf8)
			{
				itr = line.erase(itr);
				continue;
			}
			++itr;
		}
		return true;
	}
	return false;
}