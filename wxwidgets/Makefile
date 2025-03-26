CFLAGS = -I/usr/local/lib/wx/include/gtk3-unicode-3.2 -I/usr/local/include/wx-3.2 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread
LFLAGS = -L/usr/local/lib -pthread   -lwx_gtk3u_xrc-3.2 -lwx_gtk3u_html-3.2 -lwx_gtk3u_qa-3.2 -lwx_gtk3u_core-3.2 -lwx_baseu_xml-3.2 -lwx_baseu_net-3.2 -lwx_baseu-3.2 

test: main.cpp  Makefile
	g++ -o wx_example main.cpp `wx-config --cxxflags --libs`
	#g++ main.cpp -o test -I. $(CFLAGS) $(LFLAGS) -g -lm

clean:
	rm test


#g++ -o wx_example main.cpp `wx-config --cxxflags --libs`
#g++ `wx-config --cxxflags` -o test *.cpp `wx-config --libs`
#-I/usr/local/lib/wx/include/gtk3-unicode-3.2 -I/usr/local/include/wx-3.2 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread
#-L/usr/local/lib -pthread   -lwx_gtk3u_xrc-3.2 -lwx_gtk3u_html-3.2 -lwx_gtk3u_qa-3.2 -lwx_gtk3u_core-3.2 -lwx_baseu_xml-3.2 -lwx_baseu_net-3.2 -lwx_baseu-3.2 

