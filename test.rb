#! /usr/bin/env ruby

def create_test_file()
	`rm -f dat/*`
	puts "Generating 1 Gb test file for benchmarking."
	`dd if=/dev/urandom of=dat/test.dat bs=4096 count=262144`
end

def flush_page_cache()
	if File.exist?("/proc/sys/vm/drop_caches")
		`sync; echo 1 > /proc/sys/vm/drop_caches`
	else
		`purge`
	end
end

def get_read_buffer_size()
	create_test_file()
	puts "Determining optimal read buffer size."
	min_t = 9999999
	min_n =  0
	page_size = `getconf PAGE_SIZE`.to_i
	(1..24).each do |i|
		flush_page_cache()
		n = page_size * i
		t = `./out/read_benchmark.run dat/test.dat #{n}`.to_f
		if (t < 0.995 * min_t)
			min_t = t
			min_n = n
		end
		puts "(#{i}/24): Took #{t} seconds using buffer size #{n}."
	end
	puts "Deleting test file."
	`rm dat/test.dat`
	return min_n
end

s = get_read_buffer_size()
puts "Chosen read buffer size: #{s} bytes."
