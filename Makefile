BUILD_DIR=.build

all:
	$(shell if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi )
	cd $(BUILD_DIR) ; cmake ../projects/linux ; make ; cd ..
	#cd $(BUILD_DIR) ; cmake .. -DCMAKE_CXX_COMPILER=clang++ ; make ; cd ..

osx:
	$(shell if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi )
	cd $(BUILD_DIR) ; cmake ../projects/osx ; make ; cd ..

clean:
	rm -fr $(BUILD_DIR)
	rm -fr sviewgl

