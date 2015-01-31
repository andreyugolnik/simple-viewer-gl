BUILD_DIR=.build

all:
	@echo "Usage:"
	@echo "    make <linux | osx>"
	@echo "    make <clean>"

linux:
	$(shell if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi )
	cd $(BUILD_DIR) ; cmake .. -DCMAKE_CXX_FLAGS='-Wall -Wextra -O2' ; make ; cd ..
	cp $(BUILD_DIR)/sviewgl .

osx:
	$(shell if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi )
	cd $(BUILD_DIR) ; cmake .. -DCMAKE_CXX_FLAGS='-Wall -Wextra -O2' ; make ; cd ..

clean:
	rm -fr $(BUILD_DIR)
	rm -fr sviewgl

