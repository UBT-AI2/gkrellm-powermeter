Build: 

-	cmake CMakeLists.txt
-	make

Install:

-	copy the resulting .so into one of the following folders:
	-	~/.gkrellm2/plugins/
	-	/usr/local/lib/gkrellm2/plugins/
	-	/usr/lib/gkrellm2/plugins/
	
-	alternatively, run gkrellm -p libgkrellm-powermeter.so to test the plugin temporarily

Run:

-	In gkrellm's settings: enable the plugin
