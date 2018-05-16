

generate:
	build/strips/zap scripts/generate_js.js | clang-format > src/script-data.h