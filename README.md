Build: 

-	cmake CMakeLists.txt
-	make -j$(nproc)

Install:

-	strip libgkrellm-powermeter.so
-	mv libgkrellm-powermeter.so gkrellm-powermeter.so
-	copy gkrellm-powermeter.so into one of the following folders:
	-	~/.gkrellm2/plugins/
	-	/usr/local/lib/gkrellm2/plugins/
	-	/usr/lib/gkrellm2/plugins/
	
-	alternatively, run gkrellm -p gkrellm-powermeter.so to test the plugin temporarily

Run:

-	In gkrellm's settings: enable the plugin
