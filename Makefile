.DEFAULT_GOAL := help

.PHONY: help setup build flash render check clean

help:
	@echo "Voyager firmware project"
	@echo ""
	@echo "  make setup   Fetch pinned build and rendering dependencies"
	@echo "  make build   Compile the firmware into artifacts/"
	@echo "  make flash   Build and flash the firmware with ZSA Zapp"
	@echo "  make render  Generate the printable SVG into artifacts/"
	@echo "  make check   Compile firmware and regenerate the SVG"
	@echo "  make clean   Remove disposable build state"

setup:
	@./scripts/setup.sh

build:
	@./scripts/build.sh

flash: build
	@./scripts/flash.sh

render:
	@./scripts/render.sh

check: build render

clean:
	@./scripts/clean.sh
