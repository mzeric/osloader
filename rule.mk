#COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
#CC = gcc
#OUTPUT_OPTION = -o $@

DEPS_PATH=.deps

CFLAGS += -I$(HEADER_DIR) -MMD -MP -MF $(DEPS_PATH)/$(@:.o=.d)
CXXFLAGS += -I$(HEADER_DIR) -MMD -MP -MF $(DEPS_PATH)/$(@:.o=.d)


%.o: %.c
	$(COMPILE.c)  $(OUTPUT_OPTION) $<
%.o: %.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
%.o: %.S
	$(COMPILE.c) -c -o $@ $<
%.o: %.s
	$(COMPILE.c) -c -o $@ $<
clean:
	$(RM) $(OBJS)

.PHONY: prepare all df size
prepare:
	$(if $(wildcard $(DEPS_PATH)),,mkdir -p $(DEPS_PATH))
	
df:; @df -h .| awk 'NR == 2 { printf( "%s spaces available\n", $$4 ) }'
size:; size a

$(foreach dep_file,$(OBJS:.o=.d),$(eval -include $(DEPS_PATH)/$(dep_file)))
