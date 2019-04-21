mmu: Process.cpp pte.cpp VMA.cpp pager.cpp frame_table.cpp main.cpp 
	g++-8.1 -g -o mmu Process.cpp pte.cpp VMA.cpp pager.cpp frame_table.cpp main.cpp 
#module load gcc-8.1
clean:
	rm -f mmu *~
