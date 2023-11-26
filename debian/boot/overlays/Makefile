# Makefile for compiling all DTS files in the current directory to DTBO


DTC := dtc


DTC_FLAGS := -q -I dts -O dtb -o

DTS_FILES := $(wildcard *.dts)


DTBO_FILES := $(DTS_FILES:.dts=.dtbo)


all: $(DTBO_FILES)


%.dtbo: %.dts
	$(DTC) $(DTC_FLAGS) $@ $<


clean:
	rm -f *.dtbo

.PHONY: all clean
