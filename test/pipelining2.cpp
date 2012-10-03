// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; c-file-style: "stroustrup"; -*-
// vi:set ts=4 sts=4 sw=4 noet cino+=(0 :
// Copyright 2012, The TPIE development team
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

#include <tpie/tpie.h>
#include <tpie/pipelining.h>
#include <boost/random.hpp>
#include <tpie/file_stream.h>
#include <iostream>
#include <sstream>
#include <tpie/progress_indicator_arrow.h>

using namespace tpie;
using namespace tpie::pipelining;

template <typename src_pipe_t>
class add_t {
	typedef typename src_pipe_t::factory_type src_fact_t;
	typedef typename src_fact_t::generated_type src_t;

public:
	template <typename dest_t>
	class type : public pipe_segment {
		dest_t dest;
		src_t src;
	public:
		typedef int item_type;

		type(dest_t dest, src_pipe_t srcpipe)
			: dest(dest)
			, src(srcpipe.factory.construct())
		{
			add_push_destination(dest);
			add_pull_destination(src);
			this->set_name("Adder", PRIORITY_INSIGNIFICANT);
		}

		void push(int i) {
			dest.push(i+src.pull());
		}
	};
};

template <typename src_pipe_t>
inline pipe_middle<tempfactory_1<add_t<src_pipe_t>, src_pipe_t> >
add(src_pipe_t srcpipe) {
	return tempfactory_1<add_t<src_pipe_t>, src_pipe_t>(srcpipe);
}

void go() {
	passive_buffer<int> buf;
	pipeline p
		= scanf_ints()
		| pipesort()
		| fork(buf.input())
		| reverser()
		| add(buf.output())
		| printf_ints();
	p.plot();
	p();
}

int main() {
	tpie_init();
	get_memory_manager().set_limit(50*1024*1024);
	go();
	tpie_finish();
	return 0;
}