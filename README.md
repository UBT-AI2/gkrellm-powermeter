Build: 

-	cmake CMakeLists.txt
-	make

Install:

-	copy the resulting .so into one of the following folders:
	-	~/.gkrellm2/plugins/
	-	/usr/local/lib/gkrellm2/plugins/
	-	/usr/lib/gkrellm2/plugins/
	
-	alternatively, run gkrellm -p <plugin> to test the plugin temporarely

Run:

-	In gkrellm's settings: enable the plugin
