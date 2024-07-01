#pragma once
/*=================================================
* Copyright © 2020-2024 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
* 职责：提供高性能的文本缓冲功能；
*
* 规定各行的第一个结点为换行符；删除空行，则会删除整行；
*
*=====================History========================
*
*
* =================================================*/

#include "RmGUI.h"

/// @brief 位置
struct RmPiecePosition
{
	/// @brief 字符索引
	size_t Index = 0;
	/// @brief UTF8字符索引
	size_t Utf8 = 0;
};

/// @brief 文本
struct RmPieceBuffer
{
	/// @brief 文本
	RmString Text;
	/// @brief UTF8字符数目
	size_t Utf8 = 0;
};

/// @brief 结点
struct RmPieceNode
{
	/// @brief 文本索引
	size_t File = 0;
	/// @brief 起始位置
	RmPiecePosition Left;
	/// @brief 结束位置（不包含）
	RmPiecePosition Right;
};

/// @brief 记录
struct RmPieceRecord
{
	/// @brief 批处理号
	size_t Batch = 0;
	/// @brief 文本索引
	size_t File = 0;
	/// @brief 起始位置
	RmPiecePosition Left;
	/// @brief 结束位置（不包含）
	RmPiecePosition Right;

	/// @brief 行数
	size_t Row = 0;
	/// @brief 列数
	size_t Column = 0;
	/// @brief 插入动作
	bool Insert = true;
};

/// @brief 文本缓冲
class RmPieceTable final
{
public:
	RmPieceTable();
	~RmPieceTable();

	/// @brief 插入文本
	/// @param row 行数，返回新行
	/// @param col 列数，返回新列
	/// @param utf8 文本
	/// @return 操作结果
	bool insert(size_t& row, size_t& col, const RmString& utf8);

	/// @brief 删除文本
	/// @param row 行数，返回新行
	/// @param col 列数，返回新列
	/// @param count 文本长度
	/// @return 操作结果
	bool remove(size_t& row, size_t& col, size_t count);

	/// @brief 撤销操作（ctrl+x）
	/// @param row 行数，返回新行
	/// @param col 列数，返回新列
	/// @return 
	bool undo(size_t& row, size_t& col);

	/// @brief 重复操作（ctrl+y）
	/// @param row 行数，返回新行
	/// @param col 列数，返回新列
	/// @return 
	bool redo(size_t& row, size_t& col);

	/// @brief 截取文本
	/// @param row 
	/// @param col 
	/// @param count 
	/// @param buffer 
	/// @return 
	bool text(size_t row, size_t col, size_t count, RmString& buffer);

	/// @brief 截取文本
	/// @param buffer
	void text(RmString& buffer);

	/// @brief 提交修改
	void commit();

	/// @brief 重置文本
	void reset();

private:
	/// @brief 插入行
	/// @param record 
	/// @return 
	bool insert_line(const RmPieceRecord& record);

	/// @brief 删除行
	/// @param record 
	/// @return 
	bool remove_line(const RmPieceRecord& record);

	/// @brief 精简行
	/// @param row 
	/// @return 
	bool purify_line(size_t row);

private:
	/// @brief 文本缓冲
	RmVector<RmPieceBuffer> m_TextList;
	/// @brief 文本描述
	RmVector<RmList<RmPieceNode>> m_PieceList;
	/// @brief 文本操作
	RmVector<RmPieceRecord> m_RecordList;
	/// @brief 当前操作
	size_t m_RecordIndex;
	/// @brief 批处理编号
	size_t m_BatchNum;
};