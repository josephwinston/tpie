macro(add_unittest NAME)
  add_executable(ut-${NAME} test_${NAME}.cpp common.h priority_queue.h test_timer.h)
  set_target_properties(ut-${NAME} PROPERTIES FOLDER tpie/unittest)
  target_link_libraries(ut-${NAME} tpie ${Boost_LIBRARIES})
  if(TPIE_HAS_SNAPPY)
    target_link_libraries(ut-${NAME} ${Snappy_LIBRARY})
  endif(TPIE_HAS_SNAPPY)
  set(MTESTS ${ARGV})
  list(REMOVE_AT MTESTS 0)
  foreach(TEST ${MTESTS})
    add_test(${NAME}_${TEST} ut-${NAME} ${TEST})
  endforeach(TEST)
endmacro(add_unittest)

macro(add_fulltest NAME)
  set(MTESTS ${ARGV})
  list(REMOVE_AT MTESTS 0)
  foreach(TEST ${MTESTS})
    add_test(NAME ${NAME}_${TEST} CONFIGURATIONS FullTest COMMAND ut-${NAME} ${TEST})
  endforeach(TEST)
endmacro(add_fulltest)

add_unittest(allocator deque list)
add_unittest(ami_stream basic truncate)
add_unittest(array
	basic
	iterators
	auto_ptr
	memory
	bit_basic
	bit_iterators
	bit_memory
	copyempty
	arrayarray
	frontback
	swap
	allocator
	copy
	from_view
	)
add_unittest(atomic basic parallel)
add_unittest(compressed_stream basic seek seek_2 reopen_1 reopen_2 read_seek truncate truncate_2 position_0 position_1 position_2 position_3 position_4 position_5 position_6 position_7 position_seek uncompressed uncompressed_new
basic_u seek_u seek_2_u reopen_1_u reopen_2_u read_seek_u truncate_u truncate_2_u position_0_u position_1_u position_2_u position_3_u position_4_u position_5_u position_6_u position_7_u position_seek_u uncompressed_u uncompressed_new_u
odd_block_size)
add_unittest(disjoint_set basic memory)
add_unittest(external_priority_queue basic)
add_unittest(external_queue basic empty_size sized large)
add_unittest(external_sort amismall small tiny)
add_unittest(external_stack new named-new ami named-ami io)
add_unittest(file_count basic)
add_unittest(filestream memory)
add_unittest(hashmap chaining linear_probing iterators memory)
add_unittest(internal_priority_queue basic memory)
add_unittest(internal_queue basic memory)
add_unittest(internal_stack basic memory)
add_unittest(internal_vector basic memory)
add_unittest(job repeat)
add_unittest(memory basic)
add_unittest(merge_sort
	empty_input
	internal_report
	internal_report_after_resize
	one_run_external_report
	external_report
	small_final_fanout
	evacuate_before_merge
	evacuate_before_report
	sort_upper_bound
	temp_file_usage
	tall_tree
	)
add_unittest(packed_array basic1 basic2 basic4)
add_unittest(parallel_sort basic1 basic2 general equal_elements bad_case)
add_unittest(serialization unsafe safe serialization2 stream stream_dtor stream_reopen)
add_unittest(serialization_sort
	empty_input
	internal_report
	internal_report_after_resize
	one_run_external_report
	external_report
	small_final_fanout
	evacuate_before_merge
	evacuate_before_report
	)
add_unittest(stats simple)
add_unittest(stream
	basic
	array
	array_compressed
	odd
	odd_compressed
	reopen
	truncate
	extend
	backwards
	array_file
	odd_file
	truncate_file
	extend_file
	backwards_file
	user_data
	user_data_file
	peek_skip_1
	peek_skip_2
	)
add_unittest(stream_exception basic)
add_unittest(pipelining
	vector
	filestream
	fspull
	fsaltpush
	merge
	reverse
	internal_reverse
	passive_reverse
	internal_passive_reverse
	sort
	sorttrivial
	operators
	uniq
	memory
	fork
	merger_memory
	fetch_forward
	virtual
	virtual_cref_item_type
	virtual_fork
	prepare
	end_time
	pull_iterator
	push_iterator
	parallel
	parallel_ordered
	parallel_multiple
	parallel_own_buffer
	parallel_push_in_end
	node_map
	join
	copy_ctor
	)
add_unittest(pipelining_runtime evacuate get_phase_graph)
add_unittest(pipelining_serialization basic reverse sort)
add_unittest(maybe basic auto_ptr)
add_unittest(close_file internal serialization_writer_close serialization_writer_dtor serialization_reader_dtor)
add_unittest(node_name gcc msvc)
add_unittest(snappy basic)

add_fulltest(ami_stream stress)
add_fulltest(disjoint_set large large_cycle very_large medium ovelflow)
add_fulltest(external_priority_queue medium large large_cycle memory)
add_fulltest(external_sort amilarge large tall very_large)
add_fulltest(internal_priority_queue large_cycle)
add_fulltest(internal_vector medium large)
add_fulltest(pipelining sortbig parallel_step)
