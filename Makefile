BUILD_DIR=.build

all:
	@echo "Usage:"
	@echo "    make <release | debug>"
	@echo "    make <clean>"

release:
	$(shell if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi )
	cd $(BUILD_DIR) ; cmake .. -DCMAKE_CXX_FLAGS='-Wall -Wextra -O2' ; make ; cd ..
	cp $(BUILD_DIR)/sviewgl .

debug:
	$(shell if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi )
	cd $(BUILD_DIR) ; cmake .. -DCMAKE_CXX_FLAGS='-Wall -Wextra -O0 -g' ; make ; cd ..
	cp $(BUILD_DIR)/sviewgl .

clean:
	rm -fr $(BUILD_DIR)
	rm -fr sviewgl

