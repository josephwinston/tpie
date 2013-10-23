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

#include <tpie/pipelining/pipeline.h>
#include <tpie/pipelining/node.h>
#include <tpie/pipelining/graph.h>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <boost/unordered_set.hpp>

namespace {
	typedef tpie::pipelining::bits::node_map S;

	class name {
	public:
		inline name(S::ptr segmap, S::id_t id) : segmap(segmap), id(id) {}
		S::ptr segmap;
		S::id_t id;
	};

	inline std::ostream & operator<<(std::ostream & out, const name & n) {
		S::val_t p = n.segmap->get(n.id);
		std::string name = p->get_name();
		if (name.size())
			return out << name << " (" << n.id << ')';
		else
			return out << typeid(*p).name() << " (" << n.id << ')';
	}
} // default namespace

namespace tpie {

namespace pipelining {

namespace bits {

typedef boost::unordered_map<const node *, size_t> nodes_t;

void pipeline_base::plot(std::ostream & out, bool full) {
	typedef tpie::pipelining::bits::node_map::id_t id_t;

	node_map::ptr segmap = m_segmap->find_authority();
	const node_map::relmap_t & relations = segmap->get_relations();
	
	boost::unordered_map<id_t, id_t> repr;
	if (!full) {
	 	for (node_map::relmapit i = relations.begin(); i != relations.end(); ++i) {
			id_t s = i->first;
			id_t t = i->second.first;
			if (i->second.second != pushes) std::swap(s,t);
			if (segmap->get(s)->get_plot_options() & node::PLOT_SIMPLIFIED_HIDE)
				repr[s] = t;
		}
	}
	
	out << "digraph {\n";
	for (node_map::mapit i = segmap->begin(); i != segmap->end(); ++i) {
		if (repr.count(i->first)) continue;
		if (!full && (segmap->get(i->first)->get_plot_options() & node::PLOT_BUFFERED))
			out << '"' << name(segmap, i->first) << "\" [shape=box];\n";
		else
			out << '"' << name(segmap, i->first) << "\";\n";
	}

	for (node_map::relmapit i = relations.begin(); i != relations.end(); ++i) {
		id_t s = i->first;
		id_t t = i->second.first;
		if (i->second.second != pushes) std::swap(s,t);
		if (repr.count(s)) continue;
		while (repr.count(t)) t=repr[t];
		switch (i->second.second) {
			case pushes:
				out << '"' << name(segmap, s) << "\" -> \"" << name(segmap, t) << "\";\n";
				break;
			case pulls:
				out << '"' << name(segmap, s) << "\" -> \"" << name(segmap, t) << "\" [arrowhead=none,arrowtail=normal,dir=both];\n";
				break;
			case depends:
				out << '"' << name(segmap, s) << "\" -> \"" << name(segmap, t) << "\" [arrowhead=none,arrowtail=normal,dir=both,style=dashed];\n";
				break;
		}
	}
	out << '}' << std::endl;
}

void pipeline_base::operator()(stream_size_type items, progress_indicator_base & pi, const memory_size_type initialMemory) {
	typedef std::vector<phase> phases_t;
	typedef phases_t::const_iterator it;

	node_map::ptr map = m_segmap->find_authority();
	graph_traits g(*map);
	const phases_t & phases = g.phases();
	if (initialMemory == 0) log_warning() << "No memory for pipelining" << std::endl;

	memory_size_type mem = initialMemory;
	mem -= graph_traits::memory_usage(phases.size());

	if (mem > initialMemory) { // overflow
		log_warning() << "Not enough memory for pipelining framework overhead" << std::endl;
		mem = 0;
	}

	log_debug() << "Assigning " << mem << " b memory to each pipelining phase." << std::endl;
	for (it i = phases.begin(); i != phases.end(); ++i) {
		i->assign_memory(mem);
#ifndef TPIE_NDEBUG
		i->print_memory(log_debug());
#endif // TPIE_NDEBUG
	}
	g.go_all(items, pi);
}

void pipeline_base::forward_any(std::string key, const boost::any & value) {
	typedef graph_traits::nodes_t nodes_t;
	typedef graph_traits::nodeit it;

	node_map::ptr map = m_segmap->find_authority();
	graph_traits g(*map);
	const nodes_t & sources = g.item_sources();
	for (size_t j = 0; j < sources.size(); ++j) {
		sources[j]->add_forwarded_data(key, value);
	}
}

bool pipeline_base::can_fetch(std::string key) {
	typedef graph_traits::nodes_t nodes_t;
	typedef graph_traits::nodeit it;

	node_map::ptr map = m_segmap->find_authority();
	graph_traits g(*map);
	const nodes_t & sinks = g.item_sinks();
	for (size_t j = 0; j < sinks.size(); ++j) {
		if (sinks[j]->can_fetch(key)) return true;
	}
	return false;
}

boost::any pipeline_base::fetch_any(std::string key) {
	typedef graph_traits::nodes_t nodes_t;
	typedef graph_traits::nodeit it;

	node_map::ptr map = m_segmap->find_authority();
	graph_traits g(*map);
	const nodes_t & sinks = g.item_sinks();
	for (size_t j = 0; j < sinks.size(); ++j) {
		if (sinks[j]->can_fetch(key)) return sinks[j]->fetch_any(key);
	}

	std::stringstream ss;
	ss << "Tried to fetch nonexistent key '" << key << '\'';
	throw invalid_argument_exception(ss.str());
}

} // namespace bits

void pipeline::output_memory(std::ostream & o) const {
	bits::node_map::ptr segmap = p->get_node_map()->find_authority();
	for (bits::node_map::mapit i = segmap->begin(); i != segmap->end(); ++i) {
		bits::node_map::val_t p = segmap->get(i->first);
		o << p->get_name() << ": min=" << p->get_minimum_memory() << "; max=" << p->get_available_memory() << "; prio=" << p->get_memory_fraction() << ";" << std::endl;

	}
}

} // namespace pipelining

} // namespace tpie
