

generate:
	build/sp/sp scripts/generate_js.js | clang-format > src/script-data.h