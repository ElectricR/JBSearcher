

main: main.cpp
	clang++ main.cpp `wx-config --cxxflags --libs` -std=c++20 -Wall -Wextra -Wextra-semi -Wl,-rpath="./lib" -O3 -o main

main2: main.cpp
	clang++ -I/usr/local/lib/wx/include/gtk3-unicode-3.1 -I/usr/local/include/wx-3.1 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread -L./lib -lwx_gtk3u_xrc-3.1 -lwx_gtk3u_html-3.1 -lwx_gtk3u_qa-3.1 -lwx_gtk3u_core-3.1 -lwx_baseu_xml-3.1 -lwx_baseu_net-3.1 -lwx_baseu-3.1	-static -std=c++20 -Wall -Wextra -Wextra-semi main.cpp -O3 -o main

run: main
	./main
