#release
GPP = clang++ -I C:\libs -Dstage_loader_path=. -I include/nvidia -I .
#debug for address sanitizer
#GPP = clang-cl /MT -fsanitize=address  /std:c++latest

#-Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror

CFLAGS = -ferror-limit=3 -w -I .  -std=c++2a -I include #-O3 -march=native -mtune=native \
  #-fsanitize=address -fno-omit-frame-pointer


MAIN = Source.cpp
FAN_OBJECT_FOLDER = 

BASE_PATH = 

ifeq ($(OS),Windows_NT)
  BASE_PATH += lib/fan/
    #                        magic - replace / with \ thanks to windows
  FAN_OBJECT_FOLDER = $(subst /,\,$(BASE_PATH))
	FAN_INCLUDE_PATH = C:/libs/fan/include
  CFLAGS += -I C:/libs/fan/include/fan/imgui -I C:\libs\fan\include\baseclasses -I C:/libs/fan/include/imgui -I C:/libs/fan/src/libwebp -I C:/libs/fan/src/libwebp/src C:/libs/fan/lib/libwebp/libwebp.a C:/libs/fan/lib/opus/libopus.a
	CFLAGS += -DFAN_INCLUDE_PATH=$(FAN_INCLUDE_PATH) -DWITCH_INCLUDE_PATH=C:/libs/WITCH
	CFLAGS += lib/libuv/uv_a.lib
else
  BASE_PATH += lib/fan/
  FAN_OBJECT_FOLDER += $(BASE_PATH)
  CFLAGS += -lX11 -lXrandr -L /usr/local/lib -lopus -L/usr/lib/x86_64-linux-gnu/libGL.so.1 -lwebp -ldl
  CFLAGS += -DFAN_INCLUDE_PATH=/usr/include/ -I /usr/include/fan/imgui
	CFLAGS += -DWITCH_INCLUDE_PATH=/usr/include/WITCH
endif

CFLAGS += -Dfan_build_pch

PCH_PATH = pch.h
CFLAGS += -Dfan_pch=\"$(PCH_PATH)\"

debug:
	$(GPP) $(CFLAGS)  -include-pch $(PCH_PATH).gch  $(MAIN) lib/imgui/libimgui.lib

release:
	$(GPP) $(CFLAGS) $(MAIN) -fdata-sections -ffunction-sections -Wl,--gc-sections -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -O3 lib/imgui/libimgui.lib

clean:
	rm -f a.out
