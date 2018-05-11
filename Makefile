

generate:
	build/sp/strips scripts/generate_js.js | clang-format > src/script-data.h