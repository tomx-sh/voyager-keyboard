.DEFAULT_GOAL := help

.PHONY: help setup build build-native-fn flash flash-native-fn render check clean

help:
	@echo "Voyager firmware project"
	@echo ""
	@echo "  make setup   Fetch pinned build and rendering dependencies"
	@echo "  make build   Compile the firmware into artifacts/"
	@echo "  make build-native-fn   Compile the same firmware with a -native-fn filename"
	@echo "  make flash   Build and flash the firmware with ZSA Zapp"
	@echo "  make flash-native-fn   Build and flash the -native-fn firmware artifact"
	@echo "  make render  Generate the printable SVG into artifacts/"
	@echo "  make check   Compile firmware and regenerate the SVG"
	@echo "  make clean   Remove disposable build state"

setup:
	@./scripts/setup.sh

build:
	@./scripts/build.sh

build-native-fn:
	@./scripts/build.sh native-fn

flash: build
	@./scripts/flash.sh

flash-native-fn: build-native-fn
	@./scripts/flash.sh native-fn

render:
	@./scripts/render.sh

check: build render

clean:
	@./scripts/clean.sh
