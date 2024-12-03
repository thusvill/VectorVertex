# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  VectorVertex_config = debug
  VVEditor_config = debug

else ifeq ($(config),release)
  VectorVertex_config = release
  VVEditor_config = release

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := VectorVertex VVEditor

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

VectorVertex:
ifneq (,$(VectorVertex_config))
	@echo "==== Building VectorVertex ($(VectorVertex_config)) ===="
	@${MAKE} --no-print-directory -C VectorVertex -f Makefile config=$(VectorVertex_config)
endif

VVEditor: VectorVertex
ifneq (,$(VVEditor_config))
	@echo "==== Building VVEditor ($(VVEditor_config)) ===="
	@${MAKE} --no-print-directory -C Editor -f Makefile config=$(VVEditor_config)
endif

clean:
	@${MAKE} --no-print-directory -C VectorVertex -f Makefile clean
	@${MAKE} --no-print-directory -C Editor -f Makefile clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   VectorVertex"
	@echo "   VVEditor"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"