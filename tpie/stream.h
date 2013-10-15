// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; eval: (progn (c-set-style "stroustrup") (c-set-offset 'innamespace 0)); -*-
// vi:set ts=4 sts=4 sw=4 noet :
// Copyright 2013, The TPIE development team
// 
// This file is part of TPIE.
// 
// TPIE is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
// 
// TPIE is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with TPIE.  If not, see <http://www.gnu.org/licenses/>

#ifndef TPIE_STREAM_H
#define TPIE_STREAM_H

#include <tpie/stream.h>
#include <tpie/compressed_stream.h>

namespace tpie {

namespace ami {

template <typename T>
class cstream {
public:
	typedef T item_type;

	cstream()
		: m_status(STREAM_STATUS_INVALID)
	{
		try {
			m_stream.open(m_temp);
		} catch (const stream_exception & e) {
			log_fatal() << "Open failed: " << e.what() << std::endl;
			return;
		}
		m_status = STREAM_STATUS_VALID;
	}

	cstream(const std::string & fileName,
			stream_type st = READ_WRITE_STREAM)
		: m_temp(fileName, true)
		, m_status(STREAM_STATUS_INVALID)
	{
		try {
			m_stream.open(m_temp, st == READ_STREAM ? access_read : access_read_write);
			if (st == APPEND_STREAM) m_stream.seek(0, file_stream_base::end);
		} catch (const stream_exception & e) {
			log_fatal() << "Open failed: " << e.what() << std::endl;
			return;
		}
		m_status = STREAM_STATUS_VALID;
	}

	err new_substream(stream_type /*st*/,
					  stream_offset_type /*sub_begin*/,
					  stream_offset_type /*sub_end*/,
					  stream<T> ** /*sub_stream*/)
	{
		return BTE_ERROR;
	}

	stream_status status() const {
		return m_status;
	}

	bool is_valid() const {
		return m_status == STREAM_STATUS_VALID;
	}

	bool operator!() const {
		return !is_valid();
	}

	err read_item(T **elt) {
		if (!m_stream.can_read())
		return END_OF_STREAM;

		*elt = &(const_cast<T &>(m_stream.read_ref()));
		return NO_ERROR;
	}

	err write_item(const T &elt) {
		try {
			m_stream.write(elt);
		} catch (const stream_exception & e) {
			log_warning() << "write_item failed: " << e.what() << std::endl;
			return BTE_ERROR;
		}
		return NO_ERROR;
	}

	err read_array(T *mm_space, stream_offset_type *len) {
		size_type l=(size_t)*len;
		err e = read_array(mm_space, l);
		*len = l;
		return e;
	}

	err read_array(T *mm_space, memory_size_type & len) {
		size_type l = static_cast<size_type>(std::min(
			static_cast<stream_size_type>(len),
			static_cast<stream_size_type>(m_stream.size() - m_stream.offset())));
		m_stream.read(mm_space, mm_space+l);
		return (l == len)?NO_ERROR:END_OF_STREAM;
	}

	err write_array(const T *mm_space, memory_size_type len) {
		try {
			m_stream.write(mm_space, mm_space+len);
		} catch (const stream_exception & e) {
			log_warning() << "write_item failed: " << e.what() << std::endl;
			return BTE_ERROR;
		}
		return NO_ERROR;
	}

	stream_offset_type stream_len(void) const {
		return m_stream.size();
	}

	std::string name() const {
		return m_stream.path();
	}

	err seek(stream_offset_type offset) {
		try {
			m_stream.seek(offset);
		} catch(const stream_exception &e) {
			TP_LOG_WARNING_ID("BTE error - seek failed: " << e.what());
			return BTE_ERROR;
		}
		return NO_ERROR;
	}

	stream_offset_type tell() const {
		return m_stream.offset();
	}

	err truncate(stream_offset_type offset) {
		try {
			m_stream.truncate(offset);
		} catch(const stream_exception & e) {
			TP_LOG_WARNING_ID("BTE error - truncate failed: " << e.what());
			return BTE_ERROR;
		}
		return NO_ERROR;
	}

	err main_memory_usage(size_type *usage,
						  stream_usage usage_type) const
	{
		switch (usage_type) {
			case STREAM_USAGE_OVERHEAD:
				*usage = sizeof(*this) + compressed_stream<T>::memory_usage(0.0);
				return NO_ERROR;
			case STREAM_USAGE_CURRENT:
			case STREAM_USAGE_MAXIMUM:
			case STREAM_USAGE_SUBSTREAM:
				*usage =  memory_usage(1);
				return NO_ERROR;
			case STREAM_USAGE_BUFFER:
				*usage = compressed_stream<T>::memory_usage(block_factor())
					- compressed_stream<T>::memory_usage(0.0);
				return NO_ERROR;
		}
		return BTE_ERROR;
	}

	static memory_size_type memory_usage(memory_size_type count) {
		return count*(compressed_stream<T>::memory_usage() + sizeof(cstream<T>));
	}

	size_t available_streams(void) {
		return available_files();
	}

	memory_size_type chunk_size(void) const {
		return file_base::block_size(1.0) / sizeof(T);
	}

	void persist(persistence p) {
		m_temp.set_persistent(p == PERSIST_PERSISTENT);
	}

	persistence persist() const {
		return m_temp.is_persistent() ? PERSIST_PERSISTENT : PERSIST_DELETE;
	}

	std::string& sprint() {
		static std::string buf;
		std::stringstream ss;
		ss << "STREAM " << name() <<  " " << static_cast<long>(stream_len());
		ss >> buf;
		return buf;
	}

	compressed_stream<T>& underlying_stream() {
		return m_stream;
	}

private:
	cstream(const cstream<T>& other);
	cstream<T>& operator=(const cstream<T>& other);

	temp_file m_temp;
	compressed_stream<T> m_stream;

	stream_status m_status;

	static inline float block_factor() {
#ifndef STREAM_UFS_BLOCK_FACTOR
		return 1.0;
#else
#   ifdef WIN32
		return static_cast<float>(STREAM_UFS_BLOCK_FACTOR)/32;
#   else
		return static_cast<float>(STREAM_UFS_BLOCK_FACTOR)/512;
#   endif
#endif
	}
};

} // namespace ami

} // namespace tpie

#endif // TPIE_STREAM_H
