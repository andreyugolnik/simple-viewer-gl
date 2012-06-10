BUILD_DIR=build

all:
	$(shell [[ ! -d $(BUILD_DIR) ]] && mkdir $(BUILD_DIR) )
	cd $(BUILD_DIR) ; cmake .. -DCMAKE_CXX_COMPILER=clang++ ; make ; cd ..

clean:
	rm -fr $(BUILD_DIR)

